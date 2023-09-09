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
    void setup(const ModelCfg& in);
    int get_year(void);
    int get_month(void);
    int get_dbIndex(void);
    int get_bandIndex(void);

private:
    Ui::Model *ui;
};

#endif // MODEL_H
