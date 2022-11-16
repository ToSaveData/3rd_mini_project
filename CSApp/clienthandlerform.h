#ifndef CLIENTHANDLERFORM_H
#define CLIENTHANDLERFORM_H

#include <QWidget>

namespace Ui {
class ClientHandlerForm;
}

class QComboBox;
class QSqlQuery;
class QSqlTableModel;

class ClientHandlerForm : public QWidget                        //고객 정보를 활용하는 클래스
{
    Q_OBJECT

public:
    explicit ClientHandlerForm(QWidget *parent = nullptr);      //생성자
    ~ClientHandlerForm();                                       //소멸자

public slots:
    void dataLoad();                                            //파일 입력용 함수

signals:
    /*파일 입력 시 고객 정보를 서버에 전달하기 위한 시그널*/
    void clientLoad(QList<int>, QList<QString>);

    /*고객 정보가 추가됐음을 주문 정보 클래스에 전달하는 시그널*/
    void clientAdded(int);

    /*고객 정보가 삭제됐음을 주문 정보 클래스에 전달하는 시그널*/
    void clientRemoved(int);

    /*고객 정보가 수정됐음을 주문 정보 클래스에 전달하는 시그널*/
    void clientModified(int, QList<QString>);

    /*주문 정보 등록 시 요청한 고객 정보를 인자로 담아 반환하는 시그널*/
    void addReturn(QList<QString>);

    /*주문 정보 검색 시 요청한 고객 정보를 인자로 담아 반환하는 시그널*/
    void searchReturn(QList<QString>);

    /*주문 정보 수정 시 요청한 고객 정보를 인자로 담아 반환하는 시그널*/
    void modifyReturn(QList<QString>, int);

    /*새로 등록된 고객 정보를 서버에 전달하는 시그널*/
    void sendServer(QList<int>, QList<QString>);

    /*삭제된 고객 정보를 서버에 전달하는 시그널*/
    void sendServerCRemoved(QString);

    /*수정된 고객 정보를 서버에 전달하는 시그널*/
    void sendServerCModified(int, QString);

private:
    Ui::ClientHandlerForm *Cui;                                 //UI
    int makeCid();                                              //고객 ID를 생성하는 함수
    QSqlTableModel *tableModel;                                 //고객 정보를 나타낼 테이블 모델
    QSqlTableModel *searchModel;                                //검색 결과를 나타날 테이블 모델
    QSqlQuery *query;                                           //쿼리문을 사용하기 위한 쿼리

private slots:
    void on_enrollPushButton_clicked();                         //등록 버튼을 눌렀을 때
    void on_searchPushButton_clicked();                         //검색 버튼을 눌렀을 때
    void on_resetPushButton_clicked();                          //검색 초기화 버튼을 눌렀을 때
    void on_removePushButton_clicked();                         //삭제 버튼을 눌렀을 때
    void on_modifyPushButton_clicked();                         //수정 버튼을 눌렀을 때

    /*수정 시 선택된 데이터를 LineEdit 위젯에 미리 채우기 위한 슬롯함수*/
    void on_tableView5_clicked(const QModelIndex &index);

    /*주문 정보가 추가될 때 필요한 고객 정보를 전달하기 위한 슬롯함수*/
    void orderAddedClient(int);

    /*주문 정보를 검색할 때 필요한 고객 정보를 전달하기 위한 슬롯함수*/
    void orderSearchedClient(int);

    /*주문 정보를 수정할 때 필요한 고객 정보를 전달하기 위한 슬롯 함수*/
    void orderModifiedClient(int, int);

    /*주문 정보에 들어간 고객 정보 콤보박스의 데이터를 채우기 위한 슬롯 함수*/
    void setClientComboBox(QComboBox*, QComboBox*);
};

#endif // CLIENTHANDLERFORM_H
