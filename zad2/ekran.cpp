// ========================= Ekran.cpp =========================
#include "ekran.h"

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
    // initialize z-buffer
    zbuffer.assign(width()*height(), std::numeric_limits<float>::infinity());
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
    uchar *px = im.scanLine(y);
    // keep same channel order as original code (may be BGR in memory)
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
    if (v.z + d > 0.01f)
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

// ====================== RYSOWANIE BRYŁY (faces, triangles, z-buffer) ======================

void Ekran::drawTriangle(const Vec3 &v0c, const Vec3 &v1c, const Vec3 &v2c, const QPoint &p0, const QPoint &p1, const QPoint &p2, int baseR, int baseG, int baseB)
{
    // bounding box
    int minX = std::max(0, std::min({p0.x(), p1.x(), p2.x()}));
    int maxX = std::min(im.width()-1, std::max({p0.x(), p1.x(), p2.x()}));
    int minY = std::max(0, std::min({p0.y(), p1.y(), p2.y()}));
    int maxY = std::min(im.height()-1, std::max({p0.y(), p1.y(), p2.y()}));

    // precompute values for barycentric coordinates
    float x0 = p0.x(), y0 = p0.y();
    float x1 = p1.x(), y1 = p1.y();
    float x2 = p2.x(), y2 = p2.y();

    float denom = (y1 - y2)*(x0 - x2) + (x2 - x1)*(y0 - y2);
    if (std::abs(denom) < 1e-6f) return; // degenerate triangle

    // compute face normal in camera space for shading
    Vec3 e1 = v1c - v0c;
    Vec3 e2v = v2c - v0c;
    Vec3 normal = e1.cross(e2v);
    float nlen = normal.length();
    if (nlen == 0) return;
    Vec3 nunit = { normal.x / nlen, normal.y / nlen, normal.z / nlen };
    // simple directional light from camera (towards -z)
    Vec3 light = {0.0f, 0.0f, -1.0f};
    float llen = light.length();
    light = { light.x/llen, light.y/llen, light.z/llen };
    float intensity = nunit.x*light.x + nunit.y*light.y + nunit.z*light.z;
    if (intensity < 0) intensity = 0.0f; // no negative
    float ambient = 0.2f;
    float shade = ambient + (1.0f - ambient) * intensity;

    bool anyPixelDrawn = false;

    for (int y = minY; y <= maxY; ++y){
        for (int x = minX; x <= maxX; ++x){
            // barycentric
            float w0 = ((y1 - y2)*(x - x2) + (x2 - x1)*(y - y2)) / denom;
            float w1 = ((y2 - y0)*(x - x2) + (x0 - x2)*(y - y2)) / denom;
            float w2 = 1.0f - w0 - w1;
            if (w0 < -1e-4f || w1 < -1e-4f || w2 < -1e-4f) continue; // outside

            // interpolate depth (camera-space z)
            float depth = w0 * v0c.z + w1 * v1c.z + w2 * v2c.z;

            int idx = y*im.width() + x;
            if (depth < zbuffer[idx]){
                zbuffer[idx] = depth;
                int rr = std::min(255, std::max(0, int(baseR * shade)));
                int gg = std::min(255, std::max(0, int(baseG * shade)));
                int bb = std::min(255, std::max(0, int(baseB * shade)));
                drawPixel(x, y, rr, gg, bb);
                anyPixelDrawn = true;
            }
        }
    }

    // if needed, we could return anyPixelDrawn to detect fully hidden faces
}

void Ekran::drawCube(
    float tx, float ty, float tz,
    float ax, float ay, float az,
    float sx, float sy, float sz
    )
{
    im.fill(0xff000000);
    // reset z-buffer
    zbuffer.assign(im.width()*im.height(), std::numeric_limits<float>::infinity());

    qDebug() << tx << ty << tz <<sx <<sy <<sz << ax << ay << az;
    std::vector<Vec3> cube = {
        {-75,-75,-75}, {75,-75,-75}, {75,75,-75}, {-75,75,-75},
        {-75,-75, 75}, {75,-75, 75}, {75,75, 75}, {-75,75, 75}
    };

    // each face has 4 vertices (indices into cube), will be split into two triangles
    int faces[6][4] = {
        {0,1,2,3}, // back
        {4,5,6,7}, // front
        {0,1,5,4}, // bottom
        {2,3,7,6}, // top
        {1,2,6,5}, // right
        {0,3,7,4}  // left
    };

    Mat4 M = CreateMacierz( tx,  ty,  tz,  az,  ay,  ax,  sx,  sy,  sz);

    // transform all vertices to camera/world space
    std::vector<Vec3> tv;
    std::vector<QPoint> p;
    for (auto& v : cube){
        Vec3 tc = M * v; // transformed (camera/world)
        tv.push_back(tc);
        p.push_back(project(tc));
    }

    // simple per-face base colors (different for each face)
    int faceColors[6][3] = {{200,40,40},{40,200,40},{40,40,200},{200,200,40},{200,40,200},{40,200,200}};

    // for each face, rasterize two triangles
    for (int i=0;i<6;i++){
        int a = faces[i][0];
        int b = faces[i][1];
        int c = faces[i][2];
        int d = faces[i][3];

        // triangle 1: a,b,c
        drawTriangle(tv[a], tv[b], tv[c], p[a], p[b], p[c], faceColors[i][0], faceColors[i][1], faceColors[i][2]);
        // triangle 2: a,c,d
        drawTriangle(tv[a], tv[c], tv[d], p[a], p[c], p[d], faceColors[i][0], faceColors[i][1], faceColors[i][2]);
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
