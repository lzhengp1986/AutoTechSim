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

void Model::setup(const ModelCfg& in)
{
    int n = in.dbDesc.size();
    for (int i = 0; i < n; i++) {
        ui->list->addItem(in.dbDesc.at(i));
    }

    ui->year->setValue(in.year);
    ui->month->setValue(in.month);
    ui->list->setCurrentIndex(in.dbIndex);
    ui->bandBox->setCurrentIndex(in.bandIndex);
}

int Model::get_dbIndex(void)
{
    return ui->list->currentIndex();
}

int Model::get_month(void)
{
    return ui->month->value();
}

int Model::get_year(void)
{
    return ui->year->value();
}

int Model::get_bandIndex(void)
{
    return ui->bandBox->currentIndex();
}
