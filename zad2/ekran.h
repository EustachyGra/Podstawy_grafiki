#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <math.h>
#include <algorithm>
#include <QMouseEvent>
#include <QPainter>

class Ekran : public QWidget
{
    Q_OBJECT
    QImage im;
    QPoint last;
public:
    explicit Ekran(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void drawToPoint(QPoint now);
    void drawPixel(int x, int y, int r, int g, int b);
signals:

};

#endif // EKRAN_H
