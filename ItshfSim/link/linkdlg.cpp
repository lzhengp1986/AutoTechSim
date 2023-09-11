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
    cfg->tmrSpeedIndex = ui->tmrSpeed->currentIndex();
    cfg->fcNumIndex = ui->reqFcNum->currentIndex();
    cfg->scanIntvIndex = ui->scanInterval->currentIndex();
    cfg->svcIntvIndex = ui->svcInterval->currentIndex();
}

void LinkDlg::para2dlg(const LinkCfg* cfg)
{
    ui->tmrSpeed->setCurrentIndex(cfg->tmrSpeedIndex);
    ui->reqFcNum->setCurrentIndex(cfg->fcNumIndex);
    ui->scanInterval->setCurrentIndex(cfg->scanIntvIndex);
    ui->svcInterval->setCurrentIndex(cfg->svcIntvIndex);
}
