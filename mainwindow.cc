#include "mainwindow.hh"
#include "./ui_mainwindow.h"
#include "ray.hh"

#include <Eigen/Dense>
#include <Tracy.hpp>
#include <fmt/format.h>

#include <span>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    image_ = QImage{ui->eImage->size(), QImage::Format_RGB888};
    image_.fill(Qt::GlobalColor::black);
    ui->eImage->setPixmap(QPixmap::fromImage(image_));

    connect(ui->bRender, &QPushButton::clicked, this, &MainWindow::render);
}

MainWindow::~MainWindow() { delete ui; }

using namespace RayTracing;

QRgb color(const Color &color) { return qRgb(255 * color.x(), 255 * color.y(), 255 * color.z()); }

bool hit_sphere(const Point3 &center, double radius, const Ray &r) {
    const auto oc = r.origin() - center;

    const auto a = r.direction().dot(r.direction());
    const auto b = 2.0 * oc.dot(r.direction());
    const auto c = oc.dot(oc) - radius * radius;

    const auto discriminant = b * b - 4 * a * c;
    return discriminant > 0;
}

Color rayColor(const Ray &r) {
    ZoneScoped;
    if (hit_sphere(Point3{0, 0, -1}, 0.5, r)) { return Color{1.0, 0., 0.}; }

    const Vector3 unitDirection = r.direction().normalized();

    const auto t = 0.5 * (unitDirection.y() + 1.0);

    return (1.0 - t) * Color{1.0, 1.0, 1.0} + t * Color{0.5, 0.7, 1.0};
}

void MainWindow::render() {
    ZoneScoped;

    ui->eProgress->setValue(0);
    ui->eProgress->setFormat("%p%");

    image_ = QImage{ui->eImage->size(), QImage::Format_ARGB32};
    image_.fill(Qt::GlobalColor::black);

    const std::size_t imageWidth  = image_.size().width();
    const std::size_t imageHeight = image_.size().height();
    ui->eProgress->setMaximum(static_cast<int>(imageWidth * imageHeight));

    // Image data
    std::span<QRgb> pixels{reinterpret_cast<QRgb *>(image_.bits()), imageWidth * imageHeight};

    // Camera
    const double aspectRatio = 16.0 / 9.0;

    const double viewportHeight = 2.0;
    const double viewportWidth  = aspectRatio * viewportHeight;
    const double focalLength    = 1.0;

    const RayTracing::Point3 origin{0., 0., 0};
    const RayTracing::Vector3 horizontal{viewportWidth, 0., 0.};
    const RayTracing::Vector3 vertical{0, viewportHeight, 0};
    const auto lowerLeftCorner = origin - horizontal / 2. - vertical / 2. - RayTracing::Vector3{0., 0., focalLength};

    // Render

    timer_.start();
    for (std::size_t j = imageHeight - 1; j <= imageHeight; --j) { // works thanks to the underflow of std::size_t
        for (std::size_t i = 0; i < imageWidth; ++i) {
            // ZoneScopedN("Pixel");

            const std::size_t n = i + (imageHeight - j - 1) * imageWidth;

            const double u = i / (imageWidth - 1.);
            const double v = j / (imageHeight - 1.);

            const RayTracing::Ray r{origin, lowerLeftCorner + u * horizontal + v * vertical - origin};

            pixels[n] = color(rayColor(r));

            /*
            if (n % 10000 == 0) {
                ui->eImage->setPixmap(QPixmap::fromImage(image_));
                ui->eProgress->setValue(static_cast<int>(n));
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
            */
        }
    }
    const auto time = timer_.nsecsElapsed();
    qDebug() << "Took" << time / (1. * imageWidth * imageHeight) << "ns/op";

    ui->eImage->setPixmap(QPixmap::fromImage(image_));
    ui->eProgress->setValue(ui->eProgress->maximum());
}
