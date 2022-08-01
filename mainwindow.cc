#include "mainwindow.hh"
#include "./ui_mainwindow.h"

#include <Eigen/Dense>

#include <span>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    image_ = QImage{ui->eImage->size(), QImage::Format_RGB888};
    image_.fill(Qt::GlobalColor::black);
    ui->eImage->setPixmap(QPixmap::fromImage(image_));

    connect(ui->bRender, &QPushButton::clicked, this, &MainWindow::start_render);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::start_render() {

    ui->eProgress->setValue(0);
    ui->eProgress->setFormat("%p%");

    image_ = QImage{ui->eImage->size(), QImage::Format_ARGB32};
    image_.fill(Qt::GlobalColor::black);

    const std::size_t width  = image_.size().width();
    const std::size_t height = image_.size().height();

    ui->eProgress->setMaximum(static_cast<int>(width * height));

    qDebug() << "Image size:" << image_.size();

    std::span<QRgb> pixel{reinterpret_cast<QRgb *>(image_.bits()), width * height};

    std::size_t n = 0;
    Eigen::Vector3d center{width / 2., height / 2., 0.};

    timer_.start();
    for (std::size_t j = height - 1; j <= height; --j) { // works thanks to the underflow of std::size_t
        for (std::size_t i = 0; i < width; ++i) {
            const std::size_t xy = i + (height - j - 1) * width;

            const uint8_t r = 255 * i / (width - 1.);
            const uint8_t g = 255 * j / (height - 1.);
            const uint8_t b = 255 * 0.25;

            pixel[xy] = qRgb(r, g, b);

            // image_.setPixel(static_cast<int>(i), static_cast<int>(j), qRgb(r, g, b));

            if (xy % 100 == 0) {
                // qDebug() << "distance:" << distance;
                ui->eImage->setPixmap(QPixmap::fromImage(image_));
                ui->eProgress->setValue(static_cast<int>(n));
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }

            // std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
    }

    const auto time = timer_.nsecsElapsed();
    qDebug() << "Took" << time / (1. * width * height) << "ns/op";

    ui->eImage->setPixmap(QPixmap::fromImage(image_));
    ui->eProgress->setValue(static_cast<int>(n));
}
