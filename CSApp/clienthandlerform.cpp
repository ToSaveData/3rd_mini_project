#include "clienthandlerform.h"
#include "ui_clienthandlerform.h"
#include <QComboBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

ClientHandlerForm::ClientHandlerForm(QWidget *parent) :         //생성자
    QWidget(parent), Cui(new Ui::ClientHandlerForm), tableModel(nullptr),
    searchModel(nullptr), query(nullptr)
{
    Cui->setupUi(this);                                         //현재 클래스에 UI를 세팅

    std::vector<QTableView*> viewVec;                                  //모델 설정이 필요한 테이블 뷰 모음
    viewVec.push_back(Cui->tableView1);
    viewVec.push_back(Cui->tableView2);
    viewVec.push_back(Cui->tableView4);
    viewVec.push_back(Cui->tableView5);

    QSqlDatabase db = QSqlDatabase::addDatabase                 //QSQLITE DB에 연결명을 설정하고 추가
                        ("QSQLITE", "clientConnection");
    db.setDatabaseName("client.db");                            //저장될 DB 파일 이름 설정

    if(!db.open())  return;                                     //DB 파일 실행 예외처리

    query = new QSqlQuery(db);                                  //쿼리문을 실행할 DB를 지정

    /*client 테이블을 생성하는 쿼리문*/
    query->exec("CREATE TABLE IF NOT EXISTS client"
                "(c_id INTEGER Primary Key, "
                "name VARCHAR(100) NOT NULL, "
                "birthday VARCHAR(100) NOT NULL, "
                "phone_number VARCHAR(100) NOT NULL, "
                "address VARCHAR(100) NOT NULL, "
                "email VARCHAR(100) NOT NULL);");

    tableModel = new QSqlTableModel(this, db);                  //고객 정보를 표시할 모델 생성
    tableModel->setTable("client");                             //모델에 client 테이블 설정

    searchModel = new QSqlTableModel(this, db);                 //검색 결과를 표시할 모델 생성
    searchModel->setTable("client");                            //모델에 client 테이블 설정

    tableModel->select();                                       //db에 저장된 정보를 불러옴

    /*tableModel의 헤더를 설정*/
    tableModel->setHeaderData(0, Qt::Horizontal, tr("cid"));
    tableModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    tableModel->setHeaderData(2, Qt::Horizontal,
                              tr("birthday"));
    tableModel->setHeaderData(3, Qt::Horizontal,
                              tr("phoneNumber"));
    tableModel->setHeaderData(4, Qt::Horizontal,
                              tr("address"));
    tableModel->setHeaderData(5, Qt::Horizontal, tr("email"));

    /*searchModel의 헤더를 설정*/
    searchModel->setHeaderData(0, Qt::Horizontal, tr("cid"));
    searchModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    searchModel->setHeaderData(2, Qt::Horizontal,
                               tr("birthday"));
    searchModel->setHeaderData(3, Qt::Horizontal,
                               tr("phoneNumber"));
    searchModel->setHeaderData(4, Qt::Horizontal,
                               tr("address"));
    searchModel->setHeaderData(5, Qt::Horizontal,
                               tr("email"));

    Cui->tableView3->setModel(searchModel);                     //tableView3에 searchModel을 적용
#if 0
    for(int i = 0; i < 4; i++)
        view[i]->setModel(tableModel);                          //남은 테이블 뷰에 TableModel을 적용

    for(int i = 0; i < view.size(); i++)                        //테이블 뷰의 입력 정보에 따른 열 너비 조절
        view[i]->resizeColumnsToContents();
#else
    for(const auto &i : viewVec){
        i->setModel(tableModel);
        i->resizeColumnsToContents();
    }
#endif
}

ClientHandlerForm::~ClientHandlerForm()                         //소멸자
{
    /*생성자에서 만든 포인터 객체 소멸*/
    delete query;
    delete tableModel;
    delete searchModel;
    delete Cui;

    query = nullptr;
    tableModel = nullptr;
    searchModel = nullptr;
    Cui = nullptr;
}

/*서버 클래스의 파일 입력에 필요한 정보를 담기 위한 슬롯 함수*/
void ClientHandlerForm::dataLoad()
{
    /*서버 클래스에 고객의 이름과 ID를 담아 전송할 배열*/
    std::vector<QString> cNameVec;
    std::vector<int> cIdVec;

    /*고객의 이름과 ID를 저장할 변수 선언*/
    QString name;
    int id;

    /*현재 저장된 모든 고객의 ID와 이름을 담음*/
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        id = tableModel->record(i).value("c_id").toInt();
        name = tableModel->record(i).value("name").toString();

        cIdVec.push_back(id);
        cNameVec.push_back(name);
    }

    emit clientLoad(cIdVec, cNameVec);                        //서버 클래스의 고객 목록 입력에 필요한 시그널 방출
}

int ClientHandlerForm::makeCid() const                               //고객 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 5001;             //첫 번째 고객 ID: 5001
    else return 10;                                             //두 번째 이후는 아무 숫자
}

void ClientHandlerForm::on_enrollPushButton_clicked()           //등록 버튼을 눌렀을 때
{
    /*고객 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Cui->tableView1);
    viewVec.push_back(Cui->tableView2);
    viewVec.push_back(Cui->tableView4);
    viewVec.push_back(Cui->tableView5);

    /*고객 정보가 입력된 lineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Cui->nameLineEdit1);
    lineEditVec.push_back(Cui->birthdayLineEdit1);
    lineEditVec.push_back(Cui->phoneNumLineEdit1);
    lineEditVec.push_back(Cui->addressLineEdit1);
    lineEditVec.push_back(Cui->emailLineEdit1);

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

    std::vector<int> cIdVec;                                         //서버 클래스에 보낼 고객 ID를 담을 배열

    /*입력된 데이터 저장*/
    int cid = makeCid();
    QString name = lineEditVec[0]->text();
    QString birthday = lineEditVec[1]->text();
    QString phoneNumber = lineEditVec[2]->text();
    QString address = lineEditVec[3]->text();
    QString email = lineEditVec[4]->text();

    if(5001 == cid)                                             //첫 번째 데이터가 입력될 경우
    {
        /*client 테이블에 고객 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO client "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query->bindValue(0, cid);
        query->bindValue(1, name);
        query->bindValue(2, birthday);
        query->bindValue(3, phoneNumber);
        query->bindValue(4, address);
        query->bindValue(5, email);
        query->exec();

        tableModel->select();                                   //테이블 뷰의 정보 최신화

        /*주문 정보 클래스에 새 고객 정보가 추가 됐다는 시그널 방출*/
        emit clientAdded(cid);

        cIdVec.push_back(cid);                                         //서버 클래스에 보낼 고객 ID를 저장
    }
    else                                                        //두 번째부터 데이터가 입력될 경우
    {
        int id = tableModel->record(tableModel->rowCount()-1).  //등록될 ID 설정
                    value("c_id").toInt()+1;

        /*client 테이블에 고객 정보를 추가하는 쿼리문*/
        query->prepare("INSERT INTO client "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query->bindValue(0, id);
        query->bindValue(1, name);
        query->bindValue(2, birthday);
        query->bindValue(3, phoneNumber);
        query->bindValue(4, address);
        query->bindValue(5, email);
        query->exec();

        tableModel->select();                                   //테이블 뷰의 정보 최신화

        /*주문 정보 클래스에 새 고객 정보가 추가 됐다는 시그널 방출*/
        emit clientAdded(id);

        cIdVec.push_back(id);                                          //서버 클래스에 보낼 고객 ID를 저장
    }

    std::vector<QString> cNameVec;                                   //서버 클래스에 보낼 고객 성명을 담을 배열
    cNameVec.push_back(lineEditVec[0]->text());                           //고객 성명을 저장

    emit sendServer(cIdVec, cNameVec);                        //서버 클래스에 저장할 고객 정보 시그널 방출
#if 0
    for(int i = 0; i < viewVec.size(); i++)                        //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
    for (int i = 0 ; i < 5; i++)    lineEditVec[i]->clear();       //입력란 초기화
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }

    for(const auto &i : lineEditVec){
        i->clear();
    }
#endif
}

void ClientHandlerForm::on_searchPushButton_clicked() const          //검색 버튼을 눌렀을 때
{
    int cid = Cui->searchLineEdit->text().toInt();              //입력된 고객 ID 저장

    if(cid == 0)                                                //예외처리
    {
        Cui->searchLineEdit->clear();
        return;
    }

    searchModel->setFilter("c_id = " + QString::number(cid));   //client 테이블에 필터 설정
    searchModel->select();                                      //테이블 뷰의 정보 최신화

    Cui->tableView3->resizeColumnsToContents();                 //테이블 뷰의 입력 정보에 따른 열 너비 조절
    Cui->searchLineEdit->clear();                               //입력란 초기화
}

void ClientHandlerForm::on_resetPushButton_clicked() const           //검색 초기화 버튼을 눌렀을 때
{
    searchModel->setFilter("c_id = ''");                        //client 테이블에 필터 설정
    searchModel->select();                                      //테이블 뷰의 정보 최신화
}

void ClientHandlerForm::on_removePushButton_clicked()           //삭제 버튼을 눌렀을 때
{
    int row = Cui->tableView4->currentIndex().row();            //현재 선택된 행을 저장

    /*현재 선택된 행의 고객 정보들 저장*/
    int cid = tableModel->record(row).value("c_id").toInt();
    QString name = tableModel->record(row).value("name").
                    toString();

    /*client 테이블에서 선택된 cid와 일치하는 정보를 삭제하는 쿼리문*/
    query->prepare("DELETE FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    tableModel->select();                                       //테이블 뷰의 정보 최신화

    /*주문 정보 클래스에 고객 정보가 삭제됐다는 시그널 방출*/
    emit clientRemoved(cid);

    /*채팅 서버 클래스에 고객 정보가 삭제됐다는 시그널 방출*/
    emit sendServerCRemoved(name);
}

void ClientHandlerForm::on_modifyPushButton_clicked()           //수정 버튼을 눌렀을 때
{
    /*고객 정보가 표시될 4개의 테이블 뷰 모음*/
    std::vector<QTableView*> viewVec;
    viewVec.push_back(Cui->tableView1);
    viewVec.push_back(Cui->tableView2);
    viewVec.push_back(Cui->tableView4);
    viewVec.push_back(Cui->tableView5);

    /*수정될 정보가 입력된 LineEdit 위젯 모음*/
    std::vector<QLineEdit*> lineEditVec;
    lineEditVec.push_back(Cui->idLineEdit);
    lineEditVec.push_back(Cui->nameLineEdit2);
    lineEditVec.push_back(Cui->birthdayLineEdit2);
    lineEditVec.push_back(Cui->phoneNumLineEdit2);
    lineEditVec.push_back(Cui->addressLineEdit2);
    lineEditVec.push_back(Cui->emailLineEdit2);

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
    int cid = lineEditVec[0]->text().toInt();
    QString name = lineEditVec[1]->text();
    QString birthday = lineEditVec[2]->text();
    QString phoneNumber = lineEditVec[3]->text();
    QString address = lineEditVec[4]->text();
    QString email = lineEditVec[5]->text();

    /*client 테이블의 정보를 수정하는 쿼리문*/
    query->prepare("UPDATE client SET "
                "name = ?, birthday = ?, phone_number = ?, "
                "address = ?, email = ? "
                "WHERE c_id = ?");
    query->bindValue(0, name);
    query->bindValue(1, birthday);
    query->bindValue(2, phoneNumber);
    query->bindValue(3, address);
    query->bindValue(4, email);
    query->bindValue(5, cid);
    query->exec();

    tableModel->select();                                       //테이블 뷰의 정보 최신화

    std::vector<QString> cinfoVec;                                       //주문 정보 클래스에 보낼 고객 정보 배열
    cinfoVec.push_back(name);
    cinfoVec.push_back(phoneNumber);
    cinfoVec.push_back(address);                                   //고객 정보를 담음

    emit clientModified(cid, cinfoVec);                            //주문 정보 클래스에 고객 정보가 수정됐다는 시그널 방출
    emit sendServerCModified(cid, name);                        //서버 클래스에 고객 정보가 수정됐다는 시그널 방출

#if 0
    for(int i = 0; i < viewVec.size(); i++)                        //테이블 뷰의 입력 정보에 따른 열 너비 조절
        viewVec[i]->resizeColumnsToContents();
    for (int i = 0 ; i < 6; i++)    lineEditVec[i]->clear();       //입력란 초기화
#else
    for(const auto &i : viewVec){
        i->resizeColumnsToContents();
    }

    for(const auto &i : lineEditVec){
        i->clear();
    }
#endif


}

/*현재 고객 정보를 입력란에 채워주는 슬롯함수*/
void ClientHandlerForm::on_tableView5_clicked(const QModelIndex &index) const
{
    int row = index.row();                                      //현재 선택된 행을 저장

    /*현재 선택된 행의 정보를 저장*/
    int cid = tableModel->record(row).value("c_id").
            toInt();
    QString name = tableModel->record(row).value("name").
            toString();
    QString birthday = tableModel->record(row).
            value("birthday").toString();
    QString phoneNumber = tableModel->record(row).
            value("phone_number").toString();
    QString address = tableModel->record(row).
            value("address").toString();
    QString email = tableModel->record(row).value("email").
            toString();

    /*현재 선택된 행의 정보를 입력란에 채워줌*/
    Cui->idLineEdit->setText(QString::number(cid));
    Cui->nameLineEdit2->setText(name);
    Cui->birthdayLineEdit2->setText(birthday);
    Cui->phoneNumLineEdit2->setText(phoneNumber);
    Cui->addressLineEdit2->setText(address);
    Cui->emailLineEdit2->setText(email);
}

/*새로운 주문 정보를 등록할 경우 고객 정보를 담아서 보내주는 슬롯함수*/
void ClientHandlerForm::orderAddedClient(int cid)
{
    std::vector<QString> cinfoVec;                                       //고객 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 고객 정보만 가져오는 쿼리문*/
    query->prepare("SELECT name, phone_number, address "
                   "FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int nameColIdx = rec.indexOf("name");
    int phoneNumColIdx = rec.indexOf("phone_number");
    int addressColIdx = rec.indexOf("address");

    query->next();                                              //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장*/
    QString name= query->value(nameColIdx).toString();
    QString phoneNum = query->value(phoneNumColIdx).toString();
    QString address = query->value(addressColIdx).toString();

    cinfoVec.push_back(name);
    cinfoVec.push_back(phoneNum);
    cinfoVec.push_back(address);
    emit addReturn(cinfoVec);                                      //담은 고객 정보를 시그널로 방출
}

/*주문 정보 클래스에서 검색할 경우 필요한 고객 정보를 담아서 보내주는 슬롯함수*/
void ClientHandlerForm::orderSearchedClient(int cid)
{
    std::vector<QString> cinfoVec;                                       //고객 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 고객 정보만 가져오는 쿼리문*/
    query->prepare("SELECT name, phone_number, address "
                   "FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int nameColIdx = rec.indexOf("name");
    int phoneNumColIdx = rec.indexOf("phone_number");
    int addressColIdx = rec.indexOf("address");

    query->next();                                              //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장*/
    QString name= query->value(nameColIdx).toString();
    QString phoneNum = query->value(phoneNumColIdx).toString();
    QString address = query->value(addressColIdx).toString();

    cinfoVec.push_back(name);
    cinfoVec.push_back(phoneNum);
    cinfoVec.push_back(address);

    emit searchReturn(cinfoVec);                                   //담은 고객 정보를 시그널로 방출
}

/*주문 정보 클래스에서 주문 정보를 수정할 경우 필요한 고객 정보를 담아서 보내주는 슬롯함수*/
void ClientHandlerForm::orderModifiedClient(int cid, int row)
{
    std::vector<QString> cinfoVec;                                       //고객 정보를 담을 배열

    /*주문 정보 클래스에 보내줄 고객 정보만 가져오는 쿼리문*/
    query->prepare("SELECT name, phone_number, address "
                   "FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    /*각 정보의 열 번호를 저장*/
    QSqlRecord rec = query->record();
    int nameColIdx = rec.indexOf("name");
    int phoneNumColIdx = rec.indexOf("phone_number");
    int addressColIdx = rec.indexOf("address");

    query->next();                                              //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장*/
    QString name= query->value(nameColIdx).toString();
    QString phoneNum = query->value(phoneNumColIdx).toString();
    QString address = query->value(addressColIdx).toString();

    cinfoVec.push_back(name);
    cinfoVec.push_back(phoneNum);
    cinfoVec.push_back(address);

    emit modifyReturn(cinfoVec, row);                              //담은 고객 정보를 시그널로 방출
}

/*주문 정보 클래스의 고객 정보 관련 콤보박스 채우기*/
void ClientHandlerForm::setClientComboBox(QComboBox* CidBox, QComboBox* CinfoBox) const
{
    /*콤보박스를 채우기 위해 필요한 고객 정보를 나타내는 변수 선언*/
    int id;
    QString name, phoneNum;

    /*기존에 콤보박스에 저장된 고객 정보를 모두 삭제*/
    CidBox->clear();
    CinfoBox->clear();

    /*"선택하세요" 초기값 추가*/
    CidBox->addItem(tr("select item"));
    CinfoBox->addItem(tr("select item"));

    for(int i = 0; i < tableModel->rowCount(); i++)             //db에 저장된 고객 정보의 수만큼 반복
    {
        id = tableModel->record(i).value("c_id").toInt();       //고객 id 추출
        name = tableModel->record(i).value("name").toString();  //고객 성명 추출
        phoneNum = tableModel->record(i).value("phone_number"). //고객 전화번호 추출
                toString();
        CidBox->addItem(QString::number(id));                   //고객 id 추가
        CinfoBox->addItem(name + "(" + phoneNum + ")");         //중복되지 않도록 고객 정보 추가
    }
}
