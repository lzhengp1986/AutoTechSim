#ifndef DBCREATOR_H
#define DBCREATOR_H

#include "sqlite3.h"
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
    enum {FREQ = 0, DBU, SNR, REL, SPRB, HOUR_LINES};
    /*! @brief 读出FREQ/DBU/SNR/REL/SPRB行 */
    int read_valid_lines(void);

    /*!
     * @brief 判断content中从i行开始的n行是否为有效的hour结构
     * @param [IN] i hour结构的第1行索引号
     * @return int 返回是否有效
     */
    bool is_valid_hour(int i);

    /*! @brief 转换从i行开始的hour结构 */
    int trans_one_hour(int i,
                       QList<int>& freq,
                       QList<int>& dbu,
                       QList<int>& snr,
                       QList<int>& rel,
                       QList<int>& sprb);

    /*! @brief 转换第i行的head结构 */
    void trans_head(int i, int& year, int& month, int& ssn);
    /*! @brief 转换第i行的data结构 */
    void trans_data(int i, QList<int>& list);
    /*! @brief 转换第i行的freq结构 */
    void trans_freq(int i, int& hour, QList<int>& list);
    /*! @brief 将line按字符拆分 */
    void split(const QString& line, QList<int>& list);
    void split(const QString& line, QList<float>& list);

private:
    void db_open(sqlite3** db);
    void db_insert(sqlite3* db, int year, int month, int ssn, int freq, int dbu, int snr, int rel, int sprb);
    void db_close(sqlite3* db);

private:
    Ui::dbCreator *ui;
    QString m_fileName;
    QStringList m_content;
};

#endif // DBCREATOR_H
