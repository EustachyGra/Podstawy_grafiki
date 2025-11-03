#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    r = 255;
    g = 255;
    b = 255;
    im = QImage(400,300,QImage::Format_ARGB32);
    im.fill(0xff000000);
    red = QImage(255,25,QImage::Format_ARGB32);
    red.fill(0xffff0000);
    imageFrame(red,1);
    green = QImage(255,25,QImage::Format_ARGB32);
    green.fill(0xff00ff00);
    imageFrame(green,1);
    blue = QImage(255,25,QImage::Format_ARGB32);
    blue.fill(0xff0000ff);
    imageFrame(blue,1);
    preview = QImage(50,50,QImage::Format_ARGB32);
    preview.fill(QColor(r,g,b));
    imageFrame(preview,3);
}

bool Ekran::clickOnRed(QPoint pos)
{
    int x = pos.x();
    int y = pos.y();
    if(!(x>=im.width()+10 && x<im.width()+red.width()+10 && y>=0 &&y<red.height()))
        return false;
    x = x- im.width()-10;
    red.fill(0xff000000);
    for(int i =0; i<red.height(); i++)
    {
        uchar *pixels = red.scanLine(i);
        for(int j = 0; j<x;j++)
        {
            pixels[4*j]=0;
            pixels[4*j+1]=0;
            pixels[4*j+2]=255;
            pixels[4*j+3]=255;
            r=x;
        }
    }
    imageFrame(red,1);
    preview.fill(QColor(r,g,b));
    imageFrame(preview,3);
    update();
    return true;
}

bool Ekran::clickOnGreen(QPoint pos)
{
    int x = pos.x();
    int y = pos.y();
    if(!(x>=im.width()+10 && x<im.width()+red.width()+10 && y>=red.height() &&y<red.height()+green.height()))
        return false;
    x = x- im.width()-10;
    green.fill(0xff000000);
    for(int i =0; i<green.height(); i++)
    {
        uchar *pixels = green.scanLine(i);
        for(int j = 0; j<x;j++)
        {
            pixels[4*j]=0;
            pixels[4*j+1]=255;
            pixels[4*j+2]=0;
            pixels[4*j+3]=255;
            g=x;
        }
    }
    imageFrame(green,1);
    preview.fill(QColor(r,g,b));
    imageFrame(preview,3);
    update();
    return true;
}

bool Ekran::clickOnBlue(QPoint pos)
{
    int x = pos.x();
    int y = pos.y();
    if(!(x>=im.width()+10 && x<im.width()+red.width()+10 &&  y>=red.height()+green.height() &&y<red.height()+green.height()+blue.height()))
        return false;
    x = x- im.width()-10;
    blue.fill(0xff000000);
    for(int i =0; i<blue.height(); i++)
    {
        uchar *pixels = blue.scanLine(i);
        for(int j = 0; j<x;j++)
        {
            pixels[4*j]=255;
            pixels[4*j+1]=0;
            pixels[4*j+2]=0;
            pixels[4*j+3]=255;
            b=x;
        }
    }
    imageFrame(blue,1);
    preview.fill(QColor(r,g,b));
    imageFrame(preview,3);
    update();
    return true;
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0,0,width(),height(),Qt::cyan);
    p.drawImage(0,0,im);
    p.drawImage(410,0,red);
    p.drawImage(410,red.height(),green);
    p.drawImage(410,red.height()+green.height(),blue);
    p.drawImage(410,red.height()+blue.height()+green.height(),preview);
}


void Ekran::mouseMoveEvent(QMouseEvent *e)
{

    QPoint pos = e->pos();
    if(clickOnRed(pos) || clickOnBlue(pos) || clickOnGreen(pos))
        return;
    if(pos.x()>=im.width() || pos.x()<0 || pos.y()>=im.height() || pos.y()<0)
        return;
    if(isPressed){
        im = im_save.copy();
    }
   // drawCircle(start,pos);
    //drawToPoint(start,pos);
    drawElipse(start,pos);
    update();
}

void Ekran::drawToPoint(QPoint now, QPoint prev)
{
    ///Talica
    int x1 = now.x();
    int y1 = now.y();
    int x2 = prev.x();
    int y2 = prev.y();
    if(x1==x2 && y1==y2)
    {
        drawPixel(x1,y1);
        return;
    }
    if(std::abs(y2-y1)<=std::abs(x2-x1))
    {
        int minX = std::min(prev.x(),now.x());
        int maxX = std::max(prev.x(),now.x());
        int y;
        float a=float(y2-y1)/float(x2-x1);
        for(int x = minX; x<=maxX; x++)
        {
            y = y1+(x-x1)*a;
            drawPixel(x,y);
        }
    }
    else
    {
        int minY = std::min(prev.y(), now.y());
        int maxY = std::max(prev.y(), now.y());
        int x;
        float a=float(x2-x1)/float(y2-y1);
        for(int y=minY; y<=maxY;y++)
        {
            x = x1+(y-y1)*a;
            drawPixel(x,y);
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


void Ekran::drawPixel(int x, int y)
{
    if(x>=im.width() || x<0 || y>=im.height() || y<0)
        return;
    uchar *pixels = im.scanLine(y);
    pixels[4*x]=b;
    pixels[4*x+1]=g;
    pixels[4*x+2]=r;
    pixels[4*x+3]=255;
}

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if(clickOnRed(e->pos()) || clickOnBlue(e->pos()) || clickOnGreen(e->pos()) )
        return;
    if(!isPressed){
        last = e->pos();
        start = e->pos();
        isPressed = true;
        im_save = im.copy();
    }
}

void Ekran::mouseReleaseEvent(QMouseEvent *)
{
    if(isPressed)
    {
        isPressed = false;
    }
}

void Ekran::imageFrame(QImage& img, int size)
{
    for(int i=0; i<size;i++)
    {
        uchar *pixels = img.scanLine(i);
        for(int j=0;j<img.width();j++)
        {
            pixels[4*j]=0;
            pixels[4*j+1]=0;
            pixels[4*j+2]=0;
            pixels[4*j+3]=255;
        }
        pixels = img.scanLine(img.height()-i-1);
        for(int j=0;j<img.width();j++)
        {
            pixels[4*j]=0;
            pixels[4*j+1]=0;
            pixels[4*j+2]=0;
            pixels[4*j+3]=255;
        }
    }
    for(int i=0; i<img.height();i++)
    {
        uchar *pixels = img.scanLine(i);
        for(int j=0;j<size;j++)
        {
            int tmp = img.width()-1;
            pixels[4*j]=0;
            pixels[4*j+1]=0;
            pixels[4*j+2]=0;
            pixels[4*j+3]=255;
            pixels[4*(tmp-j)]=0;
            pixels[4*(tmp-j)+1]=0;
            pixels[4*(tmp-j)+2]=0;
            pixels[4*(tmp-j)+3]=255;
        }
    }
}

void Ekran::drawCircle(QPoint now, QPoint prev)
{
    int x1 = now.x();
    int y1 = now.y();
    int x2 = prev.x();
    int y2 = prev.y();
    float r = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    float midPoint = (std::sqrt(2)*r)/2;
    for(int x=0; x<midPoint;x++)
    {
        int y = std::sqrt(r*r-x*x);
        drawPixel(x+x1,y+y1);
        drawPixel(y+x1,x+y1);
        drawPixel(-x+x1,-y+y1);
        drawPixel(-y+x1,-x+y1);
        drawPixel(-x+x1,y+y1);
        drawPixel(-y+x1,x+y1);
        drawPixel(x+x1,-y+y1);
        drawPixel(y+x1,-x+y1);
    }
}
void Ekran::drawElipse(QPoint now, QPoint prev)
{
    int x1 = now.x(), y1 = now.y();
    int x2 = prev.x(), y2 = prev.y();
    int Cx = (x1+x2)/2;
    int Cy = (y1+y2)/2;
    int r1 = std::abs((x2-x1)/2);
    int r2 = std::abs((y2-y1)/2);
    int N = 50;
    int memX = r1*std::cos(0)+Cx;
    int memY = r2*std::sin(0)+Cy;
    for(int i=1; i<N;i++)
    {
        float a = (2*M_PI*i)/N;
        int x = r1*std::cos(a)+Cx;
        int y = r2*std::sin(a)+Cy;
        drawToPoint(QPoint(memX,memY),QPoint(x,y));
        //drawPixel(x,y,_r,_g,_b);
        memX = x;
        memY = y;
    }
    int x = r1*std::cos(0)+Cx;
    int y = r2*std::sin(0)+Cy;
    drawToPoint(QPoint(memX,memY),QPoint(x,y));
}
