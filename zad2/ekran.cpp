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
    if(isPressed){
        drawToPoint(start,last);
        last = pos;
    }
   // drawPoints();
    drawToPoint(start,pos,r,g,b);

    update();
}

void Ekran::drawToPoint(QPoint now, QPoint prev, int _r, int _g, int _b)// bool save)
{
    ///Talica
    int x1 = now.x();
    int y1 = now.y();
    int x2 = prev.x();
    int y2 = prev.y();
    if(x1==x2 && y1==y2)
    {
        drawPixel(x1,y1,_r,_g,_b);
        return;
    }
    if(std::abs(y2-y1)<=std::abs(x2-x1))
    {
        int minX = std::min(last.x(),now.x());
        int maxX = std::max(last.x(),now.x());
        int y;
        float a=float(y2-y1)/float(x2-x1);
        for(int x = minX; x<=maxX; x++)
        {
            y = y1+(x-x1)*a;
            drawPixel(x,y,_r,_g,_b);
            /*if(save)
                points.push_back(QPoint(x,y));*/
        }
    }
    else
    {
        int minY = std::min(last.y(), now.y());
        int maxY = std::max(last.y(), now.y());
        int x;
        float a=float(x2-x1)/float(y2-y1);
        for(int y=minY; y<=maxY;y++)
        {
            x = x1+(y-y1)*a;
            drawPixel(x,y,_r,_g,_b);
            /*if(save)
                points.push_back(QPoint(x,y));*/
        }
    }
    //Rekurencja
    /*
    if(now==prev)
        return;
    QPoint s;
    s.setX((now.x()+prev.x())/2);
    s.setY((now.y()+prev.y())/2);
    if(s==now || s == prev)
        return;
    if(save)
        points.push_back(s);
    drawPixel(s.x(),s.y(),_r,_g,_b);
    drawToPoint(now,s,_r,_g,_b,save);
    drawToPoint(s,prev,_r,_g,_b,save);
    */
}


void Ekran::drawPixel(int x, int y, int _r, int _g, int _b)
{
    if(x>=im.width() || x<0 || y>=im.height() || y<0)
        return;
    uchar *pixels = im.scanLine(y);
    pixels[4*x]=_b;
    pixels[4*x+1]=_g;
    pixels[4*x+2]=_r;
    pixels[4*x+3]=255;
}

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if(!isPressed){
        last = e->pos();
        start = e->pos();
        isPressed = true;
    }
}

void Ekran::mouseReleaseEvent(QMouseEvent *)
{
    isPressed = false;
    //drawToPoint(start,last,r,g,b,true);
}
/*
void Ekran::drawPoints()
{
    for(int i =0;i<points.size();i++)
    {
        drawPixel(points[i].x(),points[i].y(),r,g,b);
    }
}*/
