#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QVector>

class Ekran : public QWidget
{
    Q_OBJECT
public:
    explicit Ekran(QWidget *parent = nullptr);

    void loadImg(const QString &name);
    void SetAlpha(int alpha);
    void SetCurrentType(int type);
    void LayerChanged(int i);

signals:
    void CurrentLayerType(int);
    void CurrentLayerAlpha(int);

protected:
    void paintEvent(QPaintEvent *) override;
    void showEvent(QShowEvent *event) override;

private:
    QImage Bg;

    QVector<QImage> layers;
    QVector<int> layersType;
    QVector<int> layerAlpha;

    int currentLayer = -1;

    // blend modes
    QImage Normal(const QImage &bg, const QImage &layer, int alpha);
    QImage Sum(const QImage &bg, const QImage &layer, int alpha);
    QImage Multiply(const QImage &bg, const QImage &layer, int alpha);
    QImage Xor(const QImage &bg, const QImage &layer, int alpha);
    QImage Darker(const QImage &bg, const QImage &layer, int alpha);
    QImage Brighter(const QImage &bg, const QImage &layer, int alpha);

    int greyValue(int r, int g, int b);
};

#endif
