#include "linkwin.h"
#include "ui_linkwin.h"

LinkWin::LinkWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkWin)
{
    ui->setupUi(this);
}

LinkWin::~LinkWin()
{
    delete ui;
}
