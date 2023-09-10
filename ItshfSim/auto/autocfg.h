#ifndef AUTOCFG_H
#define AUTOCFG_H

#include <QDialog>

namespace Ui {
class AutoCfg;
}

class AutoCfg : public QDialog
{
    Q_OBJECT

public:
    explicit AutoCfg(QWidget *parent = nullptr);
    ~AutoCfg();

private:
    Ui::AutoCfg *ui;
};

#endif // AUTOCFG_H
