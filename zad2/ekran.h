#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <math.h>
#include <algorithm>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
class Ekran : public QWidget
{
    Q_OBJECT
    QImage im;
    bool isPressed = false;
    QPoint start, last;
   // std::vector<QPoint> points;
    int r;
    int g;
    int b;
public:
    explicit Ekran(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void drawToPoint(QPoint now, QPoint prev, int _r=0 ,int _g=0 ,int _b=0);// , bool save=false);
    // (opcjonalnie) void drawPixel(int x, int y, int R ,int G ,int B ) i wtedy w .cpp trzeba przy wywolaniu tej funckji podac (x,y,0,0,0)  tam gdzie wczesniej byl tylko (x,y)
    // i zmienic w deklaracji funkcji nazyw zmiennych (czyli void Ekran::drawToPoint(QPoint now, QPoint prev, int R, int G, int B)
    void drawPixel(int x, int y, int _r=0 ,int _g=0 ,int _b=0 );
    // tu podobnie
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
  //  void drawPoints();
signals:

};

#endif // EKRAN_H
