#ifndef DBCREATOR_H
#define DBCREATOR_H

#include <QDialog>

namespace Ui {
class dbCreator;
}

class dbCreator : public QDialog
{
    Q_OBJECT

public:
    explicit dbCreator(QWidget *parent = nullptr);
    ~dbCreator();

private slots:
    void on_brsBtn_clicked(void);
    void on_extBtn_clicked(void);

private:
    enum {HEAD = 0, FREQ, DBU, SNR, REL, SPRB, HOUR_LINES};
    /*! @brief 读出FREQ/DBU/SNR/REL/SPRB行 */
    int read_valid_lines(void);
    /*!
     * @brief 判断content中从id行开始的n行是否为有效的hour结构
     * @param [IN] id hour结构的第1行索引号
     * @return int 返回是否有效
     */
    bool is_valid_hour(int id);
    /*! @brief 转换从id行开始的hour结构 */
    void trans_one_hour(int id);

private:
    Ui::dbCreator *ui;
    QString m_fileName;
    QStringList m_content;
};

#endif // DBCREATOR_H
