#include "ekran.h"
#include <QDebug>
#include <algorithm>

Ekran::Ekran(QWidget *parent) : QWidget(parent) {}

void Ekran::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Bg = QImage(size(), QImage::Format_ARGB32);
    Bg.fill(0xff000000);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, Bg);
}


void Ekran::loadImg(const QString &name)
{
    if (!Bg.load(name))
    {
        qDebug() << "Nie mozna wczytac:" << name;
        return;
    }
    update();
}

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if(triangle.size()>=3)
        return;
    triangle.push_back(e->pos());
    qDebug() << e->pos() << triangle.size();
}

QPoint Ekran::getMinYMaxY()
{
    int minY=Bg.height(), maxY=0;
    for(int i =0;i<triangle.size();i++)
    {
        if(minY>triangle[i].y())
            minY=triangle[i].y();
        if(maxY<triangle[i].y())
            maxY=triangle[i].y();
    }
    return QPoint(minY,maxY);
}

void Ekran::getRGBA(double u, double v, double w, int& r, int& g, int& b, int& a)
{
    double x = u*triangle[0].x() + v*triangle[1].x() + w*triangle[2].x();
    double y = u*triangle[0].y() + v*triangle[1].y() + w*triangle[2].y();
    RGB c1, c2,c3,c4;
    c1 = getRGB(QPoint(std::floor(x),std::ceil(y)));
    c2 = getRGB(QPoint(std::ceil(x),std::ceil(y)));
    c3 = getRGB(QPoint(std::ceil(x),std::floor(y)));
    c4 = getRGB(QPoint(std::floor(x),std::floor(y)));
    double B = y - std::floor(y);
    double A = x - std::floor(x);
    RGB out = ((c1*(1-A)+c2*A)*B + ((c4)*(1-A)+(c3*A))*(1-B));
    r=out.r;
    g=out.g;
    b=out.b;
    a=out.a;
}

RGB Ekran::getRGB(QPoint p)
{
    uchar *pixels = Bg.scanLine(p.y());
    return RGB(pixels[4*p.x()+2],pixels[4*p.x()+1],pixels[4*p.x()],pixels[4*p.x()+3]);
}

void Ekran::drawPixel(int x, int y, int r, int g, int b, int a)
{
    uchar *pixels = Bg.scanLine(y);
    pixels[4*x]=b;
    pixels[4*x+1]=g;
    pixels[4*x+2]=r;
    pixels[4*x+3]=a;
}

void Ekran::resetPkt()
{
    triangle.clear();
}

void Ekran::clean()
{
    Bg.fill(0xff000000);
    update();
}
