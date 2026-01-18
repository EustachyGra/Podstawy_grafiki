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
    void buttonSelected();
    void createMatrix();
    void resetvalues();
signals:
    void variables(int tx, int ty, int sx, int sy, int a, int shx, int shy);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
