#ifndef QWCHART_H
#define QWCHART_H

#include <qchart.h>
#include <QValueAxis>
#include <QLineSeries>
#include <QScatterSeries>
using namespace QtCharts;

class WChart {
public:
    WChart(void);
    ~WChart(void);

    /* api */
    QChart* get_chart(void) const;
    void set_scan_color(QColor color);
    void set_link_color(QColor color);
    void set_regret_color(QColor color);
    void set_fcNum_color(QColor color);

    /* plot */
    void plot_scan(float hour, float fc, int snr);
    void plot_link(float hour, float fc, int snr);
    void plot_regret(float hour, int regret);
    void plot_avgScan(float hour, int fcNum);
    void plot(const int* noise, int n);

    /* clear */
    void clear_noise(void);
    void restart(void);

private:
    QChart *m_chart;
    QValueAxis *m_axisX1;

    /* 数据点 */
    QLineSeries *m_fcNum;
    QLineSeries *m_noise;
    QLineSeries *m_regret;
    QScatterSeries *m_scan;
    QScatterSeries *m_link;
};

#endif // QWCHART_H
