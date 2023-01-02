#include "orderhandlerform.h"
#include "ui_orderhandlerform.h"
#include <QHash>
#include <QComboBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QStandardItem>

OrderHandlerForm::OrderHandlerForm(QWidget *parent) :               //생성자
    QWidget(parent), Oui(new Ui::OrderHandlerForm), cnt(0),
    tableModel(nullptr), detailModel(nullptr), searchModel(nullptr), query(nullptr)
{
    Oui->setupUi(this);                                             //현재 클래스에 UI를 세팅

    std::vector<QTableView*> viewVec;                                      //모델 설정이 필요한 테이블 뷰 모음
    viewVec.push_back(Oui->tableView1);
    viewVec.push_back(Oui->tableView2);
    viewVec.push_back(Oui->tableView4);
    viewVec.push_back(Oui->tableView5);

    QSqlDatabase db = QSqlDatabase::addDatabase                     //QSQLITE DB에 연결명을 설정하고 추가
            ("QSQLITE", "orderConnection");
    db.setDatabaseName("order.db");                                 //저장될 DB 파일 이름 설정

    if(!db.open())  return;                                         //DB 파일 실행 예외처리

    query = new QSqlQuery(db);                                      //쿼리문을 실행할 DB를 지정

    /*order_info 테이블을 생성하는 쿼리문*/
    query->exec("CREATE TABLE IF NOT EXISTS order_info "
                "(o_id INTEGER Primary Key, "
                "order_date VARCHAR(100) NOT NULL, "
                "order_quantity INTEGER NOT NULL, "
                "c_id INTEGER NOT NULL, "
                "p_id INTEGER NOT NULL);");

    /*order_info 테이블 정보를 표시할 모델 생성 및 설정*/
    tableModel = new QSqlTableModel(this, db);
    tableModel->setTable("order_info");


    detailModel = new QStandardItemModel(0, 10);                    //주문 정보를 표시할 모델 생성

    searchModel = new QStandardItemModel(0, 10);                    //검색 결과를 표시할 모델 생성

    tableModel->select();                                           //db에 저장된 정보를 불러옴

    /*tableModel의 헤더를 설정*/
    tableModel->setHeaderData(0, Qt::Horizontal, tr("oid"));
    tableModel->setHeaderData(1, Qt::Horizontal,
                              tr("order_date"));
    tableModel->setHeaderData(2, Qt::Horizontal,
                              tr("order_quantity"));
    tableModel->setHeaderData(3, Qt::Horizontal, tr("cid"));
    tableModel->setHeaderData(4, Qt::Horizontal, tr("pid"));

    /*detailModel 헤더를 설정*/
    detailModel->setHeaderData(0, Qt::Horizontal, tr("oid"));
    detailModel->setHeaderData(1, Qt::Horizontal,
                               tr("order_date"));
    detailModel->setHeaderData(2, Qt::Horizontal,
                               tr("client_name"));
    detailModel->setHeaderData(3, Qt::Horizontal,
                               tr("client_phone_number"));
    detailModel->setHeaderData(4, Qt::Horizontal,
                               tr("client_address"));
    detailModel->setHeaderData(5, Qt::Horizontal,
                               tr("product_sort"));
    detailModel->setHeaderData(6, Qt::Horizontal,
                               tr("product_name"));
    detailModel->setHeaderData(7, Qt::Horizontal,
                               tr("product_price"));
    detailModel->setHeaderData(8, Qt::Horizontal,
                               tr("order_quantity"));
    detailModel->setHeaderData(9, Qt::Horizontal,
                               tr("total_price"));

    /*searchModel 헤더를 설정*/
    searchModel->setHeaderData(0, Qt::Horizontal, tr("oid"));
    searchModel->setHeaderData(1, Qt::Horizontal,
                               tr("order_date"));
    searchModel->setHeaderData(2, Qt::Horizontal,
                               tr("client_name"));
    searchModel->setHeaderData(3, Qt::Horizontal,
                               tr("client_phone_number"));
    searchModel->setHeaderData(4, Qt::Horizontal,
                               tr("client_address"));
    searchModel->setHeaderData(5, Qt::Horizontal,
                               tr("product_sort"));
    searchModel->setHeaderData(6, Qt::Horizontal,
                               tr("product_name"));
    searchModel->setHeaderData(7, Qt::Horizontal,
                               tr("product_price"));
    searchModel->setHeaderData(8, Qt::Horizontal,
                               tr("order_quantity"));
    searchModel->setHeaderData(9, Qt::Horizontal,
                               tr("total_price"));

    Oui->tableView3->setModel(searchModel);                     //tableView3에 searchModel을 적용

    viewVec[0]->setModel(tableModel);                              //tableView1에 tableModel을 적용
#if 0
    for(int i = 1; i < 4; i++)                                  //남은 테이블 뷰에 detailModel을 적용
        viewVec[i]->setModel(detailModel);

    for(int i = 0; i < viewVec.size(); i++)                        //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->setModel(detailModel);
        i->resizeColumnsToContents();
    }
#endif
}

OrderHandlerForm::~OrderHandlerForm()                               //소멸자
{
    /*생성자에서 만든 포인터 객체 소멸*/
    delete query;
    delete tableModel;
    delete detailModel;
    delete searchModel;
    delete Oui;

    query = nullptr;
    tableModel = nullptr;
    detailModel = nullptr;
    searchModel = nullptr;
    Oui = nullptr;
}

/*detailModel에 데이터를 채우는 함수*/
void OrderHandlerForm::dataload()
{
    emit clientComboBox(Oui->clientIDComboBox1,                     //고객 정보와 관련된 콤보박스를 시그널로 방출
                        Oui->clientInfoComboBox);
    emit productComboBox(Oui->productIDComboBox1,                   //제품 정보와 관련된 콤보박스를 시그널로 방출
                         Oui->productInfoComboBox);

    /*주문 정보가 표시될 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Oui->tableView2);
    viewVec.push_back(Oui->tableView4);
    viewVec.push_back(Oui->tableView5);

    /*order_info에 저장된 주문 정보를 detailModel에 표시하는 반복문*/
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        /*order_info 테이블의 정보를 각 변수에 저장*/
        int oid = tableModel->record(i).value("o_id").toInt();
        QString date = tableModel->record(i).value("order_date").
                        toString();
        int quantity = tableModel->record(i).
                value("order_quantity").toInt();
        int cid = tableModel->record(i).value("c_id").toInt();
        int pid = tableModel->record(i).value("p_id").toInt();

        /*detailMoedl에 들어갈 데이터들*/
        std::vector<QString> strVec;
        strVec.push_back(QString::number(oid));
        strVec.push_back(date);
        strVec.push_back(QString::number(quantity));

        /*QStandardItem*로 자료형 변환*/
        std::vector<QStandardItem*> itemsVec;
#if 0
        for(int i = 0; i < strVec.size(); i++)
        {
            itemsVec.push_back(new QStandardItem(strVec[i]));
        }
#else
        for(const auto &i : strVec){
            itemsVec.push_back(new QStandardItem(i));
        }
#endif

        /*detailMoedl에 데이터 추가*/
        detailModel->appendRow(itemsVec[0]);
        detailModel->setItem(cnt, 1, itemsVec[1]);
        detailModel->setItem(cnt, 8, itemsVec[2]);

        /*deatilModel에 들어갈 고객 정보와 제품 정보를 요청하는 시그널 방출*/
        emit orderAddedClient(cid);
        emit orderAddedProduct(pid);
    }
#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }
#endif
}

int OrderHandlerForm::makeOid()                                     //주문 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 100001;               //첫 번째 주문 ID: 100001
    return 10;                                                      //두 번째 이후는 아무 숫자
}

void OrderHandlerForm::on_enrollPushButton_clicked()                //등록 버튼을 눌렀을 때
{
    /*주문 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Oui->tableView1);
    viewVec.push_back(Oui->tableView2);
    viewVec.push_back(Oui->tableView4);
    viewVec.push_back(Oui->tableView5);

    /*주문 정보가 입력된 lineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Oui->orderDateLineEdit1);
    lineEditVec.push_back(Oui->orderQuantityLineEdit1);

    /*고객 및 제품 정보가 선택된 콤보박스 모음*/
    std::vector<QComboBox*> comboBoxVec;
    comboBoxVec.push_back(Oui->clientIDComboBox1);
    comboBoxVec.push_back(Oui->productIDComboBox1);

#if 1
    /*입력될 주문 정보가 하나라도 없을 경우 등록하지 않음*/
    for(int i = 0; i < lineEditVec.size(); i++)
    {
        if(lineEditVec[i]->text() == "")
            return;
        if(comboBoxVec[i]->currentText() == tr("select item"))
            return;
    }
#else
    for(const auto &i : lineEditVec){
        if(i->text().isEmpty()){
            return;
        }
    }

    for(const auto &i : comboBoxVec){
        if(i->text() == tr("select item")){
            return;
        }
    }

#endif

    int oid = makeOid();                                            //주문 id 생성 후 저장

    /*입력된 내용을 각 변수에 맞게 저장*/
    QString date = Oui->orderDateLineEdit1->text();
    int quantity = Oui->orderQuantityLineEdit1->text().toInt();
    int cid = Oui->clientIDComboBox1->currentText().toInt();
    int pid = Oui->productIDComboBox1->currentText().toInt();

    if(100001 == oid)                                               //첫 번째 데이터 추가 시
    {
        /*order_info 테이블에 주문 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO order_info "
                    "VALUES (?, ?, ?, ?, ?)");
        query->bindValue(0, oid);
        query->bindValue(1, date);
        query->bindValue(2, quantity);
        query->bindValue(3, cid);
        query->bindValue(4, pid);
        query->exec();

        tableModel->select();                                       //테이블 뷰의 정보 최신화

        /*detailMoedl에 들어갈 데이터들*/
        std::vector<QString> strVec;
        strVec.push_back(QString::number(oid));
        strVec.push_back(date);
        strVec.push_back(QString::number(quantity));

        /*QStandardItem*로 자료형 변환*/
        std::vector<QStandardItem*> itemsVec;
#if 0
        for(int i = 0; i < strVec.size(); i++)
        {
            itemsVec.push_back(new QStandardItem(strVec[i]));
        }
#else
        for(const auto &i : strVec){
            itemsVec.push_back(new QStandardItem(i));
        }
#endif

        /*detailMoedl에 데이터 추가*/
        detailModel->appendRow(itemsVec[0]);
        detailModel->setItem(cnt, 1, itemsVec[1]);
        detailModel->setItem(cnt, 8, itemsVec[2]);
    }
    else
    {
        int id = tableModel->record(tableModel->rowCount()-1).      //두번째 이후 데이터를 추가할 경우
                    value("o_id").toInt()+1;

        /*order_info 테이블에 주문 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO order_info "
                    "VALUES (?, ?, ?, ?, ?)");
        query->bindValue(0, id);
        query->bindValue(1, date);
        query->bindValue(2, quantity);
        query->bindValue(3, cid);
        query->bindValue(4, pid);
        query->exec();

        tableModel->select();                                       //테이블 뷰의 정보 최신화

        /*detailMoedl에 들어갈 데이터들*/
        std::vector<QString> strVec;
        strVec.push_back(QString::number(id));
        strVec.push_back(date);
        strVec.push_back(QString::number(quantity));

        /*QStandardItem*로 자료형 변환*/
        std::vector<QStandardItem*> itemsVec;
#if 0
        for(int i = 0; i < strVec.size(); i++)
        {
            itemsVec.push_back(new QStandardItem(strVec[i]));
        }
#else
        for(const auto &i : strVec){
            itemsVec.push_back(new QStandardItem(i));
        }
#endif

        /*detailMoedl에 데이터 추가*/
        detailModel->appendRow(itemsVec[0]);
        detailModel->setItem(cnt, 1, itemsVec[1]);
        detailModel->setItem(cnt, 8, itemsVec[2]);
    }

    /*주문정보가 추가됐다는 시그널을 고객 정보 클래스와 제품 정보 클래스에 방출*/
    emit orderAddedClient(cid);
    emit orderAddedProduct(pid);
#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }
#endif
    /*입력란 초기화*/
    Oui->orderDateLineEdit1->clear();
    Oui->orderQuantityLineEdit1->clear();

    /*콤보박스 상태 초기화*/
    Oui->clientIDComboBox1->setCurrentText(tr("select item"));
    Oui->productIDComboBox1->setCurrentText(tr("select item"));
}

/*등록 탭에서 보낸 시그널로 넘어온 고객 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::addReturnClient(std::vector<QString> cinfoVec)
{
    /*주문 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Oui->tableView1);
    viewVec.push_back(Oui->tableView2);
    viewVec.push_back(Oui->tableView4);
    viewVec.push_back(Oui->tableView5);

    /*넘어온 고객 정보를 각 변수로 저장 */
    QString name = cinfoVec[0];
    QString phoneNum = cinfoVec[1];
    QString address = cinfoVec[2];

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString> strVec;
    strVec.push_back(name);
    strVec.push_back(phoneNum);
    strVec.push_back(address);

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(cnt, 2, itemsVec[0]);
    detailModel->setItem(cnt, 3, itemsVec[1]);
    detailModel->setItem(cnt, 4, itemsVec[2]);
#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }
#endif
}

/*등록 탭에서 보낸 시그널로 넘어온 제품 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::addReturnProduct(std::vector<QString> pinfoVec)
{
    /*주문 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Oui->tableView1);
    viewVec.push_back(Oui->tableView2);
    viewVec.push_back(Oui->tableView4);
    viewVec.push_back(Oui->tableView5);

    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfoVec[0];
    QString name = pinfoVec[1];
    int price = pinfoVec[2].toInt();

    /*detailModel에 필요한 기존 정보 저장*/
    int quantity = tableModel->record(cnt).
                    value("order_quantity").toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString> strVec;
    strVec.push_back(sort);
    strVec.push_back(name);
    strVec.push_back(QString::number(price));
    strVec.push_back(QString::number(quantity));
    strVec.push_back(QString::number(totalPrice));

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(cnt, 5, itemsVec[0]);
    detailModel->setItem(cnt, 6, itemsVec[1]);
    detailModel->setItem(cnt, 7, itemsVec[2]);
    detailModel->setItem(cnt, 8, itemsVec[3]);
    detailModel->setItem(cnt, 9, itemsVec[4]);
#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }
#endif
    cnt++;                                                          //detailModel 다음 등록행 저장
}

void OrderHandlerForm::on_searchPushButton_clicked()                //검색 버튼을 눌렀을 때
{
    searchModel->removeRows(0,searchModel->rowCount());             //이전 검색 데이터 초기화

    int oid = Oui->searchLineEdit->text().toInt();                  //입력된 주문 id 저장

    if(oid == 0)                                                    //예외처리
    {
        Oui->searchLineEdit->clear();
        return;
    }

    /*현재 주문 id와 일치하는 필터를 적용*/
    tableModel->setFilter("o_id = " + QString::number(oid));
    tableModel->select();                                           //테이블 뷰의 정보 최신화

    /*현재 주문 id와 일치하는 주문 정보 저장*/
    QString date = tableModel->record(0).value("order_date").
                    toString();
    int quantity = tableModel->record(0).value("order_quantity").
                    toInt();
    int cid = tableModel->record(0).value("c_id").toInt();
    int pid = tableModel->record(0).value("p_id").toInt();

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString> strVec;
    strVec.push_back(QString::number(oid));
    strVec.push_back(date);
    strVec.push_back(QString::number(quantity));

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    searchModel->appendRow(itemsVec[0]);
    searchModel->setItem(0, 1, itemsVec[1]);
    searchModel->setItem(0, 8, itemsVec[2]);

    /*고객 정보 클래스에 고객 정보를 요청하는 시그널 방출*/
    emit orderSearchedClient(cid);

    /*제품 정보 클래스에 제품 정보를 요청하는 시그널 방출*/
    emit orderSearchedProduct(pid);

    Oui->tableView3->resizeColumnsToContents();                     //테이블 뷰의 입력 정보에 따른 열 너비 조절

    Oui->searchLineEdit->clear();                                   //입력란 초기화
}

/*검색 탭에서 보낸 시그널로 넘어온 고객 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::searchReturnClient(std::vector<QString> cinfoVec)
{
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfoVec[0];
    QString phoneNum = cinfoVec[1];
    QString address = cinfoVec[2];

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString>  strVec;
    strVec.push_back(name);
    strVec.push_back(phoneNum);
    strVec.push_back(address);

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    searchModel->setItem(0, 2, itemsVec[0]);
    searchModel->setItem(0, 3, itemsVec[1]);
    searchModel->setItem(0, 4, itemsVec[2]);
}

/*검색 탭에서 보낸 시그널로 넘어온 제품 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::searchReturnProduct(std::vector<QString> pinfoVec)
{
    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfoVec[0];
    QString name = pinfoVec[1];
    int price = pinfoVec[2].toInt();
    int quantity = tableModel->record(0).
                    value("order_quantity").toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString>  strVec;
    strVec.push_back(sort);
    strVec.push_back(name);
    strVec.push_back(QString::number(price));
    strVec.push_back(QString::number(quantity));
    strVec.push_back(QString::number(totalPrice));

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    searchModel->setItem(0, 5, itemsVec[0]);
    searchModel->setItem(0, 6, itemsVec[1]);
    searchModel->setItem(0, 7, itemsVec[2]);
    searchModel->setItem(0, 8, itemsVec[3]);
    searchModel->setItem(0, 9, itemsVec[4]);

    Oui->tableView3->resizeColumnsToContents();                     //테이블 뷰의 입력 정보에 따른 열 너비 조절

    /*모든 정보를 표시하도록 tableModel의 필터 재설정*/
    tableModel->setFilter("o_id LIKE'1%'");
}

void OrderHandlerForm::on_resetPushButton_clicked()                 //초기화 버튼을 눌렀을 때
{
    searchModel->removeRows(0,searchModel->rowCount());             //검색 결과 초기화
}

void OrderHandlerForm::on_removePushButton_clicked()                //삭제 버튼을 눌렀을 때
{
    int row = Oui->tableView4->currentIndex().row();                //현재 선택된 행을 저장
    int oid = tableModel->record(row).value("o_id").toInt();        //현재 행의 oid 저장

    /*order_info 테이블에서 선택된 oid와 일치하는 정보를 삭제하는 쿼리문*/
    query->prepare("DELETE FROM order_info WHERE o_id = ?");
    query->bindValue(0, oid);
    query->exec();

    tableModel->select();                                           //테이블 뷰의 정보 최신화

    detailModel->removeRows(row, 1);                                //detailModel에서 선택된 정보 삭제

    if(detailModel->rowCount() > 0)                                 //detailModel의 입력행 정보 최신화
        cnt = detailModel->rowCount();
}

void OrderHandlerForm::on_modifyPushButton_clicked()                //수정 버튼을 눌렀을 때
{
    /*수정될 정보가 입력된 lineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Oui->orderIDLineEdit);
    lineEditVec.push_back(Oui->orderDateLineEdit2);
    lineEditVec.push_back(Oui->orderQuantityLineEdit2);

    /*현재 lineEdit에 있는 정보를 각 변수에 저장*/
    int oid = lineEditVec[0]->text().toInt();
    QString date = lineEditVec[1]->text();
    int quantity = lineEditVec[2]->text().toInt();

    /*주문 정보 수정 탭에 있는 콤보박스 모음*/
    std::vector<QComboBox*> comboBoxVec;
    comboBoxVec.push_back(Oui->clientInfoComboBox);
    comboBoxVec.push_back(Oui->productInfoComboBox);

#if 0
    /*수정하려는 데이터가 하나라도 비어 있는 경우 수정 불가*/
    for(int i = 0; i < lineEditVec.size(); i++)
    {
        if(lineEditVec[i]->text() == "")
            return;
    }
#else
    for(const auto &i : lineEditVec){
        if(i->text().isEmpty()){
            return;
        }
    }
#endif
    if(comboBoxVec[0]->currentText() == tr("select item")) return;
    if(comboBoxVec[1]->currentText() == tr("select item")) return;

    /*고객 정보 콤보박스에서 현재 선택된 데이터의 index 출력*/
    int cBoxIndex = Oui->clientInfoComboBox->currentIndex();

    /*제품 정보 콤보박스에서 현재 선택된 데이터의 index 출력*/
    int pBoxIndex = Oui->productInfoComboBox->currentIndex();

    /*고객 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int cid = Oui->clientIDComboBox1->itemText(cBoxIndex).toInt();  //고객 ID 콤보박스에서 현재 선택된
                                                                    //데이터의 cid를 출력

    /*제품 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int pid = Oui->productIDComboBox1->itemText(pBoxIndex).toInt(); //제품 ID 콤보박스에서 현재 선택된
                                                                    //데이터의 pid를 출력

    int row = Oui->tableView5->currentIndex().row();                //현재 선택된 인덱스의 행을 저장

    /*order_info 테이블의 정보를 수정하는 쿼리문*/
    query->prepare("UPDATE order_info SET "
                    "order_date = ?, order_quantity = ?, "
                    "c_id = ?, p_id = ? "
                    "WHERE o_id = ?");
    query->bindValue(0, date);
    query->bindValue(1, quantity);
    query->bindValue(2, cid);
    query->bindValue(3, pid);
    query->bindValue(4, oid);
    query->exec();

    tableModel->select();                                           //테이블 뷰의 정보 최신화

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString>  strVec;
    strVec.push_back(QString::number(oid));
    strVec.push_back(date);
    strVec.push_back(QString::number(quantity));

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 0, itemsVec[0]);
    detailModel->setItem(row, 1, itemsVec[1]);
    detailModel->setItem(row, 8, itemsVec[2]);

    /*고객 정보 클래스에 주문 정보 변경에 필요한 고객 정보를 요청하는 시그널 방출*/
    emit orderModifiedClient(cid, row);

    /*제품 정보 클래스에 주문 정보 변경에 필요한 제품 정보를 요청하는 시그널 방출*/
    emit orderModifiedProduct(pid, row);

    /*입력란 초기화*/
    Oui->orderIDLineEdit->clear();
    Oui->orderDateLineEdit2->clear();
    Oui->orderQuantityLineEdit2->clear();

    /*콤보박스 상태 초기화*/
    Oui->clientInfoComboBox->setCurrentText(tr("select item"));
    Oui->productInfoComboBox->setCurrentText(tr("select item"));
}

/*수정 탭에서 보낸 시그널로 넘어온 고객 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::modifyReturnClient(std::vector<QString> cinfoVec, int row)
{
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfoVec[0];
    QString phoneNum = cinfoVec[1];
    QString address = cinfoVec[2];

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString>  strVec;
    strVec.push_back(name);
    strVec.push_back(phoneNum);
    strVec.push_back(address);

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 2, itemsVec[0]);
    detailModel->setItem(row, 3, itemsVec[1]);
    detailModel->setItem(row, 4, itemsVec[2]);
}

/*수정 탭에서 보낸 시그널로 넘어온 제품 정보로 detailModel을 채우는 슬롯함수*/
void OrderHandlerForm::modifyReturnProduct(std::vector<QString> pinfoVec, int row)
{
    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfoVec[0];
    QString name = pinfoVec[1];
    int price = pinfoVec[2].toInt();

    /*기존 수량과 총 가격 정보를 각 변수로 저장*/
    int quantity = detailModel->item(row, 8)->text().toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    std::vector<QString>  strVec;
    strVec.push_back(sort);
    strVec.push_back(name);
    strVec.push_back(QString::number(price));
    strVec.push_back(QString::number(quantity));
    strVec.push_back(QString::number(totalPrice));

    /*QStandardItem*로 자료형 변환*/
    std::vector<QStandardItem*> itemsVec;
#if 0
    for(int i = 0; i < strVec.size(); i++)
    {
        itemsVec.push_back(new QStandardItem(strVec[i]));
    }
#else
    for(const auto &i : strVec){
        itemsVec.push_back(new QStandardItem(i));
    }
#endif

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 5, itemsVec[0]);
    detailModel->setItem(row, 6, itemsVec[1]);
    detailModel->setItem(row, 7, itemsVec[2]);
    detailModel->setItem(row, 8, itemsVec[3]);
    detailModel->setItem(row, 9, itemsVec[4]);
}

/*현재 주문 정보를 입력란에 채워주는 슬롯함수*/
void OrderHandlerForm::on_tableView5_clicked(const QModelIndex &index)
{
    std::vector<QLineEdit*> lineEditVec;                                   //현재 주문 정보를 대입할 LineEdit 위젯 저장
    lineEditVec.push_back(Oui->orderIDLineEdit);
    lineEditVec.push_back(Oui->orderDateLineEdit2);
    lineEditVec.push_back(Oui->orderQuantityLineEdit2);

    int row = index.row();                                          //현재 행을 저장

    /*입력란과 콤보박스에 설정할 주문 정보 저장*/
    int oid = detailModel->item(row, 0)->text().toInt();
    QString date = detailModel->item(row, 1)->text();
    int quantity = detailModel->item(row, 8)->text().toInt();
    QString cName = detailModel->item(row, 2)->text();
    QString phoneNum = detailModel->item(row, 3)->text();
    QString pSort = detailModel->item(row, 5)->text();
    QString pName = detailModel->item(row, 6)->text();

    /*입력란에 고객 및 제품 정보 설정*/
    lineEditVec[0]->setText(QString::number(oid));
    lineEditVec[1]->setText(date);
    lineEditVec[2]->setText(QString::number(quantity));

    /*콤보박스에 고객 및 제품 정보 설정*/
    Oui->clientInfoComboBox->setCurrentText
            (cName + "(" + phoneNum + ")");                         //이름(전화번호) -> 동명이인 구분을 위함
    Oui->productInfoComboBox->setCurrentText
            (pName + "(" + pSort + ")");                            //제품명(제품 종류) -> 제품 정보 식별을 위함
}

void OrderHandlerForm::clientAdded()                                //고객 정보가 추가 됐다는 시그널을 받는 슬롯함수
{
    emit clientComboBox(Oui->clientIDComboBox1,                     //콤보박스에 고객 정보를 추가하기 위한 시그널 방출
                        Oui->clientInfoComboBox);
}

void OrderHandlerForm::productAdded()                               //제품 정보가 추가 됐다는 시그널을 받는 슬롯함수
{
    emit productComboBox(Oui->productIDComboBox1,                   //콤보박스에 제품 정보를 추가하기 위한 시그널 방출
                         Oui->productInfoComboBox);
}

/*고객 정보가 삭제됐다는 시그널을 받은 슬롯함수*/
void OrderHandlerForm::clientRemoved(int cid)
{
    emit clientComboBox(Oui->clientIDComboBox1,                     //고객 정보 관련 콤보박스 정보를 재설정
                        Oui->clientInfoComboBox);

    /*삭제된 cid를 포함하는 주문 ID를 저장하는 배열을 채움*/
    std::vector<int> oidVec;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("c_id").toInt()== cid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidVec.push_back(oid);
        }
    }

    /*order_info에 있는 삭제된 고객 관련 정보 삭제*/
    query->prepare("DELETE FROM order_info WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

#if 0
    /*oid가 저장된 수만큼 cid가 포함된 주문 내역 삭제 반복*/
    for(int i = 0; i < oidVec.size(); i++)
    {
        int row;                                                    //삭제될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidVec[i])))
        {
            row = k->index().row();                                 //삭제될 행 저장
            detailModel->removeRows(row, 1);                        //detailModel에서 해당 행 삭제
        }
    }
#else
    for(const auto &i : oidVec){
        int row;                                                    //삭제될 정보가 있는 행
        foreach (const auto &k, detailModel->
                 findItems(QString::number(i)))
        {
            row = k->index().row();                                 //삭제될 행 저장
            detailModel->removeRows(row, 1);                        //detailModel에서 해당 행 삭제
        }
    }
#endif

    tableModel->select();                                           //테이블 뷰의 정보 최신화
}

/*제품 정보가 삭제됐다는 시그널을 받은 슬롯함수*/
void OrderHandlerForm::productRemoved(int pid)
{
    emit productComboBox(Oui->productIDComboBox1,                   //제품 정보 관련 콤보박스 정보를 재설정
                         Oui->productInfoComboBox);

    /*삭제된 pid를 포함하는 주문 ID를 저장하는 배열을 채움*/
    std::vector<int> oidVec;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("p_id").toInt()== pid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidVec.push_back(oid);
        }
    }

    /*order_info에 있는 삭제된 제품 관련 정보 삭제*/
    query->prepare("DELETE FROM order_info WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

#if 0
    /*oid가 저장된 수만큼 pid가 포함된 주문 내역 삭제 반복*/
    for(int i = 0; i < oidVec.size(); i++)
    {
        int row;                                                    //삭제될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidVec[i])))
        {
            row = k->index().row();                                 //삭제될 행 저장
            detailModel->removeRows(row, 1);                        //detailModel에서 해당 행 삭제
        }
    }
#else
    for(const auto &i : oidVec){
        int row;                                                    //삭제될 정보가 있는 행
        foreach (const auto &k, detailModel->
                 findItems(QString::number(i)))
        {
            row = k->index().row();                                 //삭제될 행 저장
            detailModel->removeRows(row, 1);                        //detailModel에서 해당 행 삭제
        }
    }
#endif

    tableModel->select();                                           //테이블 뷰의 정보 최신화
}

/*고객 정보가 수정됐다는 시그널을 받는 슬롯함수*/
void OrderHandlerForm::clientModified(int cid, std::vector<QString> cinfoVec)
{
    emit clientComboBox(Oui->clientIDComboBox1,                     //고객 정보 관련 콤보박스 정보를 재설정
                        Oui->clientInfoComboBox);

    /*수정된 고객 ID를 포함하는 주문 ID를 저장*/
    std::vector<int> oidVec;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("c_id").toInt()== cid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidVec.push_back(oid);
        }
    }

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfoVec[0];
    QString phoneNum = cinfoVec[1];
    QString address = cinfoVec[2];
#if 0
    for(int i = 0; i < oidVec.size(); i++)
    {
        int row;                                                    //수정될 정보가 있는 행

        foreach (auto k, detailModel->
                 findItems(QString::number(oidVec[i])))
        {
            row = k->index().row();                                 //수정될 행 저장

            /*detailMoedl에 들어갈 데이터들*/
            std::vector<QString>  strVec;
            strVec.push_back(name);
            strVec.push_back(phoneNum);
            strVec.push_back(address);

            /*QStandardItem*로 자료형 변환*/
            std::vector<QStandardItem*> itemsVec;
            for(int i = 0; i < strVec.size(); i++)
            {
                itemsVec.push_back(new QStandardItem(strVec[i]));
            }

            /*detailMoedl에 데이터 수정*/
            detailModel->setItem(row, 2, itemsVec[0]);
            detailModel->setItem(row, 3, itemsVec[1]);
            detailModel->setItem(row, 4, itemsVec[2]);
        }
    }
#else
    for(const auto &i : oidVec){
        int row;                                                    //수정될 정보가 있는 행

        foreach (auto k, detailModel->
                 findItems(QString::number(i)))
        {
            row = k->index().row();                                 //수정될 행 저장

            /*detailMoedl에 들어갈 데이터들*/
            std::vector<QString>  strVec;
            strVec.push_back(name);
            strVec.push_back(phoneNum);
            strVec.push_back(address);

            /*QStandardItem*로 자료형 변환*/
            std::vector<QStandardItem*> itemsVec;
            for(const auto &j : strVec){
                itemsVec.push_back(new QStandardItem(j));
            }

            /*detailMoedl에 데이터 수정*/
            detailModel->setItem(row, 2, itemsVec[0]);
            detailModel->setItem(row, 3, itemsVec[1]);
            detailModel->setItem(row, 4, itemsVec[2]);
        }
    }
#endif
}

/*제품 정보가 수정됐다는 시그널을 받는 슬롯함수*/
void OrderHandlerForm::productModified(int pid, std::vector<QString> pinfoVec)
{
    emit productComboBox(Oui->productIDComboBox1,                   //제품 정보 관련 콤보박스 정보를 재설정
                         Oui->productInfoComboBox);

    /*수정된 제품 ID를 포함하는 주문 ID와 주문 수량을 저장*/
    std::vector<int> oidVec;
    std::vector<int> quantityVec;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("p_id").toInt()== pid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            int quantity = tableModel->record(i).
                    value("order_quantity").toInt();
            oidVec.push_back(oid);
            quantityVec.push_back(quantity);
        }
    }

    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfoVec[0];
    QString name = pinfoVec[1];
    int price = pinfoVec[2].toInt();
#if 0
    /*저장된 주문 ID가 포함된 주문 내역 수정*/
    for(int i = 0; i < oidVec.size(); i++)
    {
        int totalPrice = price * quantityVec[i];                   //해당 행의 주문 수량으로 총 가격 계산

        /*detailMoedl에 들어갈 데이터들*/
        std::vector<QString>  strVec;
        strVec.push_back(sort);
        strVec.push_back(name);
        strVec.push_back(QString::number(price));
        strVec.push_back(QString::number(quantityVec[i]));
        strVec.push_back(QString::number(totalPrice));

        /*QStandardItem*로 자료형 변환*/
        std::vector<QStandardItem*> itemsVec;
        for(int i = 0; i < strVec.size(); i++)
        {
            itemsVec.push_back(new QStandardItem(strVec[i]));
        }

        int row;                                                    //수정될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidVec[i])))
        {
            row = k->index().row();                                 //수정될 행 저장

            /*detailMoedl의 데이터 수정*/
            detailModel->setItem(row, 5, itemsVec[0]);
            detailModel->setItem(row, 6, itemsVec[1]);
            detailModel->setItem(row, 7, itemsVec[2]);
            detailModel->setItem(row, 8, itemsVec[3]);
            detailModel->setItem(row, 9, itemsVec[4]);
        }
    }
#else
    /*size() 함수가 있지만 실제 인덱스인 i가 사용되므로 변경 X*/
        /*저장된 주문 ID가 포함된 주문 내역 수정*/
        for(int i = 0; i < oidVec.size(); i++)
        {
            int totalPrice = price * quantityVec[i];                   //해당 행의 주문 수량으로 총 가격 계산

            /*detailMoedl에 들어갈 데이터들*/
            std::vector<QString>  strVec;
            strVec.push_back(sort);
            strVec.push_back(name);
            strVec.push_back(QString::number(price));
            strVec.push_back(QString::number(quantityVec[i]));
            strVec.push_back(QString::number(totalPrice));

            /*QStandardItem*로 자료형 변환*/
            std::vector<QStandardItem*> itemsVec;

            /*여기서는 변경 가능*/
            for(const auto &i : strVec){
                itemsVec.push_back(new QStandardItem(i));
            }

            int row;                                                    //수정될 정보가 있는 행
            foreach (auto k, detailModel->
                     findItems(QString::number(oidVec[i])))
            {
                row = k->index().row();                                 //수정될 행 저장

                /*detailMoedl의 데이터 수정*/
                detailModel->setItem(row, 5, itemsVec[0]);
                detailModel->setItem(row, 6, itemsVec[1]);
                detailModel->setItem(row, 7, itemsVec[2]);
                detailModel->setItem(row, 8, itemsVec[3]);
                detailModel->setItem(row, 9, itemsVec[4]);
            }
        }
#endif
}
