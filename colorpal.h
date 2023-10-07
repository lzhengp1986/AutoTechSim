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
    void on_scanWhite_clicked(void);
    void on_scanBlack_clicked(void);
    void on_scanBlue_clicked(void);
    void on_scanCyan_clicked(void);
    void on_scanMagenta_clicked(void);
    void on_scanRed_clicked(void);
    void on_linkWhite_clicked(void);
    void on_linkBlack_clicked(void);
    void on_linkBlue_clicked(void);
    void on_linkCyan_clicked(void);
    void on_linkRed_clicked(void);
    void on_linkMagenta_clicked(void);

signals:
    void scan_color(Qt::GlobalColor color);
    void link_color(Qt::GlobalColor color);

private:
    Ui::ColorPal *ui;
};

#endif // COLORPAL_H
