#include "cs_app.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator tr;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CSApp_" + QLocale(locale).name();
        if (tr.load(":/i18n/" + baseName)) {
            app.installTranslator(&tr);
            break;
        }
    }
    CS_App cs_app;
    cs_app.show();
    return app.exec();
}
