#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->layer, &QComboBox::currentIndexChanged, ui->ekran, &Ekran::LayerChanged);
    connect(ui->ekran, &Ekran::CurrentLayerType, ui->mode, &QComboBox::setCurrentIndex);
    connect(ui->mode, &QComboBox::currentIndexChanged, ui->ekran, &Ekran::SetCurrentType);
    connect(ui->horizontalSlider, &QSlider::valueChanged, ui->ekran, &Ekran::SetAlpha);
    connect(ui->ekran, &Ekran::CurrentLayerAlpha, ui->horizontalSlider, &QSlider::setValue);
}

MainWindow::~MainWindow()
{

}
void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Wybierz obraz",
        "",
        "Obrazy (*.png *.jpg *.bmp *.jpeg);;Wszystkie pliki (*.*)"
        );

    // jeśli użytkownik anulował
    if (filename.isEmpty())
        return;

    qDebug() << "Wczytuje:" << filename;

    qDebug() << filename;
    ui->ekran->loadImg(filename);

    ui->layer->addItem(QFileInfo(filename).fileName());
    ui->layer->setCurrentIndex(ui->layer->count()-1);
    ui->mode->setCurrentIndex(0);
}

