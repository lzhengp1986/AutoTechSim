#ifndef MODELDLG_H
#define MODELDLG_H

#include <QDialog>

namespace Ui {
class ModelDlg;
}

/* 模型参数 */
class ModelCfg {
public:
    ModelCfg(void);
    int get_maxband(void) const;

public:
    int year; /* 年 */
    int month; /* 月 */
    int dbIndex; /* DB索引号 */
    int bandIndex; /* 最大可用带宽 */
    bool withNoise; /* 带底噪 */
    QString dbDesc; /* DB名称 */
};

// inline
inline int ModelCfg::get_maxband(void) const
{
    int maxband = 0;
    switch (bandIndex) {
    case 0: maxband = 30000; break;
    case 1: maxband = 16000; break;
    case 2: maxband = 8000; break;
    case 3: maxband = 4000; break;
    default: maxband = 30000; break;
    }

    return maxband;
}

class ModelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ModelDlg(QWidget *parent = nullptr);
    ~ModelDlg();

    /* api */
    void dlg2para(ModelCfg* cfg);
    void para2dlg(const ModelCfg* cfg);

private:
    Ui::ModelDlg *ui;
};

#endif // MODELDLG_H
