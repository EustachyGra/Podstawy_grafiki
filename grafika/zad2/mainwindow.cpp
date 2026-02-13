#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tx, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->ty, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->tz, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->ax, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->ay, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->az, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->sx, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->sy, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(ui->sz, &QSlider::valueChanged, this, &MainWindow::createMatrix);
    connect(this, &MainWindow::variables, ui->wEkran, &Ekran::drawModel);
    connect(ui->reset, &QPushButton::released, this, &MainWindow::resetvalues);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMatrix()
{
    float tx = ui->tx->value();
    float ty = ui->ty->value();
    float tz = ui->tz->value();
    float ax = ui->ax->value();
    float ay = ui->ay->value();
    float az = ui->az->value();
    float sx = ui->sx->value();
    float sy = ui->sy->value();
    float sz = ui->sz->value();

    emit variables(tx,ty,tz,ax,ay,az,1+sx/10,1+sy/10,1+sz/10);
}

void MainWindow::resetvalues()
{
    ui->tx->setValue(0);
    ui->ty->setValue(0);
    ui->tz->setValue(0);
    ui->ax->setValue(0);
    ui->ay->setValue(0);
    ui->az->setValue(0);
    ui->sx->setValue(0);
    ui->sy->setValue(0);
    ui->sz->setValue(0);
}

