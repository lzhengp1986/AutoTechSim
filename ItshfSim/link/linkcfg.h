#ifndef LINKCFG_H
#define LINKCFG_H

#include <QDialog>

namespace Ui {
class LinkCfg;
}

class LinkCfg : public QDialog
{
    Q_OBJECT

public:
    explicit LinkCfg(QWidget *parent = nullptr);
    ~LinkCfg();

private:
    Ui::LinkCfg *ui;
};

#endif // LINKCFG_H
