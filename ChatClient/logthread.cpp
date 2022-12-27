#include "logthread.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

LogThread::LogThread(QObject *parent)                                   //생성자
    : QThread{parent}, filename(""), format("")
{
    format = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");  //년월일_시분 형식의 파일 생성 시간 저장
    filename = QString("log_%1.txt").arg(format);                       //파일 이름을 설정
}

void LogThread::run()                                                   //로그를 저장하는 함수
{
    Q_FOREVER
    {
        saveData();
        sleep(1);                                                      // 1분마다 저장
    }
}

void LogThread::appendData(QTreeWidgetItem* item)                       //저장할 로그를 배열에 추가하는 함수
{
    itemList.append(item);                                              //배열에 아이템을 저장
}

void LogThread::saveData()                                              //로그를 파일로 출력하는 함수
{
    QString id, name;                                                   //id와 이름을 저장할 변수
    if(itemList.count() > 0)                                            //저장할 로그가 있으면
    {
        QFile file(filename);                                           //파일 생성
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))         //파일 열기 예외처리
            return;

        QTextStream out(&file);
        foreach(auto item, itemList)                                    //저장된 아이템의 수만큼 반복
        {
            /*id와 이름 저장*/
            id = item->text(2);
            name = item->text(3);

            out << item->text(0) << ", ";                               //", "를 기준으로 분리
            out << item->text(1) << ", ";
            out << item->text(2) << ", ";
            out << item->text(3) << ", ";
            out << item->text(4) << ", ";
            out << item->text(5) << "\n";
        }
        file.close();                                                   //파일 출력 종료

        /*파일 이름을 log_id_고객성명_시간.txt로 설정 후 저장*/
        file.rename(QString("log_%1_%2_%3.txt").arg(id).arg(name).
                    arg(format));
        filename = QString("log_%1_%2_%3.txt").arg(id).arg(name).
                    arg(format);

    }
}
