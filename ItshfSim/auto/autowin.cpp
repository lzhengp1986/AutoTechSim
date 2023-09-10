#include "autowin.h"
#include "ui_autowin.h"

AutoWin::AutoWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoWin)
{
    ui->setupUi(this);
}

AutoWin::~AutoWin()
{
    delete ui;
}
