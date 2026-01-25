#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pusty_reset, &QPushButton::released, ui->ekran, &Ekran::resetPkt);
    connect(ui->tekstura_reset, &QPushButton::released, ui->tekstura,&Ekran::resetPkt);
    connect(ui->rob,&QPushButton::released,this,&MainWindow::rob);
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

    if (filename.isEmpty())
        return;

    qDebug() << "Wczytuje:" << filename;

    qDebug() << filename;
    ui->tekstura->loadImg(filename);

}

void MainWindow::rob()
{
    if(ui->ekran->triangle.size()<3 || ui->tekstura->triangle.size()<3)
        return;
    int minY,maxY;
    std::vector<QPoint> vec = ui->ekran->triangle;
    QPoint tmp = ui->ekran->getMinYMaxY();
    minY=tmp.x();
    maxY=tmp.y();
    for(int y = minY; y<=maxY; y++)
    {
        std::vector<QPoint> crossPoints;
        for(int i=0 ; i<vec.size();i++)
        {
            if(isLineCrossed(vec[i],vec[(i+1)%vec.size()],y))
                crossPoints.push_back(findCrossPoint(vec[i],vec[(i+1)%vec.size()],y));
        }
        std::sort(crossPoints.begin(),crossPoints.end(),[](const QPoint &a, const QPoint &b){return a.x()<b.x();});
        if(crossPoints.size()!=2)
            continue;
        for(int x = crossPoints[0].x(); x<=crossPoints[1].x(); x++)
        {
            double u,v,w;
            double XA = vec[0].x();
            double YA = vec[0].y();

            double XB = vec[1].x();
            double YB = vec[1].y();

            double XC = vec[2].x();
            double YC = vec[2].y();

            double mianownik = (XB - XA) * (YC - YA) - (YB - YA) * (XC - XA);
            v = ((x - XA) * (YC - YA) - (y - YA) * (XC - XA)) / mianownik;
            w = ((XB - XA) * (y - YA) - (YB - YA) * (x - XA)) / mianownik;
            u = 1- v -w ;
            int r,g,b,a;
            ui->tekstura->getRGBA(u,v,w,r,g,b,a);
            ui->ekran->drawPixel(x,y,r,g,b,a);
        }

        repaint();
    }
    ui->ekran->triangle.clear();
}

bool MainWindow::isLineCrossed(QPoint l1, QPoint l2, int y)
{
    if (l1.y() == l2.y())
        return false;

    return ( (y >= std::min(l1.y(),l2.y())) && (y < std::max(l1.y(),l2.y())) );
}
QPoint MainWindow::findCrossPoint(QPoint l1, QPoint l2, int y)
{
    int x1 = l1.x();
    int y1 = l1.y();
    int x2 = l2.x();
    int y2 = l2.y();
    if(x1==x2 && y1==y2)
    {
        return QPoint(x1,y1);
    }
    if(y1==y2)
    {
        return QPoint(x1,y);
    }
    int x;
    float a=float(x2-x1)/float(y2-y1);
    x = x1+(y-y1)*a;
    return QPoint(x,y);
}
