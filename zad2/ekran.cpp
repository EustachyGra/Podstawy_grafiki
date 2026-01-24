#include "ekran.h"
#include <cmath>
Vec3 sub(const Vec3 &a, const Vec3 &b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
Vec3 cross(const Vec3 &a, const Vec3 &b) {
    return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}

// ====================== KONSTRUKTOR ======================

Ekran::Ekran(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

// ====================== ZDARZENIA QT ======================

void Ekran::showEvent(QShowEvent *)
{
    im = QImage(width(), height(), QImage::Format_ARGB32);
    im.fill(0xff000000);
    qDebug() << "show" << width() << height();
    drawCube(0,0,0,0,0,0,1,1,1);
}

void Ekran::paintEvent(QPaintEvent *)
{
    qDebug() << "paint"<< width() << height();
    QPainter p(this);
    p.drawImage(0, 0, im);
}

// ====================== RYSOWANIE ======================

void Ekran::drawPixel(int x, int y, int r, int g, int b)
{

    if (x < 0 || y < 0 || x >= im.width() || y >= im.height())
        return;
    //qDebug() << x << y;
    uchar *px = im.scanLine(y);
    px[4 * x + 0] = r;
    px[4 * x + 1] = g;
    px[4 * x + 2] = b;
    px[4 * x + 3] = 255;
}

void Ekran::drawToPoint(QPoint a, QPoint b)
{
    qDebug() << "Line" << a << b;
    int x1 = a.x(), y1 = a.y();
    int x2 = b.x(), y2 = b.y();

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        drawPixel(x1, y1, 255, 255, 255);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}


// ====================== RZUTOWANIE ======================

QPoint Ekran::project(const Vec3& v)
{
    float mianownik;
    if (v.z > -d)
        mianownik = (v.z + d);
    else
        mianownik = 1;

    float x = v.x * d / mianownik;
    float y = v.y * d / mianownik;

    return QPoint(
        int(x + width() / 2),
        int(y + height() / 2)
        );
}

// ====================== RYSOWANIE BRYŁY ======================

void Ekran::drawCube(
    float tx, float ty, float tz,
    float ax, float ay, float az,
    float sx, float sy, float sz
    )
{
    im.fill(0xff000000);
    qDebug() << tx << ty << tz <<sx <<sy <<sz << ax << ay << az;
    std::vector<Vec3> cube = {
        {-75,-75,-75}, {75,-75,-75}, {75,75,-75}, {-75,75,-75},
        {-75,-75, 75}, {75,-75, 75}, {75,75, 75}, {-75,75, 75}
    };

    int faces[6][4] = {
        {0,1,2,3}, // tylna
        {4,5,6,7}, // przednia
        {0,1,5,4}, // dolna
        {2,3,7,6}, // górna
        {0,3,7,4}, // lewa
        {1,2,6,5}  // prawa
    };

    Mat4 M = CreateMacierz( tx,  ty,  tz,  az,  ay,  ax,  sx,  sy,  sz);

    std::vector<QPoint> p;
    for (auto &v : cube) p.push_back(project(M * v));

    int edgeFaces[12][2] = {
        {0,2},{0,3},{0,5},{0,4}, // tylne krawędzie
        {1,2},{1,3},{1,5},{1,4}, // przednie krawędzie
        {2,4},{3,5},{2,5},{3,4}  // boczne
    };

    // obliczasz normalną każdej ściany
    bool faceVisible[6];
    for (int f=0; f<6; ++f) {
        Vec3 v0 = M*cube[faces[f][0]];
        Vec3 v1 = M*cube[faces[f][1]];
        Vec3 v2 = M*cube[faces[f][2]];

        Vec3 n = cross(sub(v1,v0), sub(v2,v0));
        Vec3 center = {(v0.x+v1.x+v2.x)/3,(v0.y+v1.y+v2.y)/3,(v0.z+v1.z+v2.z)/3};
        Vec3 viewDir = {0-center.x,0-center.y,0-center.z};

        faceVisible[f] = (n.x*viewDir.x + n.y*viewDir.y + n.z*viewDir.z) > 0;
    }

    // rysowanie krawędzi jeśli którakolwiek przyległa ściana jest widoczna
    for (int e=0;e<12;++e) {
        int f1=edgeFaces[e][0], f2=edgeFaces[e][1];
        if (faceVisible[f1] || faceVisible[f2]) {
            drawToPoint(p[edges[e][0]], p[edges[e][1]]);
        }
    }
    update();
}

Mat4 Ekran::CreateMacierz(float tx, float ty, float tz, float az, float ay, float ax, float sx, float sy, float sz)
{
    qDebug() << tx << ty << tz <<sx <<sy <<sz << ax << ay << az;

    Mat4 T,RZ,RY,RX,S;
    T = RZ = RY = RX = S = Mat4::Identity();
    T.translate(tx,ty,tz);
    RZ.rotateZ(az);
    RY.rotateY(ay);
    RX.rotateX(ax);
    S.scale(sx,sy,sz);
    return T*RZ*RY*RX*S;

}
