#include "ekran.h"
#include <QDebug>
#include <algorithm>

Ekran::Ekran(QWidget *parent) : QWidget(parent) {}

void Ekran::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Bg = QImage(size(), QImage::Format_ARGB32);
    Bg.fill(0xff000000);
}

void Ekran::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QImage result = Bg;

    for (int i = 0; i < layers.size(); i++)
    {
        QImage layer = layers[i];
        if (layer.size() != result.size())
            layer = layer.scaled(result.size());

        switch (layersType[i])
        {
        case 0:
            result = Normal(result, layer, layerAlpha[i]);
            break;
        case 1:
            result = Sum(result, layer, layerAlpha[i]);
            break;
        case 2:
            result = Multiply(result, layer, layerAlpha[i]);
            break;
        case 3:
            result = Xor(result, layer, layerAlpha[i]);
            break;
        case 4:
            result = Darker(result, layer, layerAlpha[i]);
            break;
        case 5:
            result = Brighter(result, layer, layerAlpha[i]);
            break;
        default:
            result = Normal(result, layer, layerAlpha[i]);
            break;
        }
    }

    p.drawImage(0, 0, result);
}

QImage Ekran::Normal(const QImage &bg, const QImage &l, int aLayer)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            float a = (aLayer / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
                o[4*x+c] = b[4*x+c] * (1-a) + ly[4*x+c] * a;

            o[4*x+3] = 255;
        }
    }
    return out;
}

QImage Ekran::Sum(const QImage &bg, const QImage &l, int aLayer)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            float a = (aLayer / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
            {
                int s = std::min(255, b[4*x+c] + ly[4*x+c]);
                o[4*x+c] = b[4*x+c] * (1-a) + s * a;
            }
            o[4*x+3] = 255;
        }
    }
    return out;
}

QImage Ekran::Multiply(const QImage &bg, const QImage &l, int aLayer)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            float a = (aLayer / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
            {
                int m = (b[4*x+c] * ly[4*x+c]) >> 8;
                o[4*x+c] = b[4*x+c] * (1-a) + m * a;
            }
            o[4*x+3] = 255;
        }
    }
    return out;
}

QImage Ekran::Xor(const QImage &bg, const QImage &l, int aLayer)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            float a = (aLayer / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
            {
                int v = b[4*x+c] ^ ly[4*x+c];
                o[4*x+c] = b[4*x+c] * (1-a) + v * a;
            }
            o[4*x+3] = 255;
        }
    }
    return out;
}

int Ekran::greyValue(int r, int g, int b)
{
    return (r + g + b) / 3;
}

QImage Ekran::Darker(const QImage &bg, const QImage &l, int alpha)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b  = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            int bgGrey = greyValue(b[4*x+2], b[4*x+1], b[4*x]);
            int lyGrey = greyValue(ly[4*x+2], ly[4*x+1], ly[4*x]);

            const uchar *chosen = (lyGrey < bgGrey) ? ly : b;

            float a = (alpha / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
                o[4*x+c] = b[4*x+c] * (1-a) + chosen[4*x+c] * a;

            o[4*x+3] = 255;
        }
    }
    return out;
}

QImage Ekran::Brighter(const QImage &bg, const QImage &l, int alpha)
{
    QImage out(bg.size(), QImage::Format_ARGB32);

    for (int y = 0; y < bg.height(); y++)
    {
        const uchar *b  = bg.constScanLine(y);
        const uchar *ly = l.constScanLine(y);
        uchar *o = out.scanLine(y);

        for (int x = 0; x < bg.width(); x++)
        {
            int bgGrey = greyValue(b[4*x+2], b[4*x+1], b[4*x]);
            int lyGrey = greyValue(ly[4*x+2], ly[4*x+1], ly[4*x]);

            const uchar *chosen = (lyGrey > bgGrey) ? ly : b;

            float a = (alpha / 255.0f) * (ly[4*x+3] / 255.0f);

            for (int c = 0; c < 3; c++)
                o[4*x+c] = b[4*x+c] * (1-a) + chosen[4*x+c] * a;

            o[4*x+3] = 255;
        }
    }
    return out;
}


void Ekran::loadImg(const QString &name)
{
    QImage img;
    if (!img.load(name))
    {
        qDebug() << "Nie mozna wczytac:" << name;
        return;
    }

    layers.push_back(img.convertToFormat(QImage::Format_ARGB32));
    layersType.push_back(0);
    layerAlpha.push_back(255);
    currentLayer = layers.size() - 1;
    update();
}

void Ekran::SetAlpha(int alpha)
{
    if (currentLayer < 0) return;
    layerAlpha[currentLayer] = alpha;
    update();
}

void Ekran::LayerChanged(int i)
{
    if (i < 0 || i >= layers.size()) return;
    currentLayer = i;
    emit CurrentLayerType(layersType[i]);
    emit CurrentLayerAlpha(layerAlpha[i]);
    update();
}

void Ekran::SetCurrentType(int type)
{
    if (currentLayer < 0) return;
    layersType[currentLayer] = type;
    update();
}
