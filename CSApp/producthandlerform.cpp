#include "producthandlerform.h"
#include "ui_producthandlerform.h"
#include <QComboBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

ProductHandlerForm::ProductHandlerForm(QWidget *parent) :           //생성자
    QWidget(parent), Pui(new Ui::ProductHandlerForm),
    tableModel(nullptr), searchModel(nullptr), query(nullptr)
{
    Pui->setupUi(this);                                             //현재 클래스에 UI를 세팅

    std::vector<QTableView*> viewVec;                                      //모델 설정이 필요한 테이블 뷰 모음
    viewVec.push_back(Pui->tableView1);
    viewVec.push_back(Pui->tableView2);
    viewVec.push_back(Pui->tableView4);
    viewVec.push_back(Pui->tableView5);

    QSqlDatabase db = QSqlDatabase::addDatabase                     //QSQLITE DB에 연결명을 설정하고 추가
                        ("QSQLITE", "productConnection");
    db.setDatabaseName("product.db");                               //저장될 DB 파일 이름 설정

    if(!db.open())  return;                                         //DB 파일 실행 예외처리

    query = new QSqlQuery(db);                                      //쿼리문을 실행할 DB를 지정

    /*product 테이블을 생성하는 쿼리문*/
    query->exec("CREATE TABLE IF NOT EXISTS product"
                "(p_id INTEGER Primary Key, "
                "name VARCHAR(100) NOT NULL, "
                "price INTEGER NOT NULL, "
                "sort VARCHAR(100) NOT NULL);");

    tableModel = new QSqlTableModel(this, db);                      //제품 정보를 표시할 모델 생성
    tableModel->setTable("product");                                //모델에 product 테이블 설정

    searchModel = new QSqlTableModel(this, db);                     //제품 정보를 표시할 모델 생성
    searchModel->setTable("product");                               //모델에 product 테이블 설정

    tableModel->select();                                           //db에 저장된 정보를 불러옴

    /*tableModel의 헤더를 설정*/
    tableModel->setHeaderData(0, Qt::Horizontal, tr("pid"));
    tableModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    tableModel->setHeaderData(2, Qt::Horizontal, tr("price"));
    tableModel->setHeaderData(3, Qt::Horizontal, tr("sort"));

    /*searchModel의 헤더를 설정*/
    searchModel->setHeaderData(0, Qt::Horizontal, tr("pid"));
    searchModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    searchModel->setHeaderData(2, Qt::Horizontal, tr("price"));
    searchModel->setHeaderData(3, Qt::Horizontal, tr("sort"));

    Pui->tableView3->setModel(searchModel);                     //tableView3에 searchModel을 적용
#if 0
    for(int i = 0; i < 4; i++)                                  //남은 테이블 뷰에 TableModel을 적용
        viewVec[i]->setModel(tableModel);

    for(int i = 0; i < viewVec.size(); i++)                        //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->setModel(tableModel);
        i->resizeColumnsToContents();
    }
#endif
}

ProductHandlerForm::~ProductHandlerForm()                           //소멸자
{
    /*생성자에서 만든 포인터 객체 소멸*/
    delete query;
    delete tableModel;
    delete searchModel;
    delete Pui;

    query = nullptr;
    tableModel = nullptr;
    searchModel = nullptr;
    Pui = nullptr;
}

int ProductHandlerForm::makePid()                                   //제품 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 1001;                 //첫 번째 제품 ID: 1001
    return 10;                                                      //두 번째 이후는 아무 숫자
}

void ProductHandlerForm::on_enrollPushButton_clicked()              //등록 버튼을 눌렀을 때
{
    /*제품 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Pui->tableView1);
    viewVec.push_back(Pui->tableView2);
    viewVec.push_back(Pui->tableView4);
    viewVec.push_back(Pui->tableView5);

    /*제품 정보가 입력된 lineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Pui->nameLineEdit1);
    lineEditVec.push_back(Pui->priceLineEdit1);
    lineEditVec.push_back(Pui->sortLineEdit1);
#if 0
    /*입력된 데이터가 하나라도 없을 경우 등록하지 않음*/
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

    /*입력된 데이터 저장*/
    int pid = makePid();
    QString name = lineEditVec[0]->text();
    int price = lineEditVec[1]->text().toInt();
    QString sort = lineEditVec[2]->text();

    if(1001 == pid)                                                 //첫 번째 데이터가 입력될 경우
    {
        /*product 테이블에 제품 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO product VALUES (?, ?, ?, ?)");
        query->bindValue(0, pid);
        query->bindValue(1, name);
        query->bindValue(2, price);
        query->bindValue(3, sort);
        query->exec();
        tableModel->select();                                       //테이블 뷰의 정보 최신화

        /*주문 정보 클래스에 새 제품 정보가 추가 됐다는 시그널 방출*/
        emit productAdded(pid);
    }
    else                                                            //두 번째부터 데이터가 입력될 경우
    {
        int id = tableModel->record(tableModel->rowCount()-1).      //등록될 ID 설정
                    value("p_id").toInt()+1;

        /*product 테이블에 제품 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO product VALUES(?, ?, ?, ?)");
        query->bindValue(0, id);
        query->bindValue(1, name);
        query->bindValue(2, price);
        query->bindValue(3, sort);
        query->exec();
        tableModel->select();                                       //테이블 뷰의 정보 최신화

        /*주문 정보 클래스에 새 제품 정보가 추가 됐다는 시그널 방출*/
        emit productAdded(id);
    }

#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();

    for (int i = 0 ; i < lineEditVec.size(); i++)                      //입력란 초기화
        lineEditVec[i]->clear();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }

    for(const auto &i : lineEditVec){
        i->clear();
    }
#endif
}

void ProductHandlerForm::on_searchPushButton_clicked()              //검색 버튼을 눌렀을 때
{
    int pid = Pui->searchLineEdit->text().toInt();                  //입력된 고객 ID 저장

    if(pid == 0)                                                    //예외처리
    {
        Pui->searchLineEdit->clear();
        return;
    }

    searchModel->setFilter("p_id = " + QString::number(pid));       //product 테이블에 필터 설정
    searchModel->select();                                          //테이블 뷰의 정보 최신화

    Pui->tableView3->resizeColumnsToContents();                     //테이블 뷰의 입력 정보에 따른 열 너비 조절
    Pui->searchLineEdit->clear();                                   //입력란 초기화
}

void ProductHandlerForm::on_resetPushButton_clicked()               //검색 초기화 버튼을 눌렀을 때
{
    searchModel->setFilter("p_id = ''");                            //client 테이블에 필터 설정
    searchModel->select();                                          //테이블 뷰의 정보 최신화
}

void ProductHandlerForm::on_removePushButton_clicked()              //삭제 버튼을 눌렀을 때
{
    int row = Pui->tableView4->currentIndex().row();                //현재 선택된 행을 저장

    int pid = tableModel->record(row).value("p_id").toInt();        //현재 선택된 행의 pid 저장

    /*product 테이블에서 선택된 pid와 일치하는 정보를 삭제하는 쿼리문*/
    query->prepare("DELETE FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    tableModel->select();                                           //테이블 뷰의 정보 최신화

    /*주문 정보 클래스에 제품 정보가 삭제됐다는 시그널 방출*/
    emit productRemoved(pid);
}

void ProductHandlerForm::on_modifyPushButton_clicked()              //수정 버튼을 눌렀을 때
{
    /*제품 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Pui->tableView1);
    viewVec.push_back(Pui->tableView2);
    viewVec.push_back(Pui->tableView4);
    viewVec.push_back(Pui->tableView5);

    /*수정될 정보가 입력된 lineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Pui->idLineEdit);
    lineEditVec.push_back(Pui->nameLineEdit2);
    lineEditVec.push_back(Pui->priceLineEdit2);
    lineEditVec.push_back(Pui->sortLineEdit2);

#if 0
    /*입력란에 입력된 정보가 하나라도 없으면 정보 수정을 하지 않음*/
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

    /*입력란에 입력된 정보를 각 변수에 저장*/
    int pid = lineEditVec[0]->text().toInt();
    QString name = lineEditVec[1]->text();
    int price = lineEditVec[2]->text().toInt();
    QString sort = lineEditVec[3]->text();

    /*client 테이블의 정보를 수정하는 쿼리문*/
    query->prepare("UPDATE product SET "
                   "name = ?, price = ?, sort = ? "
                   "WHERE p_id = ?");
    query->bindValue(0, name);
    query->bindValue(1, price);
    query->bindValue(2, sort);
    query->bindValue(3, pid);
    query->exec();

    tableModel->select();                                           //테이블 뷰의 정보 최신화

    std::vector<QString> pinfoVec;                                           //주문 정보 클래스에 보낼 제품 정보 배열

    /*제품 정보를 담음*/
    pinfoVec.push_back(sort);
    pinfoVec.push_back(name);
    pinfoVec.push_back(QString::number(price));

    /*주문 정보 클래스에 제품 정보가 수정됐다는 시그널 방출*/
    emit productModified(pid, pinfoVec);

#if 0
    for(int i = 0; i < viewVec.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();

    for (int i = 0 ; i < lineEditVec.size(); i++)                      //입력란 초기화
        lineEditVec[i]->clear();
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }

    for(const auto &i : lineEditVec){
        i->clear();
    }
#endif
}

/*현재 제품 정보를 입력란에 채워주는 슬롯함수*/
void ProductHandlerForm::on_tableView5_clicked(const QModelIndex &index)
{
    int row = index.row();                                          //현재 선택된 행을 저장

    /*현재 선택된 행의 정보를 저장*/
    int pid = tableModel->record(row).value("p_id").
            toInt();
    QString name = tableModel->record(row).value("name").
            toString();
    int price = tableModel->record(row).value("price").
            toInt();
    QString sort = tableModel->record(row).value("sort").
            toString();

    /*현재 선택된 행의 정보를 입력란에 채워줌*/
    Pui->idLineEdit->setText(QString::number(pid));
    Pui->nameLineEdit2->setText(name);
    Pui->priceLineEdit2->setText(QString::number(price));
    Pui->sortLineEdit2->setText(sort);
}

/*새로운 주문 정보를 등록할 경우 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderAddedProduct(int pid)
{
    std::vector<QString> pinfoVec;                                           //제품 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 제품 정보만 가져오는 쿼리문*/
    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfoVec.push_back(sort);
    pinfoVec.push_back(name);
    pinfoVec.push_back(QString::number(price));

    emit addReturn(pinfoVec);                                          //담은 제품 정보를 시그널로 방출
}

/*주문 정보 클래스에서 검색할 경우 필요한 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderSearchedProduct(int pid)
{
    std::vector<QString> pinfoVec;                                           //제품 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 제품 정보만 가져오는 쿼리문*/
    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfoVec.push_back(sort);
    pinfoVec.push_back(name);
    pinfoVec.push_back(QString::number(price));

    emit searchReturn(pinfoVec);                                       //담은 제품 정보를 시그널로 방출
}

/*주문 정보 클래스에서 주문 정보를 수정할 경우 필요한 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderModifiedProduct(int pid, int row)
{
    std::vector<QString> pinfoVec;                                           //제품 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 제품 정보만 가져오는 쿼리문*/
    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfoVec.push_back(sort);
    pinfoVec.push_back(name);
    pinfoVec.push_back(QString::number(price));

    emit modifyReturn(pinfoVec, row);                                  //담은 제품 정보를 시그널로 방출
}

/*주문 정보 클래스의 제품 정보 콤보박스 채우기*/
void ProductHandlerForm::setProductComboBox(QComboBox* PidBox, QComboBox* PinfoBox)
{
    /*콤보박스를 채우기 위해 필요한 제품 정보를 나타내는 변수 선언*/
    int id;
    QString name;
    QString sort;

    /*기존에 콤보박스에 저장된 고객 정보를 모두 삭제*/
    PidBox->clear();
    PinfoBox->clear();

    /*"선택하세요" 초기값 추가*/
    PidBox->addItem(tr("select item"));
    PinfoBox->addItem(tr("select item"));

    for(int i = 0; i < tableModel->rowCount(); i++)                 //db에 저장된 제품 정보의 수만큼 반복
    {
        id = tableModel->record(i).value("p_id").toInt();           //제품 id 추출
        name = tableModel->record(i).value("name").toString();      //제품 성명 추출
        sort = tableModel->record(i).value("sort").                 //제품 종류 추출
                toString();

        if(PidBox->findText(QString::number(id)) < 0)               //id 콤보박스에 중복되지 않도록
            PidBox->addItem(QString::number(id));                   //제품 id 추가

        if(PinfoBox->findText(name + "(" + sort + ")") < 0)         //제품명(제품종류) 콤보박스에
            PinfoBox->addItem(name + "(" + sort + ")");             //중복되지 않도록 제품 정보 추가
    }
}
