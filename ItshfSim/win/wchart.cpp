#include "macro.h"
#include "wchart.h"
#include <QValueAxis>

WChart::WChart(void)
{
    /* step1.建立chart */
    QChart* chart = new QChart();
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

    /* step4.添加坐标轴 */
    chart->addAxis(x, Qt::AlignLeft);
    chart->addAxis(y0, Qt::AlignBottom);
    m_scan->attachAxis(x);
    m_scan->attachAxis(y0);
    m_link->attachAxis(x);
    m_link->attachAxis(y0);

    /* 设置图形区域 */
    bool flag = false;
    x->setVisible(flag);
    y0->setVisible(flag);
    x->setLabelsVisible(flag);
    y0->setLabelsVisible(flag);
    QRect rect(47, 92, 544, 409);
    chart->setPlotArea(rect);

    /* 设置chartview透明 */
    chart->setBackgroundVisible(true);
    chart->legend()->setVisible(false);
}

WChart::~WChart(void)
{
    delete m_chart;
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
    m_chart->show();
}

void WChart::clear(void)
{
    m_scan->clear();
    m_link->clear();
}
