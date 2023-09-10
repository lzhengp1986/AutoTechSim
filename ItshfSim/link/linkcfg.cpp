#include "linkcfg.h"
#include "ui_linkcfg.h"

LinkCfg::LinkCfg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkCfg)
{
    ui->setupUi(this);
}

LinkCfg::~LinkCfg()
{
    delete ui;
}
