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

    /* step2.设置通带 */
    m_upper = new QSplineSeries;
    m_lower = new QSplineSeries;
    m_upper->setName("upper");
    m_lower->setName("lower");
    m_upper->setPen(QPen(Qt::black, 1, Qt::DotLine));
    m_lower->setPen(QPen(Qt::black, 1, Qt::DotLine));
    chart->addSeries(m_upper);
    chart->addSeries(m_lower);

    /* 设置策略散点 */
    m_scan = new QScatterSeries;
    m_scan->setName("scan");
    m_scan->setOpacity(0.8);
    m_scan->setMarkerShape(QScatterSeries::MarkerShapeTriangle);
    m_scan->setBrush(QBrush(Qt::black));
    m_scan->setBorderColor(Qt::black);
    m_scan->setMarkerSize(2);
    chart->addSeries(m_scan);

    m_link = new QScatterSeries;
    m_link->setName("link");
    m_link->setMarkerShape(QScatterSeries::MarkerShapeStar);
    m_link->setBrush(QBrush(Qt::blue));
    m_link->setBorderColor(Qt::blue);
    m_link->setMarkerSize(2);
    chart->addSeries(m_link);

    /* 设置底噪 */
    m_noise = new QLineSeries;
    m_noise->setPen(QPen(Qt::darkGray, 1));
    m_noise->setName("noise");
    m_noise->setOpacity(0.5);
    chart->addSeries(m_noise);

    /* 设置懊悔值 */
    m_regret = new QLineSeries;
    m_regret->setPen(QPen(Qt::darkCyan, 1));
    m_regret->setName("regret");
    m_regret->setOpacity(0.5);
    chart->addSeries(m_regret);

    /* 设置平均试探频点数 */
    m_fcNum = new QLineSeries;
    m_fcNum->setPen(QPen(Qt::darkRed, 1));
    m_fcNum->setName("avgFreq");
    m_fcNum->setOpacity(0.5);
    chart->addSeries(m_fcNum);

    /* step3.设置x坐标轴 */
    QValueAxis *x0 = new QValueAxis;
    x0->setTitleText("Frequency/MHz");
    x0->setRange(2, 30); /* MHz */
    x0->setTickCount(15);
    x0->setMinorTickCount(1);
    x0->setLabelFormat("%u");
    x0->setTitleVisible(false);
    x0->setMinorGridLineVisible(false);
    x0->setGridLineVisible(false);
    x0->setLabelsVisible(false);
    x0->setVisible(false);

    QValueAxis *x1 = new QValueAxis;
    x1->setTitleText("Regrets");
    x1->setRange(0, 100);
    x1->setTickCount(6);
    x1->setMinorTickCount(1);
    x1->setLabelFormat("%d");
    x1->setTitleVisible(false);
    x1->setMinorGridLineVisible(false);
    x1->setGridLineVisible(false);
    x1->setLabelsFont(QFont("Arial", 6));
    x1->setLabelsVisible(true);
    x1->setVisible(true);
    m_axisX1 = x1;

    QValueAxis *x2 = new QValueAxis;
    x2->setTitleText("avgScanFc");
    x2->setRange(0, 10);
    x2->setTickCount(6);
    x2->setMinorTickCount(1);
    x2->setLabelFormat("%d");
    x2->setTitleVisible(false);
    x2->setMinorGridLineVisible(false);
    x2->setGridLineVisible(false);
    x2->setLabelsFont(QFont("Arial", 6));
    x2->setLabelsVisible(true);
    x2->setVisible(true);

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
    y0->setLabelsVisible(false);
    y0->setVisible(false);

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
    y1->setLabelsFont(QFont("Arial", 6));
    y1->setLabelsVisible(true);
    y1->setVisible(true);

    /* step4.添加坐标轴 */
    chart->addAxis(x0, Qt::AlignLeft);
    chart->addAxis(x1, Qt::AlignRight);
    chart->addAxis(x2, Qt::AlignRight);
    chart->addAxis(y0, Qt::AlignBottom);
    chart->addAxis(y1, Qt::AlignTop);
    m_upper->attachAxis(x0);
    m_upper->attachAxis(y0);
    m_lower->attachAxis(x0);
    m_lower->attachAxis(y0);
    m_scan->attachAxis(x0);
    m_scan->attachAxis(y0);
    m_link->attachAxis(x0);
    m_link->attachAxis(y0);
    m_fcNum->attachAxis(x2);
    m_fcNum->attachAxis(y0);
    m_noise->attachAxis(x0);
    m_noise->attachAxis(y1);
    m_regret->attachAxis(x1);
    m_regret->attachAxis(y0);

    /* 设置图形区域 */
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

void WChart::plot(QList<int>& h, QList<int>& u, QList<int>& l)
{
    int i;
    qreal fc, hr;
    m_upper->clear();
    m_lower->clear();
    for (i = 0; i < u.size(); i++) {
        hr = h.at(i);
        fc = KHZ2MHZ(u.at(i));
        m_upper->append(hr, fc);
    }
    for (i = 0; i < l.size(); i++) {
        hr = h.at(i);
        fc = KHZ2MHZ(l.at(i));
        m_lower->append(hr, fc);
    }
}

void WChart::plot_scan(float hour, float fc, int snr)
{
    Q_UNUSED(snr);
    m_scan->append(hour, fc);
}

void WChart::plot_link(float hour, float fc, int snr)
{
    Q_UNUSED(snr);
    m_link->append(hour, fc);
}

void WChart::plot_regret(float hour, int regret)
{
    /* 调整坐标范围 */
    qreal max = m_axisX1->max();
    if (regret > max) {
        qreal min = m_axisX1->min();
        m_axisX1->setRange(min, max * 1.2f);
    }

    /* 更新数据 */
    m_regret->append(hour, regret);
}

void WChart::plot_avgScan(float hour, float fcNum)
{
    m_fcNum->append(hour, fcNum);
}

void WChart::plot(const int* noise, int n)
{
    int i, j;
    m_noise->clear();
    for (i = j = 0; j < n; i += 15, j++) {
        qreal k = GLB2FREQ(i) / 1000.0;
        m_noise->append(noise[j], k);
    }
}

void WChart::clear_noise(void)
{
    m_noise->clear();
    m_chart->update();
}

void WChart::restart(void)
{
    /* 不要清除噪声 */
    m_regret->clear();
    m_fcNum->clear();
    m_scan->clear();
    m_link->clear();

    /* 重置懊悔范围 */
    m_axisX1->setRange(0, 100);
    m_chart->update();
}
