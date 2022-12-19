#include "mainwindow.hh"
#include "./ui_mainwindow.h"
#include "RayTracing/Camera.hh"
#include "RayTracing/Components.hh"
#include "RayTracing/Random.hh"

#include <Eigen/Dense>
#include <Tracy.hpp>
#include <spdlog/spdlog.h>
#include <taskflow/taskflow.hpp>

#include <execution>
#include <ranges>
#include <span>

using namespace RayTracing;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    image_ = QImage{ui->eImage->size(), QImage::Format_ARGB32};
    image_.fill(Qt::GlobalColor::black);
    ui->eImage->setPixmap(QPixmap::fromImage(image_));

    connect(ui->bRender, &QPushButton::clicked, this, &MainWindow::render);

    // Add a single sphere to the registry
    {
        const auto sphere = registry_.create();
        registry_.emplace<Components::Sphere>(sphere, Point3{0.0, 0.0, -1.0}, 0.5);
    }
    {
        const auto sphere = registry_.create();
        registry_.emplace<Components::Sphere>(sphere, Point3{0.0, -100.5, -1.0}, 100.0);
    }

    connect(this, &MainWindow::tileFinished, this, [&] {
        tilesFinished++;
        ui->eProgress->setValue(static_cast<int>(tilesFinished));
    });
}

MainWindow::~MainWindow() { delete ui; }

QRgb color(const Color &color) {
    Color clamped = color /*.cwiseSqrt()*/.cwiseMin(0.999).cwiseMax(0.0);
    return qRgb(255 * clamped.x(), 255 * clamped.y(), 255 * clamped.z());
}

void MainWindow::render() {
    ZoneScoped;

    ui->eProgress->setValue(0);
    ui->eProgress->setFormat("%p%");

    image_ = QImage{ui->eImage->size(), QImage::Format_ARGB32};
    image_.fill(Qt::GlobalColor::black);

    const std::size_t imageWidth  = image_.size().width();
    const std::size_t imageHeight = image_.size().height();

    // Image data
    std::span<QRgb> pixels{reinterpret_cast<QRgb *>(image_.bits()), imageWidth * imageHeight};

    // Camera
    Camera camera;

    tf::Executor executor;
    tf::Taskflow taskflow;

    constexpr std::size_t tileSize = 32;
    std::size_t tileCount          = 0;
    std::size_t samplesPerPixel    = 100;
    int32_t maximumDepth           = 50;

    // Render
    for (std::size_t j = 0; j < imageHeight; j += tileSize) {
        for (std::size_t i = 0; i < imageWidth; i += tileSize) {
            tileCount++;
            taskflow.emplace([&, i_start = i, j_start = j] {
                RandomVector randomVector{-1.0, 1.0};
                RandomDouble randomDouble{0.0, 1.0};

                std::size_t i_end = std::min(i_start + tileSize, imageWidth);
                std::size_t j_end = std::min(j_start + tileSize, imageHeight);

                std::ranges::for_each(std::views::iota(i_start, i_end), [&](std::size_t i) {
                    const auto j_range = std::ranges::iota_view{j_start, j_end};

                    std::for_each(std::execution::par_unseq, j_range.begin(), j_range.end(), [&](std::size_t j) {
                        const std::size_t n           = i + (imageHeight - j - 1) * imageWidth;
                        const Color initialPixelColor = Color::Zero();

                        std::vector<double> uList(samplesPerPixel), vList(samplesPerPixel);
                        std::generate(std::execution::par_unseq, uList.begin(), uList.end(),
                                      [&] { return (i + randomDouble()) / (imageWidth - 1.); });
                        std::generate(std::execution::par_unseq, vList.begin(), vList.end(),
                                      [&] { return (j + randomDouble()) / (imageHeight - 1.); });

                        Color pixelColor =
                            std::transform_reduce(uList.begin(), uList.end(), vList.begin(), initialPixelColor,
                                                  std::plus{}, [&](double u, double v) {
                                                      const Ray ray = camera.getRay(u, v);

                                                      return rayColor(randomVector, ray, maximumDepth);
                                                  });

                        /*
                        Color pixelColor = Color::Zero();
                        for (std::size_t sample = 0; sample < samplesPerPixel; sample++) {

                            const double u = (i + randomDouble()) / (imageWidth - 1.);
                            const double v = (j + randomDouble()) / (imageHeight - 1.);

                            const Ray ray = camera.getRay(u, v);

                            pixelColor += rayColor(randomVector, ray, maximumDepth);
                        }
                        */
                        pixels[n] = color(pixelColor / samplesPerPixel);

                        /*
                        if ((timer_.elapsed() - last_time) > 10) {
                            last_time = timer_.elapsed();

                            ui->eImage->setPixmap(QPixmap::fromImage(image_));
                            QCoreApplication::processEvents(QEventLoop::AllEvents);
                        }
                        */
                    });

                    // constexpr std::counted_iterator<std::size_t *> j_iterator{j_start, j_end};

                    /*
                    std::for_each(std::counted_iterator(j_start), std::counted_iterator(j_end), [&](std::size_t j) {
                    });
                    */

                    /*
                    for (std::size_t j = j_start; j < j_end; j++) {

                    }
                    */
                });

                emit tileFinished();

                /*
                ui->eImage->setPixmap(QPixmap::fromImage(image_));
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                */
            });
        }
    }

    ui->eProgress->setMaximum(static_cast<int>(tileCount));
    tilesFinished = 0;
    ui->eProgress->setValue(0);

    const auto start = std::chrono::high_resolution_clock::now();
    auto future      = executor.run(taskflow);
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::milliseconds(1));
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    } while (status != std::future_status::ready);
    const auto end       = std::chrono::high_resolution_clock::now();
    const double time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    /*
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<double> rng(0, 1);

    std::size_t count = 10'000'000;
    timer_.restart();
    for (std::size_t i = 0; i < count; i++) {
        volatile double rnd = rng(gen);
        (void)rnd;
    }
    const double time2 = timer_.nsecsElapsed();
    spdlog::info("random generationg took {:.3} ns/number", time2 / count);
    */

    spdlog::info("Ray tracing took {:.3} us/pixel ({:.3} ns/sample)", time_ns / 1000. / (imageWidth * imageHeight),
                 time_ns / (imageWidth * imageHeight) / samplesPerPixel);

    ui->eImage->setPixmap(QPixmap::fromImage(image_));
    image_.save("test.png");
    ui->eProgress->setValue(ui->eProgress->maximum());
}

Color MainWindow::rayColor(RandomVector &randomVectorGenerator, const RayTracing::Ray &ray, int32_t depth) const {
    // Limit number of ray bounce, stop gathering light
    if (depth <= 0) { return Color::Zero(); }

    const auto closestHit = hit(ray, 0, std::numeric_limits<double>::infinity());
    if (closestHit) {
        // Object color
        Vector3 direction = closestHit->normal_ - randomVectorInSphere(randomVectorGenerator);
        // Recursively gather light
        return 0.5 * rayColor(randomVectorGenerator, Ray{closestHit->point_, direction}, depth - 1);
        // Normal color
        //        return 0.5 * (closestHit->normal_ + Vector3::Ones());
    }

    // Background color
    const auto t = 0.5 * (ray.direction().normalized().y() + 1.0);
    return (1.0 - t) * Color::Ones() + t * Color{0.5, 0.7, 1.0};
}

std::optional<RayTracing::HitRecord> MainWindow::hit(const RayTracing::Ray &ray, double tMinimum,
                                                     double tMaximum) const {
    std::optional<HitRecord> closestHit;

    auto sphere_view = registry_.view<const Components::Sphere>();

    sphere_view.each([&](const Components::Sphere &sphere) {
        const auto hitRecord = sphere.hit(ray, tMinimum, tMaximum);

        if (hitRecord) {
            tMaximum   = hitRecord->t_; // Update maximum t
            closestHit = *hitRecord;    // Update closestHit
        }
    });

    return closestHit;
}
