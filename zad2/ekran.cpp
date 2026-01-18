#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget(parent)
{
    type = DrawType::AddPolP;
    r = g = b = 255;
    setFocusPolicy(Qt::StrongFocus);
    origin = QPoint(0, 0);
}

void Ekran::showEvent(QShowEvent *)
{
    im = QImage(width(), height(), QImage::Format_ARGB32);
    im.fill(0xff000000);
    im_save = im.copy();
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(0, 0, im);
}

float Ekran::calcLength(QPoint p1, QPoint p2)
{
    return std::sqrt(
        (p2.x() - p1.x()) * (p2.x() - p1.x()) +
        (p2.y() - p1.y()) * (p2.y() - p1.y()));
}

// ================== RYSOWANIE ==================

void Ekran::drawPixel(int x, int y, int r, int g, int b)
{
    if (x < 0 || y < 0 || x >= im.width() || y >= im.height())
        return;

    uchar *px = im.scanLine(y);
    px[4 * x + 0] = b;
    px[4 * x + 1] = g;
    px[4 * x + 2] = r;
    px[4 * x + 3] = 255;
}

void Ekran::drawToPoint(QPoint a, QPoint b)
{
    int x1 = a.x(), y1 = a.y();
    int x2 = b.x(), y2 = b.y();

    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void Ekran::drawSquare(QPoint c, int a,int r,int g,int b)
{
    int h = a / 2;
    for (int x = c.x() - h; x <= c.x() + h; x++)
    {
        drawPixel(x, c.y() - h,r,g,b);
        drawPixel(x, c.y() + h,r,g,b);
    }
    for (int y = c.y() - h; y <= c.y() + h; y++)
    {
        drawPixel(c.x() - h, y,r,g,b);
        drawPixel(c.x() + h, y,r,g,b);
    }
}

void Ekran::drawPolygonPoints()
{
    im = im_save.copy();
    drawSquare(origin,10,255,0,0);
    for (auto &p : polPi)
        drawSquare(p, 10);
}

void Ekran::drawFilledPolygon(std::vector<QPoint> vec)
{
    if (vec.size() < 2) return;

    for (size_t i = 0; i < vec.size() - 1; i++)
        drawToPoint(vec[i], vec[i+1]);

    if (vec.size() > 2)
        drawToPoint(vec.back(), vec.front());
}

// ================== MYSZ ==================

void Ekran::mousePressEvent(QMouseEvent *e)
{
    if (type == DrawType::AddPolP)
    {
        polPi.push_back(e->pos());
        drawPolygonPoints();
        drawFilledPolygon(polPi);
        update();
    }
    else if(type == DrawType::setOrigin)
    {
        origin = e->pos();
        drawPolygonPoints();
        drawFilledPolygon(polPi);
        update();
    }
    else if (type == DrawType::MvPolP)
    {
        for (size_t i = 0; i < polPi.size(); i++)
        {
            if(calcLength(e->pos(),polPi[i]) < 10)
                idxPi = static_cast<int>(i);
        }
    }
    else if(type == DrawType::DelPolP)
    {
        // usuwanie od końca, aby nie stracić indeksów
        for (int i = static_cast<int>(polPi.size()) - 1; i >= 0; i--)
        {
            if(calcLength(e->pos(),polPi[i]) < 10)
                polPi.erase(polPi.begin()+i);
        }
        drawPolygonPoints();
        drawFilledPolygon(polPi);
        update();
    }
}

void Ekran::mouseMoveEvent(QMouseEvent *e)
{
    if(type == DrawType::MvPolP && idxPi != -1)
    {
        polPi[idxPi] = e->pos();
        drawPolygonPoints();
        drawFilledPolygon(polPi);
        update();
    }
    else if(type == DrawType::setOrigin)
    {
        origin = e->pos();
        drawPolygonPoints();
        drawFilledPolygon(polPi);
        update();
    }
}

void Ekran::mouseReleaseEvent(QMouseEvent *)
{
    if(idxPi != -1)
        idxPi = -1;

    if(type == DrawType::setOrigin)
    {
        emit oseted();
        type = DrawType::None;
    }
}

// ================== TRANSFORMACJE ==================

QPoint Ekran::applyMatrix(int tx, int ty, int sx, int sy, int a, int shx, int shy)
{
    polT.clear();
    macierz M = createMacierz(tx, ty, sx, sy, a, shx, shy);

    for(size_t i = 0; i < polPi.size(); i++)
    {
        vec tmp(polPi[i] - origin);
        polT.push_back((M * tmp).getPoint() + origin);
    }

    drawPolygonPoints();
    drawFilledPolygon(polT);
    update();

    return QPoint(); // nie zwracamy nic sensownego, można zmienić na void
}

macierz Ekran::createMacierz(float tx, float ty, float sx, float sy, float a, float shx, float shy)
{
    macierz T, Sh, S, R;
    T.transform(tx, ty);
    Sh.shear(shx/20, shy/20);
    S.scale(1+sx/20, 1+sy/20);
    R.rotate(a);

    // Kolejność: najpierw skala, potem rotacja, shear, translacja
    return T * Sh * R * S;
}

// ================== POZOSTAŁE ==================

void Ekran::ResetButtons()
{
    type = DrawType::None;
    emit currentType(0);
}

void Ekran::addPolygonPointSet()
{
    type = DrawType::AddPolP;
    emit currentType(7);
}

void Ekran::delPolygonPointSet()
{
    type = DrawType::DelPolP;
    emit currentType(8);
}

void Ekran::movePolygonPointSet()
{
    type = DrawType::MvPolP;
    emit currentType(9);
}

void Ekran::polygonChange(int i)
{
    switch (i) {
    case 1: addPolygonPointSet(); break;
    case 2: delPolygonPointSet(); break;
    case 3: movePolygonPointSet(); break;
    default: break;
    }
}

void Ekran::originset()
{
    type = DrawType::setOrigin;
}

void Ekran::colorDialog() {}
void Ekran::savePolygon() {}
bool Ekran::isLineCrossed(QPoint, QPoint, int) { return false; }
QPoint Ekran::findCrossPoint(QPoint, QPoint, int) { return {}; }
void Ekran::floodFill(QPoint, QColor) {}
void Ekran::keyPressEvent(QKeyEvent *) {}
void Ekran::keyReleaseEvent(QKeyEvent *) {}
