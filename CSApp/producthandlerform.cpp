#include "producthandlerform.h"
#include "ui_producthandlerform.h"
#include <QList>
#include <QComboBox>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

ProductHandlerForm::ProductHandlerForm(QWidget *parent) :                   //생성자
    QWidget(parent),
    Pui(new Ui::ProductHandlerForm)
{
    Pui->setupUi(this);                                                     //현재 클래스에 UI를 세팅

    QVector<QTableView*> view;
    view << Pui->tableView1 << Pui->tableView2
         << Pui->tableView4 << Pui->tableView5;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "productConnection");
    db.setDatabaseName("product.db");

    if(!db.open())
    {
        return;
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE IF NOT EXISTS product"
                "(p_id INTEGER Primary Key, "
                "name VARCHAR(100) NOT NULL, "
                "price INTEGER NOT NULL, "
                "sort VARCHAR(100) NOT NULL);");

    tableModel = new QSqlTableModel(this, db);
    tableModel->setTable("product");

    searchModel = new QSqlTableModel(this, db);
    searchModel->setTable("product");

    tableModel->select();
    tableModel->setHeaderData(0, Qt::Horizontal, tr("pid"));
    tableModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    tableModel->setHeaderData(2, Qt::Horizontal, tr("price"));
    tableModel->setHeaderData(3, Qt::Horizontal, tr("sort"));

    searchModel->setHeaderData(0, Qt::Horizontal, tr("pid"));
    searchModel->setHeaderData(1, Qt::Horizontal, tr("name"));
    searchModel->setHeaderData(2, Qt::Horizontal, tr("price"));
    searchModel->setHeaderData(3, Qt::Horizontal, tr("sort"));

    for(int i = 0; i < 4; i++)
        view[i]->setModel(tableModel);

    Pui->tableView3->setModel(searchModel);

}

ProductHandlerForm::~ProductHandlerForm()                           //소멸자
{
    delete Pui;                                                     //생성자에서 만든 포인터 객체 소멸
}

int ProductHandlerForm::makePid()                                   //제품 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 1001;                 //첫 번째 제품 ID: 1001
    else return 10;                                                 //두 번째 이후는 아무 숫자
}

void ProductHandlerForm::setProductComboBox(QComboBox* PidBox, QComboBox* PinfoBox)
{                                                                   //주문 정보 클래스의 제품 정보 콤보박스 채우기
    int id;
    QString name;
    QString sort;

    PidBox->clear();
    PinfoBox->clear();

    PidBox->addItem(tr("select item"));
    PinfoBox->addItem(tr("select item"));

    for(int i = 0; i < tableModel->rowCount(); i++)                 //db에 저장된 제품 정보의 수만큼 반복
    {
        id = tableModel->record(i).value("p_id").toInt();            //제품 id 추출
        name = tableModel->record(i).value("name").toString();      //제품 성명 추출
        sort = tableModel->record(i).value("sort").                 //제품 종류 추출
                toString();

        if(PidBox->findText(QString::number(id)) < 0)               //id 콤보박스에 중복되지 않도록
            PidBox->addItem(QString::number(id));                   //제품 id 추가

        if(PinfoBox->findText(name + "(" + sort + ")") < 0)         //제품명(제품종류) 콤보박스에
            PinfoBox->addItem(name + "(" + sort + ")");             //중복되지 않도록 제품 정보 추가
    }
}


void ProductHandlerForm::on_enrollPushButton_clicked()              //등록 버튼을 눌렀을 때
{
    /*제품 정보가 표시될 4개의 테이블 뷰 모음*/
    QVector<QTableView*> view;
    view << Pui->tableView1 << Pui->tableView2
         << Pui->tableView4 << Pui->tableView5;

    /*제품 정보가 입력된 lineEdit 위젯 모음*/
    QVector<QLineEdit*> lineEdit;
    lineEdit << Pui->nameLineEdit1 << Pui->priceLineEdit1
             << Pui->sortLineEdit1;

    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
    }

    int pid = makePid();
    QString name = lineEdit[0]->text();
    int price = lineEdit[1]->text().toInt();
    QString sort = lineEdit[2]->text();

    if(1001 == pid)
    {
        query->prepare("INSERT INTO product VALUES (?, ?, ?, ?)");
        query->bindValue(0, pid);
        query->bindValue(1, name);
        query->bindValue(2, price);
        query->bindValue(3, sort);
        query->exec();
        tableModel->select();

        /*주문 정보 클래스에 새 제품 정보가 추가 됐다는 시그널 방출*/
        emit productAdded(pid);
    }
    else
    {
        int id = tableModel->record(tableModel->rowCount()-1).
                    value("p_id").toInt()+1;
        query->prepare("INSERT INTO product VALUES (?, ?, ?, ?)");
        query->bindValue(0, id);
        query->bindValue(1, name);
        query->bindValue(2, price);
        query->bindValue(3, sort);
        query->exec();
        tableModel->select();

        /*주문 정보 클래스에 새 제품 정보가 추가 됐다는 시그널 방출*/
        emit productAdded(id);
    }

    for(int i = 0; i < view.size(); i++)                            //테이블 뷰의 입력 정보에 따른 열 크기 조절
        view[i]->resizeColumnsToContents();

    for (int i = 0 ; i < lineEdit.size(); i++)    lineEdit[i]->clear();           //입력란 초기화
}

void ProductHandlerForm::on_searchPushButton_clicked()              //검색 버튼을 눌렀을 때
{
    int pid = Pui->searchLineEdit->text().toInt();

    searchModel->setFilter("p_id = " + QString::number(pid));
    searchModel->select();

    Pui->searchLineEdit->clear();                                   //입력란 초기화
}

void ProductHandlerForm::on_resetPushButton_clicked()
{
    searchModel->setFilter("p_id = ''");
    searchModel->select();
}

void ProductHandlerForm::on_removePushButton_clicked()              //삭제 버튼을 눌렀을 때
{
    qDebug() <<Pui->tableView4->currentIndex().isValid();
    if(!Pui->tableView4->currentIndex().isValid())   return;

    int row = Pui->tableView4->currentIndex().row();
    int pid = tableModel->record(row).value("p_id").toInt();
    QString sort = tableModel->record(row).value("sort").toString();
    QString name = tableModel->record(row).value("name").toString();

    query->prepare("DELETE FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    tableModel->select();

    /*주문 정보 클래스에 제품 정보가 삭제됐다는 시그널 방출*/
    emit productRemoved(pid, sort, name);

}

void ProductHandlerForm::on_modifyPushButton_clicked()              //수정 버튼을 눌렀을 때
{
    /*제품 정보가 표시될 4개의 테이블 뷰 모음*/
    QVector<QTableView*> view;
    view << Pui->tableView1 << Pui->tableView2
         << Pui->tableView4 << Pui->tableView5;

    /*수정될 정보가 입력된 lineEdit 위젯 모음*/
    QVector<QLineEdit*> lineEdit;
    lineEdit << Pui->idLineEdit << Pui->nameLineEdit2
             << Pui->priceLineEdit2 << Pui->sortLineEdit2;

    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
    }

    int pid = lineEdit[0]->text().toInt();
    QString name = lineEdit[1]->text();
    int price = lineEdit[2]->text().toInt();
    QString sort = lineEdit[3]->text();

    query->prepare("UPDATE product SET "
                   "name = ?, price = ?, sort = ? "
                   "WHERE p_id = ?");
    query->bindValue(0, name);
    query->bindValue(1, price);
    query->bindValue(2, sort);
    query->bindValue(3, pid);
    query->exec();
    tableModel->select();

    QList<QString> pinfo;                                           //주문 정보 클래스에 보낼 제품 정보 배열
    pinfo << sort << name << QString::number(price);                //제품 정보를 담음

    /*주문 정보 클래스에 제품 정보가 수정됐다는 시그널 방출*/
    emit productModified(pid, pinfo);

    for (int i = 0 ; i < lineEdit.size(); i++)                      //입력란 초기화
        lineEdit[i]->clear();
}

/*현재 제품 정보를 입력란에 채워주는 슬롯함수*/
void ProductHandlerForm::on_tableView5_clicked(const QModelIndex &index)
{
    int row = index.row();

    int pid = tableModel->record(row).value("p_id").
            toInt();
    QString name = tableModel->record(row).value("name").
            toString();
    int price = tableModel->record(row).value("price").
            toInt();
    QString sort = tableModel->record(row).value("sort").
            toString();

    Pui->idLineEdit->setText(QString::number(pid));
    Pui->nameLineEdit2->setText(name);
    Pui->priceLineEdit2->setText(QString::number(price));
    Pui->sortLineEdit2->setText(sort);
}

/*새로운 주문 정보를 등록할 경우 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderAddedProduct(int pid)
{
    QList<QString> pinfo;                                           //제품 정보를 담을 배열

    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfo << sort << name << QString::number(price);
    emit addReturn(pinfo);                                          //담은 제품 정보를 시그널로 방출
}

/*주문 정보 클래스에서 검색할 경우 필요한 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderSearchedProduct(int pid)
{
    QList<QString> pinfo;                                           //제품 정보를 담을 배열

    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfo << sort << name << QString::number(price);
    emit searchReturn(pinfo);                                       //담은 제품 정보를 시그널로 방출
}

/*주문 정보 클래스에서 주문 정보를 수정할 경우 필요한 제품 정보를 담아서 보내주는 슬롯함수*/
void ProductHandlerForm::orderModifiedProduct(int pid, int row)
{
    QList<QString> pinfo;                                           //제품 정보를 담을 배열

    query->prepare("SELECT sort, name, price "
                   "FROM product WHERE p_id = ?");
    query->bindValue(0, pid);
    query->exec();

    QSqlRecord rec = query->record();
    int sortColIdx = rec.indexOf("sort");
    int nameColIdx = rec.indexOf("name");
    int priceColIdx = rec.indexOf("price");

    query->next();                                                  //쿼리의 결과값으로 이동

    /*주문 정보 클래스로 보낼 고객 정보를 저장할 변수*/
    QString sort= query->value(sortColIdx).toString();
    QString name = query->value(nameColIdx).toString();
    int price = query->value(priceColIdx).toInt();

    pinfo << sort << name << QString::number(price);

    emit modifyReturn(pinfo, row);                                  //담은 제품 정보를 시그널로 방출
}
