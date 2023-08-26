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

void Model::setup(int index, int month, const QStringList& list)
{
    int n = list.size();
    for (int i = 0; i < n; i++) {
        ui->list->addItem(list.at(i));
    }

    ui->list->setCurrentIndex(index);
    ui->month->setValue(month);
}

int Model::get_index(void)
{
    return ui->list->currentIndex();
}

int Model::get_month(void)
{
    return ui->month->value();
}
