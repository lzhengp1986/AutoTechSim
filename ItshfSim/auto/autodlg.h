#ifndef AUTODLG_H
#define AUTODLG_H

#include <QDialog>

namespace Ui {
class AutoDlg;
}

class AutoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AutoDlg(QWidget *parent = nullptr);
    ~AutoDlg();

private:
    Ui::AutoDlg *ui;
};

#endif // AUTODLG_H
