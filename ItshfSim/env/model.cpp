#include "model.h"
#include "ui_model.h"

Model::Model(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Model)
{
    ui->setupUi(this);
}

Model::~Model()
{
    delete ui;
}

void Model::para2win(const ModelCfg* cfg)
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

void Model::win2para(ModelCfg *cfg)
{
    cfg->year = ui->year->value();
    cfg->month = ui->month->value();
    cfg->dbIndex = ui->list->currentIndex();
    cfg->bandIndex = ui->bandBox->currentIndex();
}
