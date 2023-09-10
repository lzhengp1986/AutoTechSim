#ifndef MODELDLG_H
#define MODELDLG_H

#include <QDialog>

namespace Ui {
class ModelDlg;
}

/* 模型参数 */
typedef struct {
    int year; /* 年 */
    int month; /* 月 */
    int dbIndex; /* DB索引号 */
    int bandIndex; /* 最大可用带宽 */
    QStringList dbDesc; /* DB名称列表 */
} ModelCfg;

class ModelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ModelDlg(QWidget *parent = nullptr);
    ~ModelDlg();

    /* api */
    void dlg2para(ModelCfg* cfg);
    void para2dlg(const ModelCfg* cfg);
    static int get_maxband(int bandIndex);

private:
    Ui::ModelDlg *ui;
};

// inline
inline int ModelDlg::get_maxband(int bandIndex)
{
    int maxband = 0;
    switch (bandIndex) {
    case 1: maxband = 1000; break;
    case 2: maxband = 2000; break;
    case 3: maxband = 4000; break;
    case 4: maxband = 8000; break;
    default: maxband = 30000; break;
    }

    return maxband;
}

#endif // MODELDLG_H
