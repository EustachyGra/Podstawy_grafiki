

// ----------------------------- ekran.cpp -----------------------------
#include "ekran.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QFileDialog>
#include <algorithm>
#include <limits>
#include <cmath>

/* =========================================================
   Pomocnicze wektory
   ========================================================= */

Vec3 Ekran::cross(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float Ekran::dot(const Vec3& a, const Vec3& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

bool Ekran::isBackface(const Vec3& a, const Vec3& b, const Vec3& c)
{
    Vec3 ab = b - a;
    Vec3 ac = c - a;
    Vec3 n  = cross(ab, ac);

    Vec3 view = {0,0,-1};
    return dot(n, view) >= 0;
}

/* ========================================================= */

Ekran::Ekran(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    // Don't import texture on construction; user will be prompted on show
}

void Ekran::showEvent(QShowEvent *)
{
    im = QImage(width(), height(), QImage::Format_ARGB32);
    im.fill(0xff000000);
    zbuffer.assign(width()*height(), std::numeric_limits<float>::infinity());

    // ask for texture first
    importTexture();

    // load obj (shows dialog if path empty)
    loadOBJ("");

    drawModel(0,0,0,0,0,0,1,1,1);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0,0,im);
}

/* ========================================================= */

void Ekran::drawPixel(int x, int y, int r, int g, int b, int a)
{
    if (x<0 || y<0 || x>=im.width() || y>=im.height())
        return;

    uchar* px = im.scanLine(y);
    px[4*x+0] = static_cast<uchar>(r);
    px[4*x+1] = static_cast<uchar>(g);
    px[4*x+2] = static_cast<uchar>(b);
    px[4*x+3] = static_cast<uchar>(a);
}

QPoint Ekran::project(const Vec3& v)
{
    float m = v.z + d;
    if (fabs(m) < 0.00001f) m = 0.00001f;

    float x = v.x * d / m;
    float y = v.y * d / m;

    return QPoint(int(x + width()/2), int(-y + height()/2)); // note: flip y for screen coords
}

/* ========================================================= */

void Ekran::drawTriangle(QPoint p0, QPoint p1, QPoint p2,
                         float z0, float z1, float z2,
                         QPointF uv0, QPointF uv1, QPointF uv2)
{
    std::vector<QPoint> v = {p0,p1,p2};

    int minY = im.height(), maxY = 0;
    for (auto& p : v) {
        minY = std::min(minY, p.y());
        maxY = std::max(maxY, p.y());
    }

    minY = std::max(minY,0);
    maxY = std::min(maxY, im.height()-1);

    double invZ0 = 1.0 / (z0 + d);
    double invZ1 = 1.0 / (z1 + d);
    double invZ2 = 1.0 / (z2 + d);

    for (int y = minY; y <= maxY; y++)
    {
        std::vector<float> xs;

        for (int i=0;i<3;i++)
            if (isLineCrossed(v[i], v[(i+1)%3], y))
                xs.push_back(findCrossPoint(v[i], v[(i+1)%3], y));

        if (xs.size() != 2) continue;
        std::sort(xs.begin(), xs.end());

        int xStart = std::ceil(xs[0]);
        int xEnd   = std::floor(xs[1]);

        for (int x=xStart; x<=xEnd; x++)
        {
            if (x<0 || x>=im.width()) continue;

            double XA=v[0].x(), YA=v[0].y();
            double XB=v[1].x(), YB=v[1].y();
            double XC=v[2].x(), YC=v[2].y();

            double det = (XB-XA)*(YC-YA)-(YB-YA)*(XC-XA);
            if (fabs(det) < 0.00001) continue;

            double v1 = ((x-XA)*(YC-YA)-(y-YA)*(XC-XA))/det;
            double w1 = ((XB-XA)*(y-YA)-(YB-YA)*(x-XA))/det;
            double u1 = 1 - v1 - w1;

            double invZ = u1*invZ0 + v1*invZ1 + w1*invZ2;
            if (invZ <= 0) continue;

            double depth = 1.0 / invZ;
            int idx = y*im.width() + x;
            if (depth >= zbuffer[idx]) continue;

            zbuffer[idx] = depth;

            int r,g,b,a;
            // compute perspective-correct UV and fetch color
            double U = (u1 * uv0.x() * invZ0 + v1 * uv1.x() * invZ1 + w1 * uv2.x() * invZ2) / invZ;
            double V = (u1 * uv0.y() * invZ0 + v1 * uv1.y() * invZ1 + w1 * uv2.y() * invZ2) / invZ;

            // OBJ uses V=0 at bottom; convert to image coordinates (flip V)
            double tx = std::clamp(U, 0.0, 1.0) * (texture.width()-1);
            double ty = std::clamp(1.0 - V, 0.0, 1.0) * (texture.height()-1);

            RGB c = getRGB(QPoint(int(tx+0.5), int(ty+0.5)));
            r = c.r; g = c.g; b = c.b; a = c.a;

            drawPixel(x,y,r,g,b,a);
        }
    }
}

/* =========================================================
   OBJ LOADER
   ========================================================= */

bool Ekran::loadOBJ(const QString& path)
{
    QString filename = path;
    if (filename.isEmpty()) {
        filename = QFileDialog::getOpenFileName(
            this,
            "Wybierz obiekt",
            "",
            "Obiekty (*.obj);;Wszystkie pliki (*.*)"
            );
    }

    if (filename.isEmpty())
        return false;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    vertices.clear();
    faces.clear();
    texcoords.clear();

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith('#')) continue;

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size()==0) continue;

        QString t = parts[0];
        if (t == "v")
        {
            if (parts.size() < 4) continue;
            float x = parts[1].toFloat();
            float y = parts[2].toFloat();
            float z = parts[3].toFloat();
            vertices.push_back({x,y,z});
        }
        else if (t == "vt")
        {
            if (parts.size() < 3) continue;
            float u = parts[1].toFloat();
            float v = parts[2].toFloat();
            texcoords.push_back(QPointF(u,v));
        }
        else if (t == "f")
        {
            if (parts.size() < 4) continue; // expecting triangle
            Face f;
            for (int i=0;i<3;i++) {
                QString s = parts[1+i];
                QStringList elems = s.split('/');
                if (elems.size() >= 1 && !elems[0].isEmpty()) f.v[i] = elems[0].toInt() - 1;
                if (elems.size() >= 2 && !elems[1].isEmpty()) f.vt[i] = elems[1].toInt() - 1; else f.vt[i] = 0;
            }
            faces.push_back(f);
        }
    }
    return true;
}

/* ========================================================= */

void Ekran::drawModel(    float tx, float ty, float tz,
                      float az, float ay, float ax,
                      float sx=1, float sy=1, float sz=1)
{
    im.fill(0xff000000);
    zbuffer.assign(im.width()*im.height(), std::numeric_limits<float>::infinity());
    Mat4 M = CreateMacierz(0,0,300, 0,0,0, 1,1,1);

    std::vector<Vec3> tv;
    std::vector<QPoint> pp;

    for (auto& v : vertices)
    {
        Vec3 t = M * v;
        tv.push_back(t);
        pp.push_back(project(t));
    }

    for (auto& f : faces)
    {
        int a=f.v[0], b=f.v[1], c=f.v[2];
        if (a < 0 || b < 0 || c < 0) continue;
        if (a >= (int)tv.size() || b >= (int)tv.size() || c >= (int)tv.size()) continue;

        if (isBackface(tv[a],tv[b],tv[c])) continue;

        QPointF uvA = (f.vt[0] >= 0 && f.vt[0] < (int)texcoords.size()) ? texcoords[f.vt[0]] : QPointF(0,0);
        QPointF uvB = (f.vt[1] >= 0 && f.vt[1] < (int)texcoords.size()) ? texcoords[f.vt[1]] : QPointF(0,0);
        QPointF uvC = (f.vt[2] >= 0 && f.vt[2] < (int)texcoords.size()) ? texcoords[f.vt[2]] : QPointF(0,0);

        drawTriangle(pp[a],pp[b],pp[c],
                     tv[a].z,tv[b].z,tv[c].z,
                     uvA,uvB,uvC);
    }
    update();
}

/* ========================================================= */
Mat4 Ekran::CreateMacierz(
    float tx, float ty, float tz,
    float az, float ay, float ax,
    float sx=1, float sy=1, float sz=1)
{
    Mat4 T,RZ,RY,RX,S;
    T = RZ = RY = RX = S = Mat4::Identity();

    // Build matrix in the order used previously
    T.translate(tx,ty,tz);
    RZ.rotateZ(az);
    RY.rotateY(ay);
    RX.rotateX(ax);
    S.scale(sx,sy,sz);

    return T * RZ * RY * RX * S;
}

void Ekran::importTexture()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Wybierz obraz",
        "",
        "Obrazy (*.png *.jpg *.bmp *.jpeg)"
        );

    if (filename.isEmpty())
        return;

    texture.load(filename);

    texture = texture
                  .convertToFormat(QImage::Format_ARGB32)
                  .scaled(255, 255, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    TA = {0,0};
    TB = {texture.width()-1,0};
    TC = {texture.width()-1,texture.height()-1};
    TD = {0,texture.height()-1};
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
    float a = float(x2-x1)/float(y2-y1);
    float x = x1 + (y - y1) * a;
    return x;
}
RGB Ekran::getRGB(QPoint p)
{
    int x = std::clamp(p.x(), 0, texture.width()  - 1);
    int y = std::clamp(p.y(), 0, texture.height() - 1);

    uchar *pixels = texture.scanLine(y);
    return RGB(pixels[4*x],pixels[4*x + 1], pixels[4*x + 2],pixels[4*x + 3]);
}

void Ekran::getRGBA(double u, double v, double w,
                    double invZ0, double invZ1, double invZ2,
                    int& r, int& g, int& b, int& a)
{
    // Not used in new pipeline but kept for compatibility
    double invZ = u*invZ0 + v*invZ1 + w*invZ2;
    if (invZ <= 0.0) { r=g=b=0; a=255; return; }

    double U = (u * invZ0 + v * invZ1 + w * invZ2) / invZ; // placeholder
    double V = (u * invZ0 + v * invZ1 + w * invZ2) / invZ;

    double tx = std::clamp(U, 0.0, 1.0) * (texture.width()-1);
    double ty = std::clamp(1.0 - V, 0.0, 1.0) * (texture.height()-1);

    RGB c = getRGB(QPoint(int(tx+0.5), int(ty+0.5)));
    r=c.r; g=c.g; b=c.b; a=c.a;
}


