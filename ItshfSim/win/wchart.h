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
    QChart* get_chart(void) const;
    void plot(float hour, float fc, int snr);

private:
    QChart* m_chart;
    QScatterSeries* m_scan;
    QScatterSeries* m_link;
};

#endif // QWCHART_H
