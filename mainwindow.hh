#pragma once

#include "RayTracing/HitRecord.hh"
#include "RayTracing/Random.hh"
#include "RayTracing/Ray.hh"

#include <entt/entt.hpp>

#include <QElapsedTimer>
#include <QMainWindow>

#include <optional>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  signals:
    void tileFinished();

  private slots:
    void render();

    RayTracing::Color rayColor(RayTracing::RandomVector &randomVectorGenerator, const RayTracing::Ray &ray,
                               int32_t depth) const;
    std::optional<RayTracing::HitRecord> hit(const RayTracing::Ray &ray, double tMinimum, double tMaximum) const;

  private:
    Ui::MainWindow *ui;

    QImage image_;
    QElapsedTimer timer_;

    entt::registry registry_;

    std::size_t tilesFinished = 0;
};
