#ifndef LINKWIN_H
#define LINKWIN_H

#include <QDialog>

namespace Ui {
class LinkWin;
}

typedef struct {
    int speed; /* ���涨ʱ�ٶ� */
    int reqPtn; /* �����������ͣ����ڻ���� */
} LinkCfgPara;

class LinkWin : public QDialog
{
    Q_OBJECT

public:
    explicit LinkWin(QWidget *parent = nullptr);
    ~LinkWin();

    /* api */

private:
    Ui::LinkWin *ui;
};

#endif // LINKWIN_H
