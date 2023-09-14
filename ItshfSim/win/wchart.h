#ifndef QWCHART_H
#define QWCHART_H

#include <qchart.h>
#include <QScatterSeries>
using namespace QtCharts;

class WChart {
public:
    WChart(void);
    ~WChart(void);

    /* api */
    void set_scan_color(QColor color);
    void set_link_color(QColor color);
    QChart* get_chart(void) const;
    void plot(float hour, float fc, int snr);
    void clear(void);

private:
    QChart* m_chart;
    QScatterSeries* m_scan;
    QScatterSeries* m_link;
};

#endif // QWCHART_H
