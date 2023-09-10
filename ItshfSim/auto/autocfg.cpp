#include "autocfg.h"
#include "ui_autocfg.h"

AutoCfg::AutoCfg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoCfg)
{
    ui->setupUi(this);
}

AutoCfg::~AutoCfg()
{
    delete ui;
}
