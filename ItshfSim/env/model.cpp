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

void Model::setup(int year, int month, const QStringList& list, int dbIndex, int bandIndex)
{
    int n = list.size();
    for (int i = 0; i < n; i++) {
        ui->list->addItem(list.at(i));
    }

    ui->year->setValue(year);
    ui->month->setValue(month);
    ui->list->setCurrentIndex(dbIndex);
    ui->bandBox->setCurrentIndex(bandIndex);
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
