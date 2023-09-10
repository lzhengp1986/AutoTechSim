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
