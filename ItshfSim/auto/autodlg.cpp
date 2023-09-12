#include "autodlg.h"
#include "ui_autodlg.h"

AutoDlg::AutoDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoDlg)
{
    ui->setupUi(this);
}

AutoDlg::~AutoDlg()
{
    delete ui;
}

void AutoDlg::dlg2para(AutoCfg* cfg)
{
}

void AutoDlg::para2dlg(const AutoCfg* cfg)
{
}

