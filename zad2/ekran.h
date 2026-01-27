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
#include <QFileDialog>
#include <QDebug>
#include <algorithm>
#include <limits>

struct RGB{
    int r,g,b,a;
    RGB(){};
    RGB(int _r, int _g, int _b, int _a)
    {
        r=_r;
        g=_g;
        b=_b;
        a=_a;
    }
    RGB operator*(double a)
    {
        RGB tmp;
        tmp.r=this->r*a;
        tmp.g=this->g*a;
        tmp.b=this->b*a;
        tmp.a=this->a*a;
        return tmp;
    }
    RGB operator+(RGB c)
    {
        RGB tmp;
        tmp.r=(this->r+c.r)%255;
        tmp.g=(this->g+c.g)%255;
        tmp.b=(this->b+c.b)%255;
        tmp.a=(this->a+c.a)%255;
        return tmp;
    }
};

struct Vec3
{
    float x, y, z;
    Vec3 operator-(const Vec3 &v2) const
    {
        return { x - v2.x, y - v2.y, z - v2.z };
    }
    float operator*(const Vec3 &v2) const
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
        Vec3 tmp;
        tmp.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3];
        tmp.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3];
        tmp.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3];
        float w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3];
        if (w != 0.0f && w != 1.0f)
        {
            tmp.x/=w;
            tmp.y/=w;
            tmp.z/=w;
        }
        return tmp;
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
    QPoint origin;
    QPoint start, last;
    QImage texture;
    std::vector<QPoint> polPi;
    std::vector<QPoint> polT;
    int idxPi = -1;
    int r = 255, g = 255, b = 255;
    float d = 250.0f;
    QPoint TA,TB,TC,TD;
    std::vector<float> zbuffer;
    std::vector<double> invZ;

public:
    explicit Ekran(QWidget *parent=nullptr);

    QPoint project(const Vec3 &v);
    void drawCube(float tx, float ty, float tz, float ax, float ay, float az, float sx, float sy, float sz);
    void importTexture();
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent *);
    void drawPixel(int x, int y, int r, int g, int b, int a);
    Mat4 CreateMacierz(float tx, float ty, float tz, float az, float ay, float ax, float sx, float sy, float sz);
    void drawTriangle(QPoint p0, QPoint p1, QPoint p2, float z0, float z1, float z2, bool whichTriangle);
    bool isLineCrossed(QPoint l1, QPoint l2, int y);
    float findCrossPoint(QPoint l1, QPoint l2, int y);
    void getRGBA(double u, double v, double w, int& r, int& g, int& b, int& a, bool whichTriangle);
    RGB getRGB(QPoint p);
};

#endif // EKRAN_H


