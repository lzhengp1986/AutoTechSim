#ifndef AUTOWIN_H
#define AUTOWIN_H

#include <QDialog>

namespace Ui {
class AutoWin;
}

class AutoWin : public QDialog
{
    Q_OBJECT

public:
    explicit AutoWin(QWidget *parent = nullptr);
    ~AutoWin();

private:
    Ui::AutoWin *ui;
};

#endif // AUTOWIN_H
