#include "linkreq.h"
#include "ui_linkreq.h"

LinkReq::LinkReq(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkReq)
{
    ui->setupUi(this);
}

LinkReq::~LinkReq()
{
    delete ui;
}
