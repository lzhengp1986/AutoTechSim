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

    /* 设置懊悔值 */
    m_regret = new QLineSeries;
    m_regret->setPen(QPen(Qt::cyan, 1));
    m_regret->setName("regret");
    m_regret->setOpacity(0.5);
    chart->addSeries(m_regret);

    /* step3.设置x坐标轴 */
    QValueAxis *x0 = new QValueAxis;
    x0->setTitleText("frequency/MHz");
    x0->setRange(2, 30); /* MHz */
    x0->setTickCount(15);
    x0->setMinorTickCount(1);
    x0->setLabelFormat("%u");
    x0->setTitleVisible(false);
    x0->setMinorGridLineVisible(false);
    x0->setGridLineVisible(false);

    QValueAxis *x1 = new QValueAxis;
    x1->setTitleText("regrets");
    x1->setRange(0, 100);
    x1->setTickCount(6);
    x1->setMinorTickCount(1);
    x1->setLabelFormat("%d");
    x1->setTitleVisible(false);
    x1->setMinorGridLineVisible(false);
    x1->setGridLineVisible(false);
    x1->setLabelsVisible(true);
    x1->setVisible(true);
    m_axisX1 = x1;

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
    chart->addAxis(x0, Qt::AlignLeft);
    chart->addAxis(x1, Qt::AlignLeft);
    chart->addAxis(y0, Qt::AlignBottom);
    chart->addAxis(y1, Qt::AlignTop);
    m_scan->attachAxis(x0);
    m_scan->attachAxis(y0);
    m_link->attachAxis(x0);
    m_link->attachAxis(y0);
    m_noise->attachAxis(x0);
    m_noise->attachAxis(y1);
    m_regret->attachAxis(x1);
    m_regret->attachAxis(y0);

    /* 设置图形区域 */
    bool flag = false;
    x0->setVisible(flag);
    y0->setVisible(flag);
    y1->setVisible(flag);
    x0->setLabelsVisible(flag);
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

void WChart::set_regret_color(QColor color)
{
    m_regret->setColor(color);
}

QChart* WChart::get_chart(void) const
{
    return m_chart;
}

void WChart::plot(float hour, float fc, int snr, int regret)
{
    m_scan->append(hour, fc);
    m_regret->append(hour, regret);
    if (snr > MIN_SNR) {
        m_link->append(hour, fc);
    }

    /* 懊悔值：调整坐标范围 */
    qreal max = m_axisX1->max();
    if (regret > max) {
        qreal min = m_axisX1->min();
        m_axisX1->setRange(min, max * 2);
    }

    m_chart->update();
}

void WChart::plot(const int* noise, int n)
{
    int i, j;
    m_noise->clear();
    for (i = j = 0; j < n; i += 15, j++) {
        qreal k = GLB2FREQ(i) / 1000.0;
        m_noise->append(noise[j], k);
    }
    m_chart->update();
}

void WChart::clear(void)
{
    m_noise->clear();
    m_regret->clear();
    m_scan->clear();
    m_link->clear();
    m_chart->update();
}
