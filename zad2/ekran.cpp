#include "ekran.h"


Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    im = QImage(400,300,QImage::Format_ARGB32);
    im.fill(0xff000000);
    int x = 50;
    int y = 100;
    int r = 255;
    int g = 255;
    int b = 0;

    uchar *pixels = im.scanLine(y);
    pixels[0]=b;
    pixels[1]=g;
    pixels[2]=r;
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0,0,width(),height(),Qt::green);
    p.drawImage(0,0,im);
}

void Ekran::mouseMoveEvent(QMouseEvent *e)
{
    QPoint pos = e->pos();
    int x = pos.x();
    int y = pos.y();
    int r = 255;
    int g = 255;
    int b = 255;
    drawPixel(x,y,r,g,b);
    update();
}

void Ekran::drawToPoint(QPoint now)
{
   // y = ax+b
    // znalzec a i b
    // problem przy bardzije pionowych liniach
}


void Ekran::drawPixel(int x, int y, int r, int g, int b)
{
    if(x>=im.width() || x<0 || y>=im.height() || y<0)
        return;
    uchar *pixels = im.scanLine(y);
    pixels[4*x]=b;
    pixels[4*x+1]=g;
    pixels[4*x+2]=r;
    pixels[4*x+3]=255;
    update();
}
