#include "colorpal.h"
#include "ui_colorpal.h"

ColorPal::ColorPal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorPal)
{
    ui->setupUi(this);
}

ColorPal::~ColorPal()
{
    delete ui;
}

void ColorPal::on_scanWhite_clicked(void)
{
    emit scan_color(Qt::white);
}

void ColorPal::on_scanBlack_clicked(void)
{
    emit scan_color(Qt::black);
}

void ColorPal::on_scanBlue_clicked(void)
{
    emit scan_color(Qt::blue);
}

void ColorPal::on_scanCyan_clicked(void)
{
    emit scan_color(Qt::cyan);
}

void ColorPal::on_scanMagenta_clicked(void)
{
    emit scan_color(Qt::magenta);
}

void ColorPal::on_scanRed_clicked(void)
{
    emit scan_color(Qt::red);
}

void ColorPal::on_linkWhite_clicked(void)
{
    emit link_color(Qt::white);
}

void ColorPal::on_linkBlack_clicked(void)
{
    emit link_color(Qt::black);
}

void ColorPal::on_linkBlue_clicked(void)
{
    emit link_color(Qt::blue);
}

void ColorPal::on_linkCyan_clicked(void)
{
    emit link_color(Qt::cyan);
}

void ColorPal::on_linkMagenta_clicked(void)
{
    emit link_color(Qt::magenta);
}

void ColorPal::on_linkRed_clicked(void)
{
    emit link_color(Qt::red);
}

void ColorPal::on_regWhite_clicked(void)
{
    emit reg_color(Qt::white);
}

void ColorPal::on_regBlack_clicked(void)
{
    emit reg_color(Qt::black);
}

void ColorPal::on_regBlue_clicked(void)
{
    emit reg_color(Qt::blue);
}

void ColorPal::on_regCyan_clicked(void)
{
    emit reg_color(Qt::cyan);
}

void ColorPal::on_regMagenta_clicked(void)
{
    emit reg_color(Qt::magenta);
}

void ColorPal::on_regRed_clicked(void)
{
    emit reg_color(Qt::red);
}

void ColorPal::on_avgWhite_clicked(void)
{
    emit fnum_color(Qt::white);
}

void ColorPal::on_avgBlack_clicked(void)
{
    emit fnum_color(Qt::black);
}

void ColorPal::on_avgBlue_clicked(void)
{
    emit fnum_color(Qt::blue);
}

void ColorPal::on_avgCyan_clicked(void)
{
    emit fnum_color(Qt::cyan);
}

void ColorPal::on_avgMagenta_clicked(void)
{
    emit fnum_color(Qt::magenta);
}

void ColorPal::on_avgRed_clicked(void)
{
    emit fnum_color(Qt::red);
}
