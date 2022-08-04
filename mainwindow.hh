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

  private slots:
    void render();

    RayTracing::Color rayColor(RayTracing::RandomVector &randomVectorGenerator, const RayTracing::Ray &ray,
                               int32_t depth);
    std::optional<RayTracing::HitRecord> hit(const RayTracing::Ray &ray, double tMinimum, double tMaximum);

  private:
    Ui::MainWindow *ui;

    QImage image_;
    QElapsedTimer timer_;

    entt::registry registry_;
};
