#ifndef EKRAN_H
#define EKRAN_H

#include <QPainter>
#include <QWidget>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
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

class Ekran : public QWidget
{
    Q_OBJECT
public:
    explicit Ekran(QWidget *parent = nullptr);
    std::vector<QPoint> triangle;

    void loadImg(const QString &name);
    void mousePressEvent(QMouseEvent *e);
    QPoint getMinYMaxY();
    void getRGBA(double u, double v, double w, int &r, int &g, int &b, int &a);
    RGB getRGB(QPoint p);
    void drawPixel(int x, int y, int r, int g, int b, int a);
public slots:
    void resetPkt();
    void clean();
protected:
    void paintEvent(QPaintEvent *) override;
    void showEvent(QShowEvent *event) override;

private:
    QImage Bg;
};

#endif
