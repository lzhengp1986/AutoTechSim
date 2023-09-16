#include "macro.h"
#include "wchart.h"
#include <QValueAxis>

WChart::WChart(void)
{
    /* step1.建立chart */
    QChart* chart = new QChart();
    foreach (auto ax, chart->axes()) {
        chart->removeAxis(ax);
    }
    chart->removeAllSeries();
    m_chart = chart;

    /* step2.设置散点 */
    m_scan = new QScatterSeries;
    m_link = new QScatterSeries;
    chart->addSeries(m_scan);
    chart->addSeries(m_link);
    m_scan->setName("scan");
    m_link->setName("link");
    m_scan->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    m_link->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scan->setBorderColor(Qt::black);
    m_link->setBorderColor(Qt::blue);
    m_scan->setBrush(QBrush(Qt::black));
    m_link->setBrush(QBrush(Qt::blue));
    m_scan->setMarkerSize(1);
    m_link->setMarkerSize(1);

    /* 设置底噪 */
    m_noise = new QLineSeries;
    m_noise->setPen(QPen(Qt::darkGray, 1));
    m_noise->setName("noise");
    m_noise->setOpacity(0.5);
    chart->addSeries(m_noise);

    /* step3.设置x坐标轴 */
    QValueAxis *x = new QValueAxis;
    x->setTitleText("frequency/MHz");
    x->setRange(2, 30); /* MHz */
    x->setTickCount(15);
    x->setMinorTickCount(1);
    x->setLabelFormat("%u");
    x->setTitleVisible(false);
    x->setMinorGridLineVisible(false);
    x->setGridLineVisible(false);

    /* 设置y0坐标轴 */
    QValueAxis *y0 = new QValueAxis;
    y0->setTitleText("UT/hour");
    y0->setRange(0, 24); /* hour */
    y0->setTickCount(13);
    y0->setMinorTickCount(1);
    y0->setLabelFormat("%u");
    y0->setTitleVisible(false);
    y0->setMinorGridLineVisible(false);
    y0->setGridLineVisible(false);

    /* 设置y1坐标轴 */
    QValueAxis *y1 = new QValueAxis;
    y1->setTitleText("N0/dB");
    y1->setRange(-140, 20);
    y1->setTickCount(8);
    y1->setMinorTickCount(1);
    y1->setLabelFormat("%d");
    y1->setTitleVisible(false);
    y1->setMinorGridLineVisible(false);
    y1->setGridLineVisible(false);

    /* step4.添加坐标轴 */
    chart->addAxis(x, Qt::AlignLeft);
    chart->addAxis(y0, Qt::AlignBottom);
    chart->addAxis(y1, Qt::AlignTop);
    m_scan->attachAxis(x);
    m_scan->attachAxis(y0);
    m_link->attachAxis(x);
    m_link->attachAxis(y0);
    m_noise->attachAxis(x);
    m_noise->attachAxis(y1);

    /* 设置图形区域 */
    bool flag = false;
    x->setVisible(flag);
    y0->setVisible(flag);
    y1->setVisible(flag);
    x->setLabelsVisible(flag);
    y0->setLabelsVisible(flag);
    y1->setLabelsVisible(flag);
    QRect rect(47, 92, 544, 409);
    chart->setPlotArea(rect);

    /* 设置chartview透明 */
    chart->setBackgroundVisible(false);
    chart->legend()->setVisible(false);
    chart->show();
}

WChart::~WChart(void)
{
    delete m_chart;
    m_chart = nullptr;
}

void WChart::set_scan_color(QColor color)
{
    m_scan->setBorderColor(color);
    m_scan->setBrush(QBrush(color));
}

void WChart::set_link_color(QColor color)
{
    m_link->setBorderColor(color);
    m_link->setBrush(QBrush(color));
}

QChart* WChart::get_chart(void) const
{
    return m_chart;
}

void WChart::plot(float hour, float fc, int snr)
{
    m_scan->append(hour, fc);
    if (snr > MIN_SNR) {
        m_link->append(hour, fc);
    }
    m_chart->update();
}

void WChart::plot(bool withNoise)
{
    static int s_noise[640] = {
        #include "noise.txt"
    };

    m_noise->clear();
    if (withNoise == true) {
        int i, j;
        for (i = j = 0; i < MAX_GLB_CHN; i += 15, j++) {
            qreal k = GLB2FREQ(i) / 1000.0;
            m_noise->append(s_noise[j], k);
        }
    }
    m_chart->update();
}

void WChart::clear(void)
{
    m_noise->clear();
    m_scan->clear();
    m_link->clear();
    m_chart->update();
}
