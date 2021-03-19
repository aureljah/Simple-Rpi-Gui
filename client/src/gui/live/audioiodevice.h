#ifndef AUDIOIODEVICE_H
#define AUDIOIODEVICE_H

#include <QtCore/QIODevice>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class audioIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit audioIODevice(QXYSeries *series, QObject *parent = nullptr);

    static const int sampleCount = 2000;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QXYSeries *m_series;
    QVector<QPointF> m_buffer;
};
#endif // AUDIOIODEVICE_H
