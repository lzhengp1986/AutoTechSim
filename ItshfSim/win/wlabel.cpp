#include "wlabel.h"
#include <QDateTime>

WLabel::WLabel(void)
{
    m_label.clear();
    for (int i = 0; i < LABLE_NUM; i++) {
        QLabel *label = new QLabel();
        m_label.append(label);
    }

    /* 版本 */
    m_label.at(VERSION)->setText("version1.0");
    m_label.at(VERSION)->setMinimumWidth(70);

    /* 时戳 */
    QDateTime ts = QDateTime::currentDateTime();
    QString stamp = ts.toString("yyyy-MM-dd hh:mm:ss");
    m_label.at(STAMP)->setMinimumWidth(120);
    m_label.at(STAMP)->setText(stamp);

    /* 状态 */
    m_label.at(STATE)->setMinimumWidth(60);
    m_label.at(STATE)->setText("WAIT");

    /* 信道 */
    m_label.at(CHAN_NAME)->setText("glbChId");
    m_label.at(CHAN_VALUE)->setMinimumWidth(30);
    m_label.at(FREQ_NAME)->setText("frequency");
    m_label.at(FREQ_VALUE)->setMinimumWidth(30);
    m_label.at(FREQ_UNIT)->setText("KHz");
    set_channel(0);

    /* SNR */
    m_label.at(SNR_NAME)->setText("SNR");
    m_label.at(SNR_VALUE)->setMinimumWidth(30);
    m_label.at(SNR_UNIT)->setText("dB");
    set_ratio(-10);

    /* Noise */
    m_label.at(NOISE_NAME)->setText("Noise");
    m_label.at(NOISE_VALUE)->setMinimumWidth(30);
    m_label.at(NOISE_UNIT)->setText("dBm");
    set_noise(-120);
}

WLabel::~WLabel(void)
{
    for (int i = 0; i < LABLE_NUM; i++) {
        delete m_label.at(i);
    }
    m_label.clear();
}

/* 获取label */
QLabel* WLabel::get_label(int index) const
{
    return m_label.at(index);
}

void WLabel::set_time(const Time* ts)
{
    QString stamp = QString("%1-%2-%3 %4:%5:%6")
            .arg(ts->year, 2, 10, QLatin1Char('0'))
            .arg(ts->month, 2, 10, QLatin1Char('0'))
            .arg(ts->day, 2, 10, QLatin1Char('0'))
            .arg(ts->hour, 2, 10, QLatin1Char('0'))
            .arg(ts->min, 2, 10, QLatin1Char('0'))
            .arg(ts->sec, 2, 10, QLatin1Char('0'));
    m_label.at(STAMP)->setText(stamp);
}

void WLabel::set_state(int state, int dsec)
{
    /* 状态 */
    QString text;
    switch (state) {
    case IDLE: text = "IDLE"; break;
    case SCAN: text = "SCAN"; break;
    case LINK: text = "LINK"; break;
    default: text = "WAIT"; break;
    }

    if (state != WAIT) {
        /* 倒计时 */
        QString ts;
        int asec = ABS(dsec);
        if (asec > 24 * 3600) {
            ts = "inf";
        } else if (asec > 3600) {
            ts = QString::number(asec / 3600) + "h";
        } else if (asec > 60) {
            ts = QString::number(asec / 60) + "m";
        } else {
            ts = QString::number(asec) + "s";
        }

        m_label.at(STATE)->setText(text + ":" + ts);
    } else {
        m_label.at(STATE)->setText(text);
    }
}

void WLabel::set_channel(int glbChId)
{
    int fc = glb2freq(glbChId);
    m_label.at(FREQ_VALUE)->setText(int2str(fc));
    m_label.at(CHAN_VALUE)->setText(int2str(glbChId));
}

void WLabel::set_ratio(int snr)
{
    m_label.at(SNR_VALUE)->setText(int2str(snr));
}

void WLabel::set_noise(int n0)
{
    m_label.at(NOISE_VALUE)->setText(int2str(n0));
}

int WLabel::glb2freq(int glbChId)
{
    return (glbChId * 3 + 2000);
}

QString WLabel::int2str(int value)
{
    return QString::number(value);
}
