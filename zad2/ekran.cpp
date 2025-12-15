#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}
{
    type = DrawType::None;
    r = 255;
    g = 255;
    b = 255;
    setFocusPolicy(Qt::StrongFocus);
}

void Ekran::showEvent(QShowEvent *event)
{
    im = QImage(width(), height(), QImage::Format_ARGB32);
    im.fill(0xff000000);
    im_save = im.copy();
}
float Ekran::calcLength(QPoint p1, QPoint p2)
{
    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();
    float r = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    return r;
}


void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0,0,width(),height(),Qt::cyan);
    p.drawImage(0,0,im);
}


void Ekran::mouseMoveEvent(QMouseEvent *e)
{
    //qDebug() << tabPi.size();
    QPoint pos = e->pos();
    if(!isPressed)
    {
        update();
        return;
    }
    if(type == DrawType::AddP)
    {
        if(e->pos().x()>=im.width() || e->pos().x()<0 || e->pos().y()>=im.height() || e->pos().y()<0)
            return;
        tabPi[tabPi.size()-1] = e->pos();
        drawCurvePoints();
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
    }
    else if(type == DrawType::MvP)
    {
        if(e->pos().x()>=im.width() || e->pos().x()<0 || e->pos().y()>=im.height() || e->pos().y()<0)
            return;
        if(idxPi<0)
            return;
        tabPi[idxPi] = e->pos();
        drawCurvePoints();
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
    }
    else if(type == DrawType::AddPolP)
    {
        if(e->pos().x()>=im.width() || e->pos().x()<0 || e->pos().y()>=im.height() || e->pos().y()<0)
            return;
        polPi[polPi.size()-1] = e->pos();
        drawPolygonPoints();
        drawFilledPolygon();
        update();
    }
    else if(type == DrawType::MvPolP)
    {
        if(e->pos().x()>=im.width() || e->pos().x()<0 || e->pos().y()>=im.height() || e->pos().y()<0)
            return;
        if(idxPi<0)
            return;
        polPi[idxPi] = e->pos();
        drawPolygonPoints();
        drawFilledPolygon();
        update();
    }

    else
        im = im_save.copy();

    if(type==DrawType::Circle)
        drawCircle(start,pos);
    if(type==DrawType::Line)
        drawToPoint(start,pos);
    if(type==DrawType::Elipse)
        drawElipse(start,pos);
    update();
}

void Ekran::drawToPoint(QPoint now, QPoint prev, bool solid)
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
            if(!solid)
                x+=3;
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
            if(!solid)
                y+=3;
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

// B(t) = (1-t)^3P_0 + 3(1-t)^2tP_1+3(1-t)t^2P_2+t^3P_3
void Ekran::drawCurve()
{
    int n = (tabPi.size()-3)/4;
    for(int i =0; i<=n;i++)
    {
        QPoint p1=tabPi[3*i+0],p2=tabPi[3*i+1],p3=tabPi[3*i+2],p4=tabPi[3*i+3];
        QPoint last=p1;
        for(float t=0; t<=1;t+=0.01)
        {
            float n = 1-t;
            float x = n*n*n*p1.x() + 3*n*n*t*p2.x() + 3*n*t*t*p3.x() + t*t*t*p4.x();
            float y = n*n*n*p1.y() + 3*n*n*t*p2.y() + 3*n*t*t*p3.y() + t*t*t*p4.y();
            QPoint tmp(x,y);
            drawToPoint(last,tmp);
            last = tmp;
        }
        drawToPoint(last,p4);
    }
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

    if(!isPressed){
        last = e->pos();
        start = e->pos();
    }
    if(e->pos().x()>=im.width() || e->pos().x()<0 || e->pos().y()>=im.height() || e->pos().y()<0)
        return;
    if(type == DrawType::Fill)
    {
        floodFill(e->pos(), im.pixelColor(e->pos().x(),e->pos().y()));
        update();
        return;
    }
    if(type == DrawType::AddP)
    {
        isPressed = true;
        if(tabPi.size()<=0)
            im_save = im.copy();
        tabPi.push_back(e->pos());
        drawCurvePoints();
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
        update();
        return;
    }
    if(type == DrawType::MvP)
    {
        if(tabPi.size()<=0)
            im_save = im.copy();
        isPressed = true;
        for(int i = 0; i<tabPi.size();i++)
        {
            if(calcLength(e->pos(),tabPi[i])<=10)
            {
                idxPi = i;
                break;
            }
        }
        drawCurvePoints();
        update();
        return;
    }
    if(type == DrawType::DelP)
    {
        if(tabPi.size()<=0)
            im_save = im.copy();
        for(int i = 0; i<tabPi.size();i++)
        {
            if(calcLength(e->pos(),tabPi[i])<=10)
            {
                //qDebug("%s", "znal");
                tabPi.erase(tabPi.begin()+i);
                break;
            }
        }
        drawCurvePoints();
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
        update();
        return;
    }
    if(type == DrawType::AddPolP)
    {
        isPressed = true;
        if(polPi.size()<=0)
            im_save = im.copy();
        polPi.push_back(e->pos());
        drawPolygonPoints();
        drawFilledPolygon();
        update();
        return;
    }
    if(type == DrawType::MvPolP)
    {
        if(polPi.size()<=0)
            im_save = im.copy();
        isPressed = true;
        for(int i = 0; i<polPi.size();i++)
        {
            if(calcLength(e->pos(),polPi[i])<=10)
            {
                idxPi = i;
                break;
            }
        }
        drawPolygonPoints();
        drawFilledPolygon();
        update();
        return;
    }
    if(type == DrawType::DelPolP)
    {
        if(polPi.size()<=0)
            im_save = im.copy();
        for(int i = 0; i<polPi.size();i++)
        {
            if(calcLength(e->pos(),polPi[i])<=10)
            {
                //qDebug("%s", "znal");
                polPi.erase(polPi.begin()+i);
                break;
            }
        }
        drawPolygonPoints();
        drawFilledPolygon();
        update();
        return;
    }

    isPressed = true;
    im_save = im.copy();
}

void Ekran::mouseReleaseEvent(QMouseEvent *e)
{
    if(isPressed)
    {
        isPressed = false;
    }
    if(type == DrawType::AddP)
    {
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
        update();
    }
    if(idxPi>=0)
    {
        idxPi = -1;
    }
}

void Ekran::drawCircle(QPoint now, QPoint prev)
{
    int x1 = now.x();
    int y1 = now.y();
    float r = calcLength(now,prev);
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
        memX = x;
        memY = y;
    }
    int x = r1*std::cos(0)+Cx;
    int y = r2*std::sin(0)+Cy;
    drawToPoint(QPoint(memX,memY),QPoint(x,y));
}

void Ekran::drawSquare(QPoint center, int a)
{
    for(int i=center.x()-a/2;i<center.x()+a/2;i++)
    {
        drawPixel(i,center.y()-a/2);
        drawPixel(i,center.y()+a/2);
    }
    for(int i=center.y()-a/2;i<center.y()+a/2;i++)
    {
        drawPixel(center.x()-a/2,i);
        drawPixel(center.x()+a/2,i);
    }
}

void Ekran::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Shift && type == DrawType::Elipse)
        type=DrawType::Circle;
    qDebug()<<"Kurwa";

}

void Ekran::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Shift && type == DrawType::Circle)
        type=DrawType::Elipse;
}

void Ekran::drawCurvePoints()
{
        im = im_save.copy();
        for(int i = 0; i < tabPi.size(); i++)
        {
            if(i%3==0){
                drawCircle(tabPi[i], tabPi[i] + QPoint(10, 0));
                if(i-1>=0)
                    drawToPoint(tabPi[i],tabPi[i-1],false);
                if(i+1<tabPi.size())
                    drawToPoint(tabPi[i],tabPi[i+1],false);
            }
            else
                drawSquare(tabPi[i],10);
        }
        if(tabPi.size()>=4)
        {
            drawCurve();
        }
        update();
}

void Ekran::drawFilledPolygon()
{
        if(polPi.size()==0)
            return;
        if(polPi.size()==1){
            drawPixel(polPi[0].x(),polPi[0].y());
            return;
        }
        if(polPi.size()==2){
            drawToPoint(polPi[0],polPi[1]);
            return;
        }
        int minY=im.height(),maxY=0;
        for(int i=0 ; i<polPi.size();i++)
        {
            drawToPoint(polPi[i],polPi[(i+1)%polPi.size()]);
            if(minY>polPi[i].y())
                minY=polPi[i].y();
            if(maxY<polPi[i].y())
                maxY=polPi[i].y();
        }
        for(int y = minY; y<=maxY; y++)
        {
            std::vector<QPoint> crossPoints;
            for(int i=0 ; i<polPi.size();i++)
            {
                if(isLineCrossed(polPi[i],polPi[(i+1)%polPi.size()],y))
                    crossPoints.push_back(findCrossPoint(polPi[i],polPi[(i+1)%polPi.size()],y));
            }
            std::sort(crossPoints.begin(),crossPoints.end(),[](const QPoint &a, const QPoint &b){return a.x()<b.x();});
            qDebug() << crossPoints.size();

            for(int k=0; 2*k<crossPoints.size();k++)
            {
                drawToPoint(crossPoints[2*k],crossPoints[2*k+1]);
                update();
            }
        }

}

void Ekran::drawPolygonPoints()
{
        im = im_save.copy();
        for(int i = 0; i < polPi.size(); i++)
        {
            drawSquare(polPi[i],10);
        }
        update();
}

void Ekran::lineChange()
{
    saveCurve();
    savePolygon();
    ResetButtons();
    type = DrawType::Line;
    emit currentType(1);
    return;


}

void Ekran::circChange()
{
    saveCurve();
    savePolygon();
    ResetButtons();
    type = DrawType::Elipse;
    emit currentType(2);
    return;

}

void Ekran::ResetButtons()
{
    type = DrawType::None;
    emit currentType(0);
}

void Ekran::addPointSet()
{
    type = DrawType::AddP;
    emit currentType(3);
    return;
}

void Ekran::delPointSet()
{
    type = DrawType::DelP;
    emit currentType(4);
    return;
}

void Ekran::movePointSet()
{
    type = DrawType::MvP;
    emit currentType(5);
    return;
}

void Ekran::addPolygonPointSet()
{
    type = DrawType::AddPolP;
    emit currentType(7);
    return;
}

void Ekran::delPolygonPointSet()
{
    type = DrawType::DelPolP;
    emit currentType(8);
    return;
}

void Ekran::movePolygonPointSet()
{
    type = DrawType::MvPolP;
    emit currentType(9);
    return;
}

void Ekran::colorDialog()
{
    QColor color = QColorDialog::getColor(Qt::white,this,tr("Select color"));
    color.getRgb(&r,&g,&b);
    emit ColorChange(color);
}

void Ekran::fillChange()
{
    saveCurve();
    savePolygon();
    ResetButtons();
    type = DrawType::Fill;
    emit currentType(6);
    return;
}

void Ekran::polygonChange(int i)
{
    if(tabPi.size()>=4)
            saveCurve();
    switch(i)
    {
    case 0:
            ResetButtons();
            break;
    case 1:
            ResetButtons();
            addPolygonPointSet();
            break;
    case 2:
            ResetButtons();
            delPolygonPointSet();
            break;
    case 3:
            ResetButtons();
            movePolygonPointSet();
            break;
    default:
            break;
    }
}

void Ekran::saveCurve()
{
    if(type != DrawType::AddP && type != DrawType::DelP && type != DrawType::MvP)
            return;
    im = im_save.copy();
    if(tabPi.size()>=4)
        drawCurve();
    tabPi.clear();
    update();
}

void Ekran::savePolygon()
{
    if(type != DrawType::AddPolP && type != DrawType::DelPolP && type != DrawType::MvPolP)
        return;
    im = im_save.copy();
    drawFilledPolygon();
    polPi.clear();
    update();
}

bool Ekran::isLineCrossed(QPoint l1, QPoint l2, int y)
{
    if (l1.y() == l2.y())
        return false;

    return ( (y >= std::min(l1.y(),l2.y())) && (y < std::max(l1.y(),l2.y())) );
}

QPoint Ekran::findCrossPoint(QPoint l1, QPoint l2, int y)
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

void Ekran::floodFill(QPoint point, QColor color)
{
    int x = point.x();
    int y = point.y();
    if(x<0 || x>=im.width() || y<0 || y>=im.height())
        return;
    if(im.pixelColor(x,y)!=color)
        return;
    QStack<QPoint> stack;
    stack.push(point);
    while(stack.size()>0)
    {
        QPoint tmp = stack.pop();
        x = tmp.x();
        y = tmp.y();
        if(x<0 || x>=im.width() || y<0 || y>=im.height())
            continue;
        if(im.pixelColor(QPoint(x,y))!=color)
            continue;
        drawPixel(x,y);
        stack.push(QPoint(x-1,y));
        stack.push(QPoint(x+1,y));
        stack.push(QPoint(x,y-1));
        stack.push(QPoint(x,y+1));
    }
}

void Ekran::curveChange(int i)
{
    //qDebug() << i;
    if(polPi.size()>0)
    {
        savePolygon();
    }
    switch(i)
    {
    case 0:
        ResetButtons();
        break;
    case 1:
        ResetButtons();
        addPointSet();
        break;
    case 2:
        ResetButtons();
        delPointSet();
        break;
    case 3:
        ResetButtons();
        movePointSet();
        break;
    default:
        break;
    }
}

