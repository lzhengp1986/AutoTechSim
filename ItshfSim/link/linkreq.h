#ifndef LINKREQ_H
#define LINKREQ_H

#include <QDialog>

namespace Ui {
class LinkReq;
}

class LinkReq : public QDialog
{
    Q_OBJECT

public:
    explicit LinkReq(QWidget *parent = nullptr);
    ~LinkReq();

private:
    Ui::LinkReq *ui;
};

#endif // LINKREQ_H
