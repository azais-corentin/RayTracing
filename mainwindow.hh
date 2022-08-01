#pragma once

#include <QElapsedTimer>
#include <QMainWindow>

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

  private:
    Ui::MainWindow *ui;

    QImage image_;
    QElapsedTimer timer_;
};
