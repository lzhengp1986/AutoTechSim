#ifndef MODEL_H
#define MODEL_H

#include <QDialog>

namespace Ui {
class Model;
}

class Model : public QDialog
{
    Q_OBJECT

public:
    explicit Model(QWidget *parent = nullptr);
    ~Model();

    /* api */
    void set_list(const QStringList& list);
    int get_index(void);
    int get_month(void);

private:
    Ui::Model *ui;
};

#endif // MODEL_H
