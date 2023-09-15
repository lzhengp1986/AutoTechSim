#include "modeldlg.h"
#include "ui_modeldlg.h"

ModelCfg::ModelCfg(void)
{
    month = 1;
    year = 2023;
    dbDesc << "成都市区-乐山沐川"
           << "成都市区-陕西西安"
           << "成都市区-海南三亚";

    dbIndex = 0;
    bandIndex = 0;
    withNoise = false;
}

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

void ModelDlg::para2dlg(const ModelCfg* cfg)
{
    int n = cfg->dbDesc.size();
    for (int i = 0; i < n; i++) {
        ui->list->addItem(cfg->dbDesc.at(i));
    }

    ui->year->setValue(cfg->year);
    ui->month->setValue(cfg->month);
    ui->list->setCurrentIndex(cfg->dbIndex);
    ui->bandBox->setCurrentIndex(cfg->bandIndex);
    ui->noiseChk->setChecked(cfg->withNoise);
}

void ModelDlg::dlg2para(ModelCfg *cfg)
{
    cfg->year = ui->year->value();
    cfg->month = ui->month->value();
    cfg->dbIndex = ui->list->currentIndex();
    cfg->bandIndex = ui->bandBox->currentIndex();
    cfg->withNoise = ui->noiseChk->isChecked();
}
