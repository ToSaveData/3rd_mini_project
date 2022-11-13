#include "clienthandlerform.h"
#include "ui_clienthandlerform.h"
#include <QList>
#include <QComboBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

ClientHandlerForm::ClientHandlerForm(QWidget *parent) :         //생성자
    QWidget(parent), Cui(new Ui::ClientHandlerForm)
{
    Cui->setupUi(this);                                         //현재 클래스에 UI를 세팅

    QVector<QTableView*> view;                                  //모델 설정이 필요한 테이블 뷰 모음
    view << Cui->tableView1 << Cui->tableView2
         << Cui->tableView4 << Cui->tableView5;

//    QSqlDatabase db = QSqlDatabase::database();
    QSqlDatabase db = QSqlDatabase::addDatabase                 //QSQLITE DB에 연결명을 설정하고 추가
                        ("QSQLITE", "clientConnection");
    db.setDatabaseName("client.db");                            //DB가 저장도리 파일 이름을 설정

    if(!db.open())  return;                                     //DB 파일 실행 예외처리

    query = new QSqlQuery(db);                                  //쿼리문을 실행할 DB를 지정
    query->exec("CREATE TABLE IF NOT EXISTS client"             //테이블 생성
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

    tableModel->select();
    tableModel->setHeaderData(0, Qt::Horizontal, tr("cid"));
    tableModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    tableModel->setHeaderData(2, Qt::Horizontal,
                              tr("birthday"));
    tableModel->setHeaderData(3, Qt::Horizontal,
                              tr("phoneNumber"));
    tableModel->setHeaderData(4, Qt::Horizontal,
                              tr("address"));
    tableModel->setHeaderData(5, Qt::Horizontal, tr("email"));

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

    for(int i = 0; i < 4; i++)
        view[i]->setModel(tableModel);

    Cui->tableView3->setModel(searchModel);
}

ClientHandlerForm::~ClientHandlerForm()                         //소멸자
{
    delete Cui;                                                 //생성자에서 만든 포인터 객체 소멸
}

void ClientHandlerForm::dataLoad()                              //서버 클래스의 파일 입력에 필요한 정보를
{                                                               //담기 위한 슬롯 함수
    QList<QString> cNameList;
    QList<int> cIdList;

    QString name;
    int id;

    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        id = tableModel->record(i).value("c_id").toInt();
        name = tableModel->record(i).value("name").toString();

        cIdList << id;
        cNameList << name;
    }

    emit clientLoad(cIdList, cNameList);                        //서버 클래스의 고객 목록 입력에 필요한 시그널 방출
}

int ClientHandlerForm::makeCid()                                 //고객 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 5001;             //첫 번째 고객 ID: 5001
    else return 10;                                             //두 번째 이후는 아무 숫자
}

void ClientHandlerForm::setClientComboBox(QComboBox* CidBox, QComboBox* CinfoBox)
{                                                               //주문 정보 클래스의 고객 정보 콤보박스 채우기
    int id;
    QString name, phoneNum;

    CidBox->clear();
    CinfoBox->clear();

    CidBox->addItem(tr("select item"));
    CinfoBox->addItem(tr("select item"));

    query->exec("SELECT c_id FROM client;");

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

void ClientHandlerForm::on_enrollPushButton_clicked()
{
    /*고객 정보가 표시될 4개의 테이블 뷰 모음*/
    QVector<QTableView*> view;
    view << Cui->tableView1 << Cui->tableView2
         << Cui->tableView4 << Cui->tableView5;

    /*고객 정보가 입력된 lineEdit 위젯 모음*/
    QVector<QLineEdit*> lineEdit;
    lineEdit << Cui->nameLineEdit1 << Cui->birthdayLineEdit1
             << Cui->phoneNumLineEdit1 << Cui->addressLineEdit1
             << Cui->emailLineEdit1;

    /*입력된 데이터가 하나라도 없을 경우 등록하지 않음*/
    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
    }

    QList<int> cIdInfo;                                         //서버 클래스에 보낼 고객 ID를 담을 배열

    /*입력된 데이터 저장*/
    int cid = makeCid();
    QString name = lineEdit[0]->text();
    QString birthday = lineEdit[1]->text();
    QString phoneNumber = lineEdit[2]->text();
    QString address = lineEdit[3]->text();
    QString email = lineEdit[4]->text();

    if(5001 == cid)                                             //첫 번째 데이터가 입력될 경우
    {
        query->prepare("INSERT INTO client "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query->bindValue(0, cid);
        query->bindValue(1, name);
        query->bindValue(2, birthday);
        query->bindValue(3, phoneNumber);
        query->bindValue(4, address);
        query->bindValue(5, email);
        query->exec();
        tableModel->select();

        /*주문 정보 클래스에 새 고객 정보가 추가 됐다는 시그널 방출*/
        emit clientAdded(cid);

        cIdInfo << cid;                                             //서버 클래스에 보낼 고객 ID를 저장
    }
    else                                                            //두 번째부터 데이터가 입력될 경우
    {
        int id = tableModel->record(tableModel->rowCount()-1).      //등록될 ID 설정
                    value("c_id").toInt()+1;
        query->prepare("INSERT INTO client "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query->bindValue(0, id);
        query->bindValue(1, name);
        query->bindValue(2, birthday);
        query->bindValue(3, phoneNumber);
        query->bindValue(4, address);
        query->bindValue(5, email);
        query->exec();
        tableModel->select();

        /*주문 정보 클래스에 새 고객 정보가 추가 됐다는 시그널 방출*/
        emit clientAdded(id);

        cIdInfo << id;                                              //서버 클래스에 보낼 고객 ID를 저장
    }

    QList<QString> cNameInfo;                                       //서버 클래스에 보낼 고객 성명을 담을 배열
    cNameInfo << lineEdit[0]->text();                               //고객 성명을 저장

    emit sendServer(cIdInfo, cNameInfo);                            //서버 클래스에 저장할 고객 정보 시그널 방출

    for(int i = 0; i < view.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 크기 조절
        view[i]->resizeColumnsToContents();

    for (int i = 0 ; i < 5; i++)    lineEdit[i]->clear();           //입력란 초기화
}

void ClientHandlerForm::on_searchPushButton_clicked()               //검색 버튼을 눌렀을 때
{
    int cid = Cui->searchLineEdit->text().toInt();

    searchModel->setFilter("c_id = " + QString::number(cid));
    searchModel->select();

    Cui->searchLineEdit->clear();
}

void ClientHandlerForm::on_resetPushButton_clicked()                //검색 기록을 초기화하는 버튼
{
    searchModel->setFilter("c_id = ''");
    searchModel->select();
}

void ClientHandlerForm::on_removePushButton_clicked()               //삭제 버튼을 눌렀을 때
{
    int row = Cui->tableView4->currentIndex().row();
    int cid = tableModel->record(row).value("c_id").toInt();
    QString name = tableModel->record(row).value("name").
                    toString();
    QString phoneNum = tableModel->record(row).
                        value("phone_number").toString();

    query->prepare("DELETE FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();
    tableModel->select();

    emit clientRemoved(cid, name, phoneNum);                        //주문 정보 클래스에 고객 정보가
                                                                    //삭제됐다는 시그널 방출

    emit sendServerCRemoved(name);                                  //채팅 서버 클래스에 고객 정보가
                                                                    //삭제됐다는 시그널 방출
}


void ClientHandlerForm::on_modifyPushButton_clicked()               //수정 버튼을 눌렀을 때
{
    /*고객 정보가 표시될 4개의 테이블 뷰 모음*/
    QVector<QTableView*> view;
    view << Cui->tableView1 << Cui->tableView2
         << Cui->tableView4 << Cui->tableView5;

    /*수정될 정보가 입력된 LineEdit 위젯 모음*/
    QVector<QLineEdit*> lineEdit;
    lineEdit << Cui->idLineEdit << Cui->nameLineEdit2
             << Cui->birthdayLineEdit2 << Cui->phoneNumLineEdit2
             << Cui->addressLineEdit2 << Cui->emailLineEdit2;

    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
    }

    int cid = lineEdit[0]->text().toInt();
    QString name = lineEdit[1]->text();
    QString birthday = lineEdit[2]->text();
    QString phoneNumber = lineEdit[3]->text();
    QString address = lineEdit[4]->text();
    QString email = lineEdit[5]->text();

    query->prepare("UPDATE client SET "
                "name = ?, birthday = ?, "
                "phone_number = ?, address = ?, "
                "email = ? "
                "WHERE c_id = ?");
    query->bindValue(0, name);
    query->bindValue(1, birthday);
    query->bindValue(2, phoneNumber);
    query->bindValue(3, address);
    query->bindValue(4, email);
    query->bindValue(5, cid);
    query->exec();
    tableModel->select();

    QList<QString> cinfo;                                           //주문 정보 클래스에 보낼 고객 정보 배열
    cinfo << name << phoneNumber << address;                        //고객 정보를 담음
    emit clientModified(cid, cinfo);                                //주문 정보 클래스에 고객 정보가 수정됐다는 시그널 방출
    emit sendServerCModified(cid, name);                            //서버 클래스에 고객 정보가 수정됐다는 시그널 방출

    for(int i = 0; i < view.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 크기 조절
        view[i]->resizeColumnsToContents();

    for (int i = 0 ; i < 6; i++)    lineEdit[i]->clear();           //입력란 초기화
}

/*현재 고객 정보를 입력란에 채워주는 슬롯함수*/
void ClientHandlerForm::on_tableView5_clicked(const QModelIndex &index)
{
    int row = index.row();

    int cid = tableModel->record(row).value("c_id").
            toInt();
    QString name = tableModel->record(row).value("name").
            toString();
    QString birthday = tableModel->record(row).value("birthday").
            toString();
    QString phoneNumber = tableModel->record(row).
            value("phone_number").toString();
    QString address = tableModel->record(row).value("address").
            toString();
    QString email = tableModel->record(row).value("email").
            toString();

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
    QList<QString> cinfo;                                       //고객 정보를 담을 배열

    query->prepare("SELECT name, phone_number, address "
                   "FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    QSqlRecord rec = query->record();
    int nameColIdx = rec.indexOf("name");
    int phoneNumColIdx = rec.indexOf("phone_number");
    int addressColIdx = rec.indexOf("address");

    query->next();
    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString name= query->value(nameColIdx).toString();
    QString phoneNum = query->value(phoneNumColIdx).toString();
    QString address = query->value(addressColIdx).toString();

    cinfo << name << phoneNum << address;
    emit addReturn(cinfo);                                      //담은 고객 정보를 시그널로 방출
}

/*주문 정보 클래스에서 검색할 경우 필요한 고객 정보를 담아서 보내주는 슬롯함수*/
void ClientHandlerForm::orderSearchedClient(int cid)
{
    QList<QString> cinfo;                                       //고객 정보를 담을 배열

    query->prepare("SELECT name, phone_number, address "
                   "FROM client WHERE c_id = ?");
    query->bindValue(0, cid);
    query->exec();

    QSqlRecord rec = query->record();
    int nameColIdx = rec.indexOf("name");
    int phoneNumColIdx = rec.indexOf("phone_number");
    int addressColIdx = rec.indexOf("address");

    query->next();
    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString name= query->value(nameColIdx).toString();
    QString phoneNum = query->value(phoneNumColIdx).toString();
    QString address = query->value(addressColIdx).toString();

    cinfo << name << phoneNum << address;
    emit searchReturn(cinfo);                                   //담은 고객 정보를 시그널로 방출
}

/*주문 정보 클래스에서 주문 정보를 수정할 경우 필요한 고객 정보를 담아서 보내주는 슬롯함수*/
void ClientHandlerForm::orderModifiedClient(int cid, int row)
{
    QList<QString> cinfo;                                       //고객 정보를 담을 배열

    /*현재 id와 일치하는 필터를 적용*/
    tableModel->setFilter("c_id = " + QString::number(cid));
    tableModel->select();

    /*현재 id와 일치하는 고객 정보 저장*/
    QString name = tableModel->record(0).value("name").
            toString();
    QString phoneNum = tableModel->record(0).value("phone_number").
            toString();
    QString address = tableModel->record(0).value("address").
            toString();

    /*모든 정보를 표시하도록 필터 재설정*/
    tableModel->setFilter("c_id LIKE'%0%'");
    tableModel->select();

    cinfo << name << phoneNum << address;
    emit modifyReturn(cinfo, row);                              //담은 고객 정보를 시그널로 방출
}
