#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <math.h>
#include <algorithm>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
#include <map>

class Ekran : public QWidget
{
    Q_OBJECT
    QImage im, red,blue,green,preview;
    bool isPressed = false;
    QPoint start, last;
    QImage im_save;
    int r;
    int g;
    int b;
public:
    explicit Ekran(QWidget *parent = nullptr);
protected:
    bool clickOnRed(QPoint pos);
    bool clickOnGreen(QPoint pos);
    bool clickOnBlue(QPoint pos);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void drawToPoint(QPoint now, QPoint prev);
    void drawPixel(int x, int y);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void imageFrame(QImage& im, int size);
    void drawPoints();
    void drawCircle(QPoint now, QPoint prev);
    void drawElipse(QPoint now, QPoint prev);
signals:

};

#endif // EKRAN_H
