// ========================= Ekran.h =========================
#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <cmath>
#include <QDebug>
#include <algorithm>
#include <limits>

enum class DrawType{
    None,
    setOrigin,
    Fill,
    AddPolP,
    DelPolP,
    MvPolP
};

struct Vec3
{
    float x, y, z;
    Vec3 operator-(const Vec3 &v2) const
    {
        return { x - v2.x, y - v2.y, z - v2.z };
    }
    float operator*(const Vec3 &v2) const // dot product
    {
        return x*v2.x + y*v2.y + z*v2.z;
    }
    Vec3 operator+(const Vec3 &v2) const { return {x+v2.x, y+v2.y, z+v2.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
    Vec3 cross(const Vec3 &v) const {
        return { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x };
    }
    float length() const { return std::sqrt(x*x + y*y + z*z); }
};

struct Mat4
{
    float m[4][4] = {};

    static Mat4 Identity()
    {
        Mat4 tmp;
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                tmp.m[i][j] = (i==j ? 1.0f : 0.0f);
        return tmp;
    }

    Vec3 operator*(const Vec3& v) const
    {
        float x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3];
        float y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3];
        float z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3];
        float w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3];
        if (w != 0.0f && w != 1.0f) { x/=w; y/=w; z/=w; }
        return {x, y, z};
    }

    Mat4 operator*(const Mat4 &o) const
    {
        Mat4 r;
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++){
                r.m[i][j]=0;
                for(int k=0;k<4;k++)
                    r.m[i][j] += m[i][k] * o.m[k][j];
            }
        return r;
    }

    Mat4 translate(float tx, float ty, float tz)
    {
        this->m[0][3] = tx;
        this->m[1][3] = ty;
        this->m[2][3] = tz;
        return *this;
    }

    Mat4 scale(float sx, float sy, float sz)
    {
        this->m[0][0] = sx;
        this->m[1][1] = sy;
        this->m[2][2] = sz;
        return *this;
    }

    Mat4 rotateX(float a)
    {
        float r = a*M_PI/180.0f;
        this->m[1][1] = cos(r); this->m[1][2] = -sin(r);
        this->m[2][1] = sin(r); this->m[2][2] = cos(r);
        return *this;
    }

    Mat4 rotateY(float a)
    {
        float r = a*M_PI/180.0f;
        this->m[0][0] = cos(r);  this->m[0][2] = sin(r);
        this->m[2][0] = -sin(r); this->m[2][2] = cos(r);
        return *this;
    }

    Mat4 rotateZ(float a)
    {
        float r = a*M_PI/180.0f;
        this->m[0][0] = cos(r); this->m[0][1] = -sin(r);
        this->m[1][0] = sin(r); this->m[1][1] = cos(r);
        return *this;
    }
};

class Ekran : public QWidget
{
    Q_OBJECT
    QImage im;
    bool isPressed = false;
    DrawType type;
    QPoint origin;
    QPoint start, last;
    QImage im_save;
    std::vector<QPoint> polPi;
    std::vector<QPoint> polT;
    int idxPi = -1;
    int r = 255, g = 255, b = 255;
    float d = 250.0f;

    // z-buffer (per-pixel depth)
    std::vector<float> zbuffer;

public:
    explicit Ekran(QWidget *parent=nullptr);

    QPoint project(const Vec3 &v);
    void drawCube(float tx, float ty, float tz, float ax, float ay, float az, float sx, float sy, float sz);

    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent *);
    void drawPixel(int x, int y, int r, int g, int b);
    void drawToPoint(QPoint a, QPoint b);
    Mat4 CreateMacierz(float tx, float ty, float tz, float az, float ay, float ax, float sx, float sy, float sz);

    // new: triangle rasterizer
    void drawTriangle(const Vec3 &v0c, const Vec3 &v1c, const Vec3 &v2c, const QPoint &p0, const QPoint &p1, const QPoint &p2, int baseR, int baseG, int baseB);
};

#endif // EKRAN_H


