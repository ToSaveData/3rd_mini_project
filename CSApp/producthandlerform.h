#ifndef PRODUCTHANDLERFORM_H
#define PRODUCTHANDLERFORM_H

#include <QWidget>

namespace Ui {
class ProductHandlerForm;
}

class QComboBox;
class QSqlQuery;
class QSqlTableModel;

class ProductHandlerForm : public QWidget                       //제품 정보를 활용하는 클래스
{
    Q_OBJECT

public:
    explicit ProductHandlerForm(QWidget *parent = nullptr);     //생성자
    ~ProductHandlerForm();                                      //소멸자

signals:
    /*제품 정보가 추가됐음을 주문 정보 클래스에 전달하는 시그널*/
    void productAdded(int);

    /*제품 정보가 삭제됐음을 주문 정보 클래스에 전달하는 시그널*/
    void productRemoved(int);

    /*제품 정보가 수정됐음을 주문 정보 클래스에 전달하는 시그널*/
    void productModified(int, std::vector<QString>);

    /*주문 정보 등록 시 요청한 제품 정보를 인자로 담아 반환하는 시그널*/
    void addReturn(std::vector<QString>);

    /*주문 정보 검색 시 요청한 제품 정보를 인자로 담아 반환하는 시그널*/
    void searchReturn(std::vector<QString>);

    /*주문 정보 수정 시 요청한 제품 정보를 인자로 담아 반환하는 시그널*/
    void modifyReturn(std::vector<QString>, int);

private:
    Ui::ProductHandlerForm *Pui;                                //UI
    int makePid();                                              //제품 ID를 생성하는 함수
    QSqlTableModel *tableModel;                                 //제품 정보를 나타낼 테이블 모델
    QSqlTableModel *searchModel;                                //검색 결과를 나타날 테이블 모델
    QSqlQuery *query;                                           //쿼리문을 사용하기 위한 쿼리

private slots:
    void on_enrollPushButton_clicked();                         //등록 버튼
    void on_searchPushButton_clicked();                         //검색 버튼
    void on_resetPushButton_clicked();                          //초기화 버튼
    void on_removePushButton_clicked();                         //삭제 버튼
    void on_modifyPushButton_clicked();                         //수정 버튼

    /*현재 제품 정보를 입력란에 채워주는 슬롯함수*/
    void on_tableView5_clicked(const QModelIndex &index);

    /*주문 정보가 추가될 때 필요한 제품 정보를//전달하기 위한 슬롯함수*/
    void orderAddedProduct(int);

    /*주문 정보를 검색할 때 필요한 제품 정보를 전달하기 위한 슬롯함수*/
    void orderSearchedProduct(int);

    /*주문 정보를 수정할 때 필요한 제품 정보를 전달하기 위한 슬롯 함수*/
    void orderModifiedProduct(int, int);

    /*주문 정보에 들어간 제품 정보 콤보박스의 데이터를 채우기 위한 슬롯 함수*/
    void setProductComboBox(QComboBox*, QComboBox*);
};

#endif // PRODUCTHANDLERFORM_H
