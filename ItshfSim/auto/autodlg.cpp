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
