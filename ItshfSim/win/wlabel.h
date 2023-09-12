#ifndef QWLABEL_H
#define QWLABEL_H

#include "type.h"
#include <QList>
#include <QLabel>

class WLabel {
public:
    WLabel(void);
    ~WLabel(void);

    /* api */
    QLabel* get_label(int index) const;
    void set_state(int state, int dsec);
    void set_time(const Time* ts);
    void set_channel(int glbChId);
    void set_ratio(int snr);
    void set_noise(int n0);

    enum {VERSION = 0,
          STAMP, STATE, COUNTDOWN,/* 时戳 */
          CHAN_NAME, CHAN_VALUE, /* 信道号 */
          FREQ_NAME, FREQ_VALUE, FREQ_UNIT, /* 频率 */
          SNR_NAME, SNR_VALUE, SNR_UNIT, /* SNR */
          NOISE_NAME, NOISE_VALUE, NOISE_UNIT, /* 噪声 */
          LABLE_NUM};

private:
    QString int2str(int value);
    int glb2freq(int glbChId);

private:
    QList<QLabel*> m_label;
};

#endif // QWLABEL_H
