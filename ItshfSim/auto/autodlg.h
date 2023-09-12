#ifndef AUTODLG_H
#define AUTODLG_H

#include <QDialog>

namespace Ui {
class AutoDlg;
}

/* 策略配置 */
typedef struct {
    int algId;
} AutoCfg;

class AutoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AutoDlg(QWidget *parent = nullptr);
    ~AutoDlg();

    /* api */
    void dlg2para(AutoCfg* cfg);
    void para2dlg(const AutoCfg* cfg);

private:
    Ui::AutoDlg *ui;
};

#endif // AUTODLG_H
