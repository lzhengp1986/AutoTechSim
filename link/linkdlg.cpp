#include "linkdlg.h"
#include "ui_linkdlg.h"

LinkCfg::LinkCfg(void)
{
    simDayIndex = 0; /* 1 */
    freqNumIndex = 0; /* 10 */
    tmrSpeedIndex = 0; /* x128 */
    scanIntvIndex = 0; /* 2sec */
    svcIntvIndex = 0; /* random */
    idleIntvIndex = 0; /* random */
    sqlRuleIndex = 0; /* 4hour */
    algIndex = 0; /* random */
}

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

void LinkDlg::dlg2para(LinkCfg* cfg, QString& algName, QString& sqlRule)
{
    cfg->simDayIndex = ui->simDay->currentIndex();
    cfg->tmrSpeedIndex = ui->tmrSpeed->currentIndex();
    cfg->freqNumIndex = ui->reqFcNum->currentIndex();
    cfg->scanIntvIndex = ui->scanInterval->currentIndex();
    cfg->svcIntvIndex = ui->linkInterval->currentIndex();
    cfg->idleIntvIndex = ui->idleInterval->currentIndex();
    cfg->sqlRuleIndex = ui->sqlRule->currentIndex();
    cfg->algIndex = ui->alg->currentIndex();
    algName = ui->alg->currentText();
    sqlRule = ui->sqlRule->currentText();
}

void LinkDlg::para2dlg(const LinkCfg* cfg)
{
    ui->simDay->setCurrentIndex(cfg->simDayIndex);
    ui->tmrSpeed->setCurrentIndex(cfg->tmrSpeedIndex);
    ui->reqFcNum->setCurrentIndex(cfg->freqNumIndex);
    ui->scanInterval->setCurrentIndex(cfg->scanIntvIndex);
    ui->linkInterval->setCurrentIndex(cfg->svcIntvIndex);
    ui->idleInterval->setCurrentIndex(cfg->idleIntvIndex);
    ui->sqlRule->setCurrentIndex(cfg->sqlRuleIndex);
    ui->alg->setCurrentIndex(cfg->algIndex);
}
