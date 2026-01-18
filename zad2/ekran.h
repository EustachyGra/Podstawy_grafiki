#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>

enum class DrawType{
    None,
    setOrigin,
    Fill,
    AddPolP,
    DelPolP,
    MvPolP
};
struct vec{
    int x;
    int y;
    int z;
    vec();
    vec(int _x, int _y, int _z=1){
        x=_x;
        y=_y;
        z=_z;
    }
    vec(QPoint p)
    {
        x=p.x();
        y=p.y();
        z=1;
    }
    vec operator+=(vec p)
    {
        this->x+=p.x;
        this->y+=p.y;
        this->z+=p.z;
        return *this;
    }
    QPoint getPoint()
    {
        return QPoint(x,y);
    }
};

struct macierz{
    float M[3][3] = {
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };
    void transform(float tx, float ty)
    {
        M[0][2]=tx;
        M[1][2]=ty;
    }
    void scale(float sx, float sy)
    {
        M[0][0]=sx;
        M[1][1]=sy;
    }
    void shear(float hx,float hy)
    {
        M[0][1]=hx;
        M[1][0]=hy;
    }
    void rotate(int alpha)
    {
        float rad = float(alpha) * (float(M_PI)/180.f);
        float c = cos(rad);
        float s = sin(rad);
        M[0][0]= c;
        M[0][1]= s;
        M[1][0]= -s;
        M[1][1]= c;
    }
    vec operator*(vec v)
    {
        vec tmp(0,0);
        tmp.x = M[0][0]*v.x+M[0][1]*v.y+M[0][2]*v.z;
        tmp.y = M[1][0]*v.x+M[1][1]*v.y+M[1][2]*v.z;
        tmp.z = M[2][0]*v.x+M[2][1]*v.y+M[2][2]*v.z;
        return tmp;
    }
    macierz operator*(const macierz& m)
    {
        macierz tmp;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                tmp.M[i][j]=0;

        for (int i = 0; i < 3; i++)
            for (int k = 0; k < 3; k++)
                for (int j = 0; j < 3; j++)
                    tmp.M[i][k] += this->M[i][j] * m.M[j][k];

        return tmp;
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
    int r;
    int g;
    int b;

public:
    explicit Ekran(QWidget *parent = nullptr);

protected:
    // eventy
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

    // pomocnicze
    float calcLength(QPoint p1, QPoint p2);
    void drawToPoint(QPoint now, QPoint prev);
    void drawPixel(int x, int y, int r=255, int g=255, int b =255);
    void drawPoints();
    void drawSquare(QPoint center, int a, int r=255, int g=255, int b=255);

    // wielokąt
    void drawFilledPolygon(std::vector<QPoint> vec);
    void drawPolygonPoints();
    void ResetButtons();
    void savePolygon();

    // scanline helpers
    bool isLineCrossed(QPoint l1, QPoint l2, int y);
    QPoint findCrossPoint(QPoint l1, QPoint l2, int y);

    // flood fill
    void floodFill(QPoint point, QColor color);

signals:
    void ColorChange(QColor col);
    void currentType(int i);
    void oseted();

public slots:
    void addPolygonPointSet();
    void delPolygonPointSet();
    void movePolygonPointSet();
    QPoint applyMatrix(int tx, int ty, int sx, int sy, int a, int shx, int shy);
    macierz createMacierz(float tx, float ty, float sx, float sy, float a, float shx, float shy);
    void colorDialog();
    void polygonChange(int i);
    void originset();
};

#endif // EKRAN_H
