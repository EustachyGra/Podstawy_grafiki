#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void createMatrix();
    void resetvalues();
signals:
    void variables(float tx, float ty, float tz, float ax, float ay, float az, float sx, float sy, float sz);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
