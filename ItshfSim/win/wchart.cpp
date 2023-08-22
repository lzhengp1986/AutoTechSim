#include "wchart.h"
#include <QValueAxis>

WChart::WChart(void)
{
    /* 添加chart */
    QChart* chart = new QChart();
    m_chart = chart;

    /* 设置x坐标轴 */
    QValueAxis *x = new QValueAxis;
    x->setTitleText("frequency/MHz");
    x->setTitleVisible(false);
    x->setRange(2, 30); /* MHz */
    x->setTickCount(15);
    x->setMinorTickCount(1);
    x->setLabelFormat("%u");
    x->setVisible(false);

    /* 设置y0坐标轴 */
    QValueAxis *y0 = new QValueAxis;
    y0->setTitleText("UT/hour");
    y0->setTitleVisible(false);
    y0->setRange(0, 24); /* hour */
    y0->setTickCount(13);
    y0->setMinorTickCount(1);
    y0->setLabelFormat("%u");
    y0->setVisible(false);

    /* 添加坐标轴 */
    chart->addAxis(x, Qt::AlignLeft);
    chart->addAxis(y0, Qt::AlignBottom);

    /* 设置图形区域 */
    QRect rect(47, 64, 544, 409);
    chart->setPlotArea(rect);

    /* 设置chartview透明 */
    chart->setBackgroundVisible(false);
}

WChart::~WChart(void)
{
    delete m_chart;
}

QChart* WChart::get_chart(void) const
{
    return m_chart;
}
