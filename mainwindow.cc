#include "mainwindow.hh"
#include "./ui_mainwindow.h"
#include "RayTracing/Camera.hh"
#include "RayTracing/Components.hh"
#include "RayTracing/Random.hh"

#include <Eigen/Dense>
#include <Tracy.hpp>
#include <spdlog/spdlog.h>
#include <taskflow/taskflow.hpp>

#include <span>

using namespace RayTracing;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    image_ = QImage{ui->eImage->size(), QImage::Format_RGB888};
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
}

MainWindow::~MainWindow() { delete ui; }

QRgb color(const Color &color) {
    Color clamped = color.cwiseMin(0.999).cwiseMax(0.0);
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

    constexpr std::size_t tileSize = 64;
    std::size_t tileCount          = 0;
    std::size_t samplesPerPixel    = 10;
    int32_t maximumDepth           = 10;

    // Render
    for (std::size_t j = 0; j < imageHeight; j += tileSize) {
        for (std::size_t i = 0; i < imageWidth; i += tileSize) {
            tileCount++;
            taskflow.emplace([&, i_start = i, j_start = j] {
                RandomVector randomVector{-1.0, 1.0};
                RandomDouble randomDouble;

                std::size_t i_end = std::min(i_start + tileSize, imageWidth);
                std::size_t j_end = std::min(j_start + tileSize, imageHeight);

                for (std::size_t i = i_start; i < i_end; i++) {
                    for (std::size_t j = j_start; j < j_end; j++) {
                        const std::size_t n = i + (imageHeight - j - 1) * imageWidth;
                        Color pixelColor    = Color::Zero();

                        for (std::size_t sample = 0; sample < samplesPerPixel; sample++) {

                            const double u = (i + randomDouble()) / (imageWidth - 1.);
                            const double v = (j + randomDouble()) / (imageHeight - 1.);

                            const Ray ray = camera.getRay(u, v);

                            pixelColor += rayColor(randomVector, ray, maximumDepth);
                        }
                        pixels[n] = color(pixelColor / samplesPerPixel);

                        /*
                        if ((timer_.elapsed() - last_time) > 10) {
                            last_time = timer_.elapsed();

                            ui->eImage->setPixmap(QPixmap::fromImage(image_));
                            ui->eProgress->setValue(static_cast<int>(n));
                            QCoreApplication::processEvents(QEventLoop::AllEvents);
                        }
                        */
                    }
                }

                // emit tileReady();

                /*
                std::unique_lock lock(mutex);
                ui->eImage->setPixmap(QPixmap::fromImage(image_));
                ui->eProgress->setValue(ui->eProgress->value() + 1);
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                */
            });
        }
    }

    ui->eProgress->setMaximum(static_cast<int>(tileCount));
    ui->eProgress->setValue(0);

    const auto start = std::chrono::high_resolution_clock::now();
    auto future      = executor.run(taskflow);
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::nanoseconds(0));
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
    ui->eProgress->setValue(ui->eProgress->maximum());
}

Color MainWindow::rayColor(RandomVector &randomVectorGenerator, const RayTracing::Ray &ray, int32_t depth) {
    ZoneScoped;

    // Limit number of ray bounce, stop gathering light
    if (depth <= 0) { return Color::Zero(); }

    const auto closestHit = hit(ray, 0, std::numeric_limits<double>::infinity());

    if (closestHit) {
        // Object color
        Point3 target = closestHit->point_ + closestHit->normal_ + randomVectorInSphere(randomVectorGenerator);
        // Recursively gather light
        return 0.5 * rayColor(randomVectorGenerator, Ray{closestHit->point_, target - closestHit->point_}, depth - 1);
        // Normal color
        // return 0.5 * (closestHit->normal_ + Vector3::Ones());
    }

    // Background color
    const auto t = 0.5 * (ray.direction().normalized().y() + 1.0);
    return (1.0 - t) * Color::Ones() + t * Color{0.5, 0.7, 1.0};
}

std::optional<RayTracing::HitRecord> MainWindow::hit(const RayTracing::Ray &ray, double tMinimum, double tMaximum) {
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
