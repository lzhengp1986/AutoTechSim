#ifndef QWCHART_H
#define QWCHART_H

#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>

class WChart {
public:
    WChart(void);
    ~WChart(void);

    /* api */
    QChart* get_chart(void) const;
    void set_scan_color(QColor color);
    void set_link_color(QColor color);

    /* plot */
    void plot(QList<int>& h, QList<int>& u, QList<int>& l);
    void plot_scan(float hour, float fc, int snr);
    void plot_link(float hour, float fc, int snr);
    void plot_regret(float hour, int regret);
    void plot_avgScan(float hour, float fcNum);
    void plot(const int* noise, int n);

    /* clear */
    void clear_noise(void);
    void restart(void);

private:
    QChart *m_chart;
    QValueAxis *m_axisX1;
    QSplineSeries *m_upper;
    QSplineSeries *m_lower;

    /* 数据点 */
    QLineSeries *m_fcNum;
    QLineSeries *m_noise;
    QLineSeries *m_regret;
    QScatterSeries *m_scan;
    QScatterSeries *m_link;
};

#endif // QWCHART_H
