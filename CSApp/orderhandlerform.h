#ifndef ORDERHANDLERFORM_H
#define ORDERHANDLERFORM_H

#include <QWidget>

namespace Ui {
class OrderHandlerForm;
}

class QComboBox;
class QSqlQuery;
class QSqlTableModel;
class QStandardItemModel;

class OrderHandlerForm : public QWidget                             //주문 정보를 활용하는 클래스
{
    Q_OBJECT

public:
    explicit OrderHandlerForm(QWidget *parent = nullptr);           //생성자
    ~OrderHandlerForm();                                            //소멸자
    void dataload();                                                //detailModel에 데이터를 채우는 함수

signals:
    /*주문 정보를 추가할 때, 고객 정보 클래스에 보내는 시그널*/
    void orderAddedClient(int);

    /*주문 정보를 추가할 때, 제품 정보 클래스에 보내는 시그널*/
    void orderAddedProduct(int);

    /*주문 정보를 검색할 때, 고객 정보 클래스에 보내는 시그널*/
    void orderSearchedClient(int);

    /*주문 정보를 검색할 때, 제품 정보 클래스에 보내는 시그널*/
    void orderSearchedProduct(int);

    /*주문 정보를 변경할 때, 고객 정보 클래스에 보내는 시그널*/
    void orderModifiedClient(int, int);

    /*주문 정보를 변경할 때, 제품 정보 클래스에 보내는 시그널*/
    void orderModifiedProduct(int, int);

    /*고객 정보 콤보박스를 채울 때, 고객 정보 클래스에 보내는 시그널*/
    void clientComboBox(QComboBox*, QComboBox*);

    /*제품 정보 콤보박스를 채울 때, 제품 정보 클래스에 보내는 시그널*/
    void productComboBox(QComboBox*, QComboBox*);

private:
    Ui::OrderHandlerForm *Oui;                                      //UI
    int makeOid() const;                                                  //주문 ID를 생성하는 함수
    int cnt;                                                    //detailModel의 입력행 정보
    QSqlTableModel *tableModel;                                     //주문 정보를 나타낼 테이블 모델
    QStandardItemModel *detailModel;                                //주문 상세 정보를 나타낼 스탠다드 아이템 모델
    QStandardItemModel *searchModel;                                //검색 결과를 나타날 스탠다드 아이템 모델
    QSqlQuery *query;                                               //sql문을 사용하기 위한 쿼리

private slots:
    void on_enrollPushButton_clicked();                             //등록 버튼을 눌렀을 때

    /*주문 정보를 추가할 때, 고객 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void addReturnClient(std::vector<QString>) const;

    /*주문 정보를 추가할 때, 제품 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void addReturnProduct(std::vector<QString>);

    void on_searchPushButton_clicked();                             //검색 버튼을 눌렀을 때

    /*주문 정보를 검색할 때, 고객 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void searchReturnClient(std::vector<QString>) const;

    /*주문 정보를 검색할 때, 제품 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void searchReturnProduct(std::vector<QString>) const;

    void on_resetPushButton_clicked() const;                              //초기화 버튼을 눌렀을 때

    void on_removePushButton_clicked();                             //삭제 버튼을 눌렀을 때

    void on_modifyPushButton_clicked();                             //수정 버튼을 눌렀을 때

    /*주문 정보를 수정할 때, 고객 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void modifyReturnClient(std::vector<QString>, int) const;

    /*주문 정보를 수정할 때, 제품 정보 클래스에서 돌아온 시그널을 받는 슬롯함수*/
    void modifyReturnProduct(std::vector<QString>, int) const;

    /*수정 시 선택된 데이터를 LineEdit 위젯에 미리 채우기 위한 슬롯함수*/
    void on_tableView5_clicked(const QModelIndex &index) const;

    /*고객 정보가 추가됐다는 시그널을 받는 슬롯함수*/
    void clientAdded();

    /*제품 정보가 추가됐다는 시그널을 받는 슬롯함수*/
    void productAdded();

    /*고객 정보가 삭제됐다는 시그널을 받는 슬롯함수*/
    void clientRemoved(int);

    /*제품 정보가 삭제됐다는 시그널을 받는 슬롯함수*/
    void productRemoved(int);

    /*고객 정보가 수정됐다는 시그널을 받는 슬롯함수*/
    void clientModified(int, std::vector<QString>);

    /*제품 정보가 수정됐다는 시그널을 받는 슬롯함수*/
    void productModified(int, std::vector<QString>);
};

#endif // ORDERHANDLERFORM_H
