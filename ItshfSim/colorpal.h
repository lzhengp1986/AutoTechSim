#ifndef COLORPAL_H
#define COLORPAL_H

#include <QWidget>

namespace Ui {
class ColorPal;
}

class ColorPal : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPal(QWidget *parent = nullptr);
    ~ColorPal();

private slots:
    void on_scanBlack_clicked(void);
    void on_scanBlue_clicked(void);
    void on_scanCyan_clicked(void);
    void on_scanMagenta_clicked(void);
    void on_linkBlack_clicked(void);
    void on_linkBlue_clicked(void);
    void on_linkCyan_clicked(void);
    void on_linkMagenta_clicked(void);
    void on_regBlack_clicked(void);
    void on_regBlue_clicked(void);
    void on_regCyan_clicked(void);
    void on_regMagenta_clicked(void);

signals:
    void scan_color(Qt::GlobalColor color);
    void link_color(Qt::GlobalColor color);
    void reg_color(Qt::GlobalColor color);

public:
    Qt::GlobalColor m_scanColor; /* 扫频点颜色 */
    Qt::GlobalColor m_linkColor; /* 建链点颜色 */
    Qt::GlobalColor m_regColor; /* 懊悔值颜色 */

private:
    Ui::ColorPal *ui;
};

#endif // COLORPAL_H
