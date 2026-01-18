#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Polygon,&QComboBox::currentIndexChanged,ui->wEkran, &Ekran::polygonChange);
    connect(ui->alfa,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->tx,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->shx,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->ty,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->shy,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->sx,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->sy,&QSlider::valueChanged,this,&MainWindow::createMatrix);
    connect(ui->wEkran,&Ekran::oseted,this,&MainWindow::buttonSelected);
    connect(ui->origin,&QPushButton::released,ui->wEkran,&Ekran::originset);
    connect(this,&MainWindow::variables,ui->wEkran,&Ekran::applyMatrix);
    connect(ui->reset,&QPushButton::released,this,&MainWindow::resetvalues);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonSelected()
{
    ui->origin->setFlat(false);
    ui->Polygon->blockSignals(true);
    ui->Polygon->setCurrentIndex(0);
    ui->Polygon->blockSignals(false);
}
void MainWindow::createMatrix()
{
    int tx = ui->tx->value();
    int ty = ui->ty->value();
    int sx = ui->sx->value();
    int sy = ui->sy->value();
    int a = ui->alfa->value();
    int shx = ui->shx->value();
    int shy = ui->shy->value();

    emit variables(tx,ty,sx,sy,a,shx,shy);
}

void MainWindow::resetvalues()
{
    ui->tx->setValue(0);
    ui->ty->setValue(0);
    ui->sx->setValue(1);
    ui->sy->setValue(1);
    ui->alfa->setValue(0);
    ui->shx->setValue(0);
    ui->shy->setValue(0);
}

