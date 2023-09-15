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
    cfg->simDayIndex = ui->simDay->currentIndex();
    cfg->tmrSpeedIndex = ui->tmrSpeed->currentIndex();
    cfg->fcNumIndex = ui->reqFcNum->currentIndex();
    cfg->scanIntvIndex = ui->scanInterval->currentIndex();
    cfg->svcIntvIndex = ui->linkInterval->currentIndex();
    cfg->idleIntvIndex = ui->idleInterval->currentIndex();
    cfg->algIndex = ui->alg->currentIndex();
    cfg->fromIndex = ui->fromFreq->currentIndex();
    cfg->bwIndex = ui->bwLen->currentIndex();
}

void LinkDlg::para2dlg(const LinkCfg* cfg)
{
    ui->simDay->setCurrentIndex(cfg->simDayIndex);
    ui->tmrSpeed->setCurrentIndex(cfg->tmrSpeedIndex);
    ui->reqFcNum->setCurrentIndex(cfg->fcNumIndex);
    ui->scanInterval->setCurrentIndex(cfg->scanIntvIndex);
    ui->linkInterval->setCurrentIndex(cfg->svcIntvIndex);
    ui->idleInterval->setCurrentIndex(cfg->idleIntvIndex);
    ui->alg->setCurrentIndex(cfg->algIndex);
    ui->fromFreq->setCurrentIndex(cfg->fromIndex);
    ui->bwLen->setCurrentIndex(cfg->bwIndex);
}
