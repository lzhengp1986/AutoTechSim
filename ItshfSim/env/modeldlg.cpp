#include "modeldlg.h"
#include "ui_modeldlg.h"

ModelDlg::ModelDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelDlg)
{
    ui->setupUi(this);
}

ModelDlg::~ModelDlg()
{
    delete ui;
}

void ModelDlg::para2win(const ModelCfg* cfg)
{
    int n = cfg->dbDesc.size();
    for (int i = 0; i < n; i++) {
        ui->list->addItem(cfg->dbDesc.at(i));
    }

    ui->year->setValue(cfg->year);
    ui->month->setValue(cfg->month);
    ui->list->setCurrentIndex(cfg->dbIndex);
    ui->bandBox->setCurrentIndex(cfg->bandIndex);
}

void ModelDlg::win2para(ModelCfg *cfg)
{
    cfg->year = ui->year->value();
    cfg->month = ui->month->value();
    cfg->dbIndex = ui->list->currentIndex();
    cfg->bandIndex = ui->bandBox->currentIndex();
}
