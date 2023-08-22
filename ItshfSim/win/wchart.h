#ifndef QWCHART_H
#define QWCHART_H

#include <qchart.h>
using namespace QtCharts;

class WChart {
public:
    WChart(void);
    ~WChart(void);

    /* api */
    QChart* get_chart(void) const;

private:
    QChart* m_chart;
};

#endif // QWCHART_H
