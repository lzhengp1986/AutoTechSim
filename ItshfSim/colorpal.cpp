#include "colorpal.h"
#include "ui_colorpal.h"

ColorPal::ColorPal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorPal)
{
    ui->setupUi(this);

    /* 扫频点颜色 */
    if (ui->scanBlack->isChecked()) {
        m_scanColor = Qt::black;
    } else if (ui->scanBlue->isChecked()) {
        m_scanColor = Qt::blue;
    } else if (ui->scanCyan->isChecked()) {
        m_scanColor = Qt::cyan;
    } else {
        m_scanColor = Qt::magenta;
    }

    /* 建链业务颜色 */
    if (ui->linkBlack->isChecked()) {
        m_linkColor = Qt::black;
    } else if (ui->linkBlue->isChecked()) {
        m_linkColor = Qt::blue;
    } else if (ui->linkCyan->isChecked()) {
        m_linkColor = Qt::cyan;
    } else {
        m_linkColor = Qt::magenta;
    }

    /* 懊悔值颜色 */
    if (ui->regBlack->isChecked()) {
        m_regColor = Qt::black;
    } else if (ui->regBlue->isChecked()) {
        m_regColor = Qt::blue;
    } else if (ui->regCyan->isChecked()) {
        m_regColor = Qt::cyan;
    } else {
        m_regColor = Qt::magenta;
    }
}

ColorPal::~ColorPal()
{
    delete ui;
}

void ColorPal::on_scanWhite_clicked(void)
{
    m_scanColor = Qt::white;
    emit scan_color(m_scanColor);
}

void ColorPal::on_scanBlack_clicked(void)
{
    m_scanColor = Qt::black;
    emit scan_color(m_scanColor);
}

void ColorPal::on_scanBlue_clicked(void)
{
    m_scanColor = Qt::blue;
    emit scan_color(m_scanColor);
}

void ColorPal::on_scanCyan_clicked(void)
{
    m_scanColor = Qt::cyan;
    emit scan_color(m_scanColor);
}

void ColorPal::on_scanMagenta_clicked(void)
{
    m_scanColor = Qt::magenta;
    emit scan_color(m_scanColor);
}

void ColorPal::on_linkWhite_clicked(void)
{
    m_linkColor = Qt::white;
    emit link_color(m_linkColor);
}

void ColorPal::on_linkBlack_clicked(void)
{
    m_linkColor = Qt::black;
    emit link_color(m_linkColor);
}

void ColorPal::on_linkBlue_clicked(void)
{
    m_linkColor = Qt::blue;
    emit link_color(m_linkColor);
}

void ColorPal::on_linkCyan_clicked(void)
{
    m_linkColor = Qt::cyan;
    emit link_color(m_linkColor);
}

void ColorPal::on_linkMagenta_clicked(void)
{
    m_linkColor = Qt::magenta;
    emit link_color(m_linkColor);
}

void ColorPal::on_regWhite_clicked(void)
{
    m_regColor = Qt::white;
    emit reg_color(m_regColor);
}

void ColorPal::on_regBlack_clicked(void)
{
    m_regColor = Qt::black;
    emit reg_color(m_regColor);
}

void ColorPal::on_regBlue_clicked(void)
{
    m_regColor = Qt::blue;
    emit reg_color(m_regColor);
}

void ColorPal::on_regCyan_clicked(void)
{
    m_regColor = Qt::cyan;
    emit reg_color(m_regColor);
}

void ColorPal::on_regMagenta_clicked(void)
{
    m_regColor = Qt::magenta;
    emit reg_color(m_regColor);
}
