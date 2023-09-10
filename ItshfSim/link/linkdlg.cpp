#include "linkdlg.h"
#include "ui_linkdlg.h"

LinkDlg::LinkDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkDlg)
{
    ui->setupUi(this);
}

LinkDlg::~LinkDlg()
{
    delete ui;
}

void LinkDlg::dlg2para(LinkCfg* cfg)
{
    if (ui->tmr1x->isChecked()) {
        cfg->speed = 1;
    } else if (ui->tmr8x->isChecked()) {
        cfg->speed = 8;
    } else if (ui->tmr16x->isChecked()) {
        cfg->speed = 16;
    } else if (ui->tmr32x->isChecked()) {
        cfg->speed = 32;
    } else if (ui->tmr64x->isChecked()) {
        cfg->speed = 64;
    } else {
        cfg->speed = 64;
    }

    if (ui->ptn2m->isChecked()) {
        cfg->reqPtn = 2;
    } else if (ui->ptn5m->isChecked()) {
        cfg->reqPtn = 5;
    } else if (ui->ptn10m->isChecked()) {
        cfg->reqPtn = 10;
    } else if (ui->ptn20m->isChecked()) {
        cfg->reqPtn = 20;
    } else if (ui->ptnRnd->isChecked()) {
        cfg->reqPtn = 0;
    } else {
        cfg->reqPtn = 0;
    }
}

void LinkDlg::para2dlg(const LinkCfg* cfg)
{
    switch (cfg->speed) {
    case 1: ui->tmr1x->setChecked(true); break;
    case 8: ui->tmr8x->setChecked(true); break;
    case 16: ui->tmr16x->setChecked(true); break;
    case 32: ui->tmr32x->setChecked(true); break;
    case 64: ui->tmr64x->setChecked(true); break;
    default: ui->tmr64x->setChecked(true); break;
    }

    switch (cfg->reqPtn) {
    case 0: ui->ptnRnd->setChecked(true); break;
    case 2: ui->ptn2m->setChecked(true); break;
    case 5: ui->ptn5m->setChecked(true); break;
    case 10: ui->ptn10m->setChecked(true); break;
    case 20: ui->ptn20m->setChecked(true); break;
    default: ui->ptnRnd->setChecked(true); break;
    }
}
