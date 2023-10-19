#include "mainwin.h"

#include <QDate>
#include <QLocale>
#include <QTranslator>
#include <QApplication>

/* 时间界限 */
#define YEAR_LIMIT 1

int main(int argc, char *argv[])
{
    /* 检查时间 */
    QString compile = __DATE__;
    QString year = compile.split(' ').last();
    QDate date = QDate::currentDate();
    int cmpYear = year.toUInt();
    int curYear = date.year();
    if (curYear > cmpYear + YEAR_LIMIT) {
        return curYear;
    }

    /* 运行程序 */
    QTranslator translator;
    QApplication a(argc, argv);
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AutoTechSim_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWin w;
    w.show();
    return a.exec();
}
