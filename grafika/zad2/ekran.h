// ----------------------------- ekran.h -----------------------------
#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QFileDialog>
#include <vector>

struct Vec3 {
    float x=0, y=0, z=0;
    Vec3(){}
    Vec3(float X, float Y, float Z):x(X),y(Y),z(Z){}
    Vec3 operator-(const Vec3& o) const { return {x-o.x,y-o.y,z-o.z}; }
    Vec3 operator+(const Vec3& o) const { return {x+o.x,y+o.y,z+o.z}; }
    Vec3 operator*(float s) const { return {x*s,y*s,z*s}; }
};

struct RGB { int r,g,b,a; RGB(int R=0,int G=0,int B=0,int A=255):r(R),g(G),b(B),a(A){}
};

struct Face {
    int v[3];   // vertex indices
    int vt[3];  // texture coordinate indices
    Face() { v[0]=v[1]=v[2]=0; vt[0]=vt[1]=vt[2]=0; }
};

// Minimal 4x4 matrix with operations used in original code
struct Mat4 {
    float m[4][4];
    static Mat4 Identity() {
        Mat4 M; memset(M.m,0,sizeof(M.m));
        for (int i=0;i<4;i++) M.m[i][i]=1.0f;
        return M;
    }
    void translate(float tx,float ty,float tz){
        Mat4 T = Identity();
        T.m[0][3]=tx; T.m[1][3]=ty; T.m[2][3]=tz;
        *this = T * (*this);
    }
    void scale(float sx,float sy,float sz){
        Mat4 S = Identity();
        S.m[0][0]=sx; S.m[1][1]=sy; S.m[2][2]=sz;
        *this = S * (*this);
    }
    void rotateX(float a){ float r = a * M_PI/180.0f; Mat4 R = Identity(); R.m[1][1]=cosf(r); R.m[1][2]=-sinf(r); R.m[2][1]=sinf(r); R.m[2][2]=cosf(r); *this = R * (*this); }
    void rotateY(float a){ float r = a * M_PI/180.0f; Mat4 R = Identity(); R.m[0][0]=cosf(r); R.m[0][2]=sinf(r); R.m[2][0]=-sinf(r); R.m[2][2]=cosf(r); *this = R * (*this); }
    void rotateZ(float a){ float r = a * M_PI/180.0f; Mat4 R = Identity(); R.m[0][0]=cosf(r); R.m[0][1]=-sinf(r); R.m[1][0]=sinf(r); R.m[1][1]=cosf(r); *this = R * (*this); }

    Mat4 operator*(const Mat4& o) const {
        Mat4 R; memset(R.m,0,sizeof(R.m));
        for (int i=0;i<4;i++) for (int j=0;j<4;j++) for (int k=0;k<4;k++) R.m[i][j]+=m[i][k]*o.m[k][j];
        return R;
    }
    Vec3 operator*(const Vec3& v) const {
        float x = v.x*m[0][0] + v.y*m[0][1] + v.z*m[0][2] + m[0][3];
        float y = v.x*m[1][0] + v.y*m[1][1] + v.z*m[1][2] + m[1][3];
        float z = v.x*m[2][0] + v.y*m[2][1] + v.z*m[2][2] + m[2][3];
        float w = v.x*m[3][0] + v.y*m[3][1] + v.z*m[3][2] + m[3][3];
        if (fabsf(w) > 1e-6f) { x/=w; y/=w; z/=w; }
        return Vec3(x,y,z);
    }
};

class Ekran : public QWidget {
    Q_OBJECT
public:
    explicit Ekran(QWidget* parent=nullptr);
protected:
    void showEvent(QShowEvent*) override;
    void paintEvent(QPaintEvent*) override;

public:
    QImage im;
    QImage texture;
    std::vector<float> zbuffer;

    std::vector<Vec3> vertices;
    std::vector<QPointF> texcoords; // u,v in [0,1]
    std::vector<Face> faces;

    QPoint TA,TB,TC,TD;
    float d = 300.0f;

    // helper
    Vec3 cross(const Vec3& a, const Vec3& b);
    float dot(const Vec3& a, const Vec3& b);
    bool isBackface(const Vec3& a, const Vec3& b, const Vec3& c);

    // rendering
    void drawPixel(int x,int y,int r,int g,int b,int a);
    QPoint project(const Vec3& v);
    void drawTriangle(QPoint p0, QPoint p1, QPoint p2,
                      float z0, float z1, float z2,
                      QPointF uv0, QPointF uv1, QPointF uv2);

    // obj + texture
    bool loadOBJ(const QString& path="");
    Mat4 CreateMacierz(float tx,float ty,float tz, float az,float ay,float ax, float sx,float sy,float sz);
    void drawModel(float tx, float ty, float tz, float az, float ay, float ax, float sx, float sy, float sz);
    void importTexture();

    bool isLineCrossed(QPoint l1, QPoint l2, int y);
    float findCrossPoint(QPoint l1,QPoint l2,int y);
    RGB getRGB(QPoint p);
    void getRGBA(double u,double v,double w,double invZ0,double invZ1,double invZ2,int& r,int& g,int& b,int& a);
};

#endif // EKRAN_H

