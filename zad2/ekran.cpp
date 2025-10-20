#include "ekran.h"


Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    im = QImage(400,300,QImage::Format_ARGB32);
    im.fill(0xff000000);

    r = 255;
    g = 255;
    b = 255;
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
    drawPixel(x,y);
    if(isPressed){
        drawToPoint(pos,last);
        last = pos;
    }
    update();
}

void Ekran::drawToPoint(QPoint now, QPoint prev)
{
    int minX = std::min(last.x(),now.x());
    int maxX = std::max(last.x(),now.x());
    if(!(minX==maxX))
    {
        float a = float(now.y()-last.y())/float(now.x()-last.x());
        float b = last.y() - (a * last.x());
        for(minX;minX<maxX;minX++)
        {
            drawPixel(minX,a*minX+b);
        }
    }
    else
    {
        int minY = std::min(last.y(), now.y());
        int maxY = std::max(last.y(), now.y());
        for (int y = minY; y <= maxY; ++y)
            drawPixel(now.x(), y);
    }
    /*if(now==prev)
        return;
    QPoint s;
    s.setX((now.x()+prev.x())/2);
    s.setY((now.y()+prev.y())/2);
    if(s==now || s == prev)
        return;
    drawPixel(s.x(),s.y());
    drawToPoint(now,s);
    drawToPoint(s,prev);*/
}


void Ekran::drawPixel(int x, int y)
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

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if(!isPressed){
        last = e->pos();
        isPressed = true;
    }
}

void Ekran::mouseReleaseEvent(QMouseEvent *)
{
    isPressed = false;
}
