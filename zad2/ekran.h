#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <math.h>
#include <QStack>
#include <algorithm>
#include<QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QSlider>
#include <QColorDialog>
#include <QDebug>


enum class DrawType{
    Line,
    Circle,
    Elipse,
    AddP,
    DelP,
    MvP,
    None,
    Fill,
    AddPolP,
    DelPolP,
    MvPolP
};


class Ekran : public QWidget
{
    Q_OBJECT
    QImage im;
    bool isPressed = false;
    DrawType type;
    QPoint start, last;
    QImage im_save;
    std::vector<QPoint> tabPi, polPi;
    int idxPi = -1;
    int r;
    int g;
    int b;
public:
    explicit Ekran(QWidget *parent = nullptr);
protected:
    float calcLength(QPoint p1, QPoint p2);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    void drawToPoint(QPoint now, QPoint prev, bool solid = true);
    void drawCurve();
    void drawPixel(int x, int y);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void drawPoints();
    void drawCircle(QPoint now, QPoint prev);
    void drawElipse(QPoint now, QPoint prev);
    void drawSquare(QPoint center, int a);
    void keyPressEvent (QKeyEvent *ev);
    void keyReleaseEvent (QKeyEvent *ev);
    void drawCurvePoints();
    void drawFilledPolygon();
    void drawPolygonPoints();
    void ResetButtons();
    void saveCurve();
    void savePolygon();
    bool isLineCrossed(QPoint l1, QPoint l2, int y);
    QPoint findCrossPoint(QPoint l1, QPoint l2, int y);
    void floodFill(QPoint point, QColor color);
signals:
    void ColorChange(QColor col);
    void currentType(int i);
public slots:
    void curveChange(int i);
    void lineChange();
    void circChange();
    void addPointSet();
    void delPointSet();
    void movePointSet();
    void addPolygonPointSet();
    void delPolygonPointSet();
    void movePolygonPointSet();
    void colorDialog();
    void fillChange();
    void polygonChange(int i);
private:
    void showEvent(QShowEvent *event);
};

#endif // EKRAN_H
