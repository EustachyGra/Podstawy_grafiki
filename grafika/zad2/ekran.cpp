#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    zbuffer.assign(10*10, std::numeric_limits<float>::infinity());
    importTexture();
}

void Ekran::showEvent(QShowEvent *)
{
    im = QImage(width(), height(), QImage::Format_ARGB32);
    im.fill(0xff000000);
    zbuffer.assign(width()*height(), std::numeric_limits<float>::infinity());
    drawCube(0,0,0,0,0,0,1,1,1);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, im);
}


void Ekran::drawPixel(int x, int y, int r, int g, int b, int a)
{
    if (x < 0 || y < 0 || x >= im.width() || y >= im.height())
        return;
    uchar *px = im.scanLine(y);
    px[4 * x + 0] = r;
    px[4 * x + 1] = g;
    px[4 * x + 2] = b;
    px[4 * x + 3] = a;
}

QPoint Ekran::project(const Vec3& v)
{


    float mianownik = (v.z + d);
    if(mianownik==0)
    {
        mianownik=0.000001f;
    }
    float x = v.x * d / mianownik;
    float y = v.y * d / mianownik;

    return QPoint(int(x + width() / 2), int(y + height() / 2));
}

void Ekran::drawTriangle(QPoint p0, QPoint p1, QPoint p2, float z0, float z1, float z2, bool whichTriangle)
{
    QRect ekran(0,0,width(),height());
    if (!ekran.contains(p0) && !ekran.contains(p1) && !ekran.contains(p2)) {
        return;
    }

    std::vector<QPoint> vec = {p0,p1,p2};
    int minY=im.height(), maxY=0;
    for(int i =0;i<3;i++)
    {
        if(minY>vec[i].y())
            minY=vec[i].y();
        if(maxY<vec[i].y())
            maxY=vec[i].y();
    }
    minY = std::max(minY,0);
    maxY= std::min(maxY,height()-1);
    for(int y = minY; y<=maxY; y++)
    {
        std::vector<float> crossPoints;
        for(int i=0 ; i<3;i++)
        {
            if(isLineCrossed(vec[i],vec[(i+1)%vec.size()],y))
                crossPoints.push_back(findCrossPoint(vec[i],vec[(i+1)%vec.size()],y));
        }
        std::sort(crossPoints.begin(),crossPoints.end());
        if(crossPoints.size()!=2)
            continue;
        int xStart = (int)std::ceil(crossPoints[0]);
        int xEnd   = (int)std::floor(crossPoints[1]);
        if((xStart <0 && xEnd >= width())  || xEnd<0 || xStart >= width() )
            continue;
        invZ.clear();
        double invZ0 = 1.0 / (double(z0) + double(d));
        double invZ1 = 1.0 / (double(z1) + double(d));
        double invZ2 = 1.0 / (double(z2) + double(d));
        invZ.push_back(invZ0);
        invZ.push_back(invZ1);
        invZ.push_back(invZ2);
        invZ.push_back(0);
        for (int x = xStart; x <= xEnd; x++)
        {
            if(x<0 || x>=width())
                continue;
            double u,v,w;
            double XA = vec[0].x();
            double YA = vec[0].y();

            double XB = vec[1].x();
            double YB = vec[1].y();

            double XC = vec[2].x();
            double YC = vec[2].y();

            double mianownik = (XB - XA) * (YC - YA) - (YB - YA) * (XC - XA);
            if (std::abs(mianownik) < 0.0001f)
                continue;
            v = ((x - XA) * (YC - YA) - (y - YA) * (XC - XA)) / mianownik;
            w = ((XB - XA) * (y - YA) - (YB - YA) * (x - XA)) / mianownik;
            u = 1- v -w ;


            double invZd = u * invZ0 + v * invZ1 + w * invZ2;
            invZ[3] = invZd;
            if (invZd <= 0.0) continue;
            double depth = 1.0 / invZd;
            int r,g,b,a;
            int idx = y * im.width() + x;

            if (idx < 0 || idx >= zbuffer.size())
                continue;

            if (depth <= 0) {
                continue;
            }

            if (depth >= zbuffer[idx])
                continue;

            zbuffer[idx] = depth;

            getRGBA(u,v,w,r,g,b,a,whichTriangle);
            drawPixel(x,y,r,g,b,a);

        }
    }
}

void Ekran::drawCube(
    float tx, float ty, float tz,
    float ax, float ay, float az,
    float sx, float sy, float sz
    )
{
    im.fill(0xff000000);
    zbuffer.assign(im.width()*im.height(), std::numeric_limits<float>::infinity());

   // qDebug() << tx << ty << tz <<sx <<sy <<sz << ax << ay << az;
    std::vector<Vec3> cube = {
        {-75,-75,-75}, {75,-75,-75}, {75,75,-75}, {-75,75,-75},
        {-75,-75, 75}, {75,-75, 75}, {75,75, 75}, {-75,75, 75}
    };

    int faces[6][4] = {
        {0,1,2,3},
        {4,5,6,7},
        {0,1,5,4},
        {2,3,7,6},
        {1,2,6,5},
        {0,3,7,4}
    };

    Mat4 M = CreateMacierz( tx,  ty,  tz,  az,  ay,  ax,  sx,  sy,  sz);

    std::vector<Vec3> tv;
    std::vector<QPoint> p;
    for (auto& v : cube){
        Vec3 tc = M * v;
        tv.push_back(tc);
        p.push_back(project(tc));

    }

    for (int i=0;i<6;i++){
        int a = faces[i][0];
        int b = faces[i][1];
        int c = faces[i][2];
        int d = faces[i][3];

        drawTriangle(p[b], p[a], p[c],
                     tv[b].z, tv[a].z, tv[c].z,
                     true);
        drawTriangle(p[d], p[c], p[a],
                     tv[d].z, tv[c].z, tv[a].z,
                     false);

    }

    update();
}

void Ekran::importTexture()
{
    /*QString filename = QFileDialog::getOpenFileName(
        this,
        "Wybierz obraz",
        "",
        "Obrazy (*.png *.jpg *.bmp *.jpeg);;Wszystkie pliki (*.*)"
        );

    if (filename.isEmpty())
        return;*/

    //qDebug() << "Wczytuje:" << filename;

    //qDebug() << filename;
    texture.load("C:/Users/kwiat/OneDrive/Dokumenty/GitHub/Podstawy_grafiki/mix/umk.jpg");
    texture = texture.scaled(
        255, 255,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
    TA={0,0};
    TB= {texture.width()-1,0};
    TD={0,texture.height()-1};
    TC= {texture.width()-1,texture.height()-1};
}

Mat4 Ekran::CreateMacierz(float tx, float ty, float tz, float az, float ay, float ax, float sx, float sy, float sz)
{
    //qDebug() << tx << ty << tz <<sx <<sy <<sz << ax << ay << az;

    Mat4 T,RZ,RY,RX,S;
    T = RZ = RY = RX = S = Mat4::Identity();
    T.translate(tx,ty,tz);
    RZ.rotateZ(az);
    RY.rotateY(ay);
    RX.rotateX(ax);
    S.scale(sx,sy,sz);
    return T*RZ*RY*RX*S;
}
bool Ekran::isLineCrossed(QPoint l1, QPoint l2, int y)
{
    if (l1.y() == l2.y())
        return false;

    return ( (y >= std::min(l1.y(),l2.y())) && (y < std::max(l1.y(),l2.y())) );
}
float Ekran::findCrossPoint(QPoint l1, QPoint l2, int y)
{
    int x1 = l1.x();
    int y1 = l1.y();
    int x2 = l2.x();
    int y2 = l2.y();
    if(x1==x2 && y1==y2)
    {
        return x1;
    }
    if(y1==y2)
    {
        return x1;
    }
    int x;
    float a=float(x2-x1)/float(y2-y1);
    x = x1+(y-y1)*a;
    return x;
}
RGB Ekran::getRGB(QPoint p)
{
    int x = std::clamp(p.x(), 0, texture.width()  - 1);
    int y = std::clamp(p.y(), 0, texture.height() - 1);

    uchar *pixels = texture.scanLine(y);
    return RGB(pixels[4*x],pixels[4*x + 1], pixels[4*x + 2],pixels[4*x + 3]);
}

void Ekran::getRGBA(double u, double v, double w, int& r, int& g, int& b, int& a, bool whichTriangle)
{
    std::vector<QPoint> triangle;
    if(whichTriangle)
        triangle = {TB,TA,TC};
    else
        triangle = {TD,TC,TA};


    double x = u*(triangle[0].x()*invZ[0]) + v*(triangle[1].x()*invZ[1]) + w*(triangle[2].x()*invZ[2]);
    double y = u*(triangle[0].y()*invZ[0]) + v*(triangle[1].y()*invZ[1]) + w*(triangle[2].y()*invZ[2]);
    x = x/invZ[3];
    y = y/invZ[3];
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
