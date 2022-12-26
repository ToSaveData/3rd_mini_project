#include "chatclient.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator tr;
    tr.load("ChatClient_ko");
    app.installTranslator(&tr);

    ChatClient w;
    w.show();
    return app.exec();
}
