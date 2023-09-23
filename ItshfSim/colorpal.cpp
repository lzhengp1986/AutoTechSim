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
