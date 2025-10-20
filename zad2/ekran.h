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
    QPoint last;
    int r;
    int g;
    int b;
public:
    explicit Ekran(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void drawToPoint(QPoint now, QPoint prev);
    void drawPixel(int x, int y);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
signals:

};

#endif // EKRAN_H
