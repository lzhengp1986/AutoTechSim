#ifndef MODEL_H
#define MODEL_H

#include <QDialog>

namespace Ui {
class Model;
}

/* 模型参数 */
typedef struct {
    int year; /* 年 */
    int month; /* 月 */
    int dbIndex; /* DB索引号 */
    int bandIndex; /* 最大可用带宽 */
    QStringList dbDesc; /* DB名称列表 */
} ModelCfg;

class Model : public QDialog
{
    Q_OBJECT

public:
    explicit Model(QWidget *parent = nullptr);
    ~Model();

    /* api */
    void setup(const ModelCfg* cfg);
    int get_year(void);
    int get_month(void);
    int get_dbIndex(void);
    int get_bandIndex(void);
    static int get_maxband(int bandIndex);

private:
    Ui::Model *ui;
};

// inline
inline int Model::get_maxband(int bandIndex)
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

#endif // MODEL_H
