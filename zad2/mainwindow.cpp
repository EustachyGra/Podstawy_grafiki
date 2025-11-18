#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->LineButton,&QPushButton::released,ui->wEkran, &Ekran::lineChange);
    connect(ui->CircButton,&QPushButton::released,ui->wEkran, &Ekran::circChange);
    connect(ui->Curve,&QComboBox::currentIndexChanged,ui->wEkran, &Ekran::curveChange);
    connect(ui->Color,&QPushButton::released, ui->wEkran, &Ekran::colorDialog);
    connect(ui->wEkran,&Ekran::ColorChange, this, &MainWindow::changeButtonColor);
    connect(ui->wEkran,&Ekran::currentType, this, &MainWindow::buttonSelected);
    connect(ui->FillButton,&QPushButton::released, ui->wEkran, &Ekran::fillChange);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeButtonColor(QColor color)
{
    QString qss = QString("background-color: %1").arg(color.name());
    ui->Color->setStyleSheet(qss);
}

void MainWindow::buttonSelected(int i)
{
    qDebug() << i;
    ui->LineButton->setFlat(false);
    ui->CircButton->setFlat(false);
    ui->FillButton->setFlat(false);

    ui->Curve->blockSignals(true);
    ui->Curve->setCurrentIndex(0);
    ui->Curve->blockSignals(false);

    switch (i) {
    case 1:
        ui->LineButton->setFlat(true);
        break;
    case 2:
        ui->CircButton->setFlat(true);
        break;
    case 3:
        ui->Curve->blockSignals(true);
        ui->Curve->setCurrentIndex(1);
        ui->Curve->blockSignals(false);
        break;
    case 4:
        ui->Curve->blockSignals(true);
        ui->Curve->setCurrentIndex(2);
        ui->Curve->blockSignals(false);
        break;
    case 5:
        ui->Curve->blockSignals(true);
        ui->Curve->setCurrentIndex(3);
        ui->Curve->blockSignals(false);
        break;
    case 6:
        ui->FillButton->setFlat(true);
        break;
    default:
        break;
    }
}

