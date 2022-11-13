#include "orderhandlerform.h"
#include "ui_orderhandlerform.h"
#include <QList>
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


OrderHandlerForm::OrderHandlerForm(QWidget *parent) :                       //생성자
    QWidget(parent), Oui(new Ui::OrderHandlerForm)
{
    Oui->setupUi(this);                                             //현재 클래스에 UI를 세팅
    QVector<QTableView*> view;
    view << Oui->tableView1 << Oui->tableView2
         << Oui->tableView4 << Oui->tableView5;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "orderConnection");
    db.setDatabaseName("order.db");

    if(!db.open())  return;

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE IF NOT EXISTS order_info "
                "(o_id INTEGER Primary Key, "
                "order_date VARCHAR(100) NOT NULL, "
                "order_quantity INTEGER NOT NULL, "
                "c_id INTEGER NOT NULL, "
                "p_id INTEGER NOT NULL);");

    tableModel = new QSqlTableModel(this, db);
    tableModel->setTable("order_info");

    detailModel = new QStandardItemModel(0, 10);

    searchModel = new QStandardItemModel(0, 10);

    tableModel->setHeaderData(0, Qt::Horizontal, tr("oid"));
    tableModel->setHeaderData(1, Qt::Horizontal,
                              tr("order_date"));
    tableModel->setHeaderData(2, Qt::Horizontal,
                              tr("order_quantity"));
    tableModel->setHeaderData(3, Qt::Horizontal, tr("cid"));
    tableModel->setHeaderData(4, Qt::Horizontal, tr("pid"));

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

    view[0]->setModel(tableModel);
    for(int i = 1; i < 4; i++)
        view[i]->setModel(detailModel);

    Oui->tableView3->setModel(searchModel);

//    if(tableModel->rowCount() > 0)
//        cnt = tableModel->rowCount();
}

OrderHandlerForm::~OrderHandlerForm()                               //소멸자
{
    delete Oui;                                                     //생성자에서 만든 포인터 객체 소멸
}

void OrderHandlerForm::dataload()                                   //주문 정보 클래스에서 채울 수 없는
{                                                                   //고객 정보, 제품 정보를 시그널로 불러서 채우는 함수

    emit clientComboBox(Oui->clientIDComboBox1,                     //고객 정보와 관련된 콤보박스를 시그널로 방출
                        Oui->clientInfoComboBox);
    emit productComboBox(Oui->productIDComboBox1,                   //제품 정보와 관련된 콤보박스를 시그널로 방출
                         Oui->productInfoComboBox);
}

int OrderHandlerForm::makeOid()                                     //주문 ID를 생성하는 함수
{
    if(0 == tableModel->rowCount())    return 100001;               //첫 번째 주문 ID: 100001
    else return 10;                                                 //두 번째 이후는 아무 숫자
}

void OrderHandlerForm::on_enrollPushButton_clicked()                //등록 버튼을 눌렀을 때
{
    QVector<QLineEdit*> lineEdit;
    lineEdit << Oui->orderDateLineEdit1
             << Oui->orderQuantityLineEdit1;
    QVector<QComboBox*> comboBox;
    comboBox << Oui->clientIDComboBox1 << Oui->productIDComboBox1;

    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
        if(comboBox[i]->currentText() == tr("select item"))
            return;
    }

    int oid = makeOid();                                            //주문 id 생성 후 저장

    /*입력된 내용을 각 변수에 맞게 저장*/
    QString date = Oui->orderDateLineEdit1->text();
    int quantity = Oui->orderQuantityLineEdit1->text().toInt();
    int cid = Oui->clientIDComboBox1->currentText().toInt();
    int pid = Oui->productIDComboBox1->currentText().toInt();

    if(100001 == oid)                                               //첫 번째 데이터 추가 시
    {
        /*첫 번째 테이블 뷰에 데이터를 추가하는 쿼리*/
        query->exec("INSERT INTO order_info VALUES ("+
                    QString::number(oid) + ", '" +
                    date + "', " +
                    QString::number(quantity) + ", " +
                    QString::number(cid) + ", " +
                    QString::number(pid) + ");");

        tableModel->select();                                       //데이터 새로 고침

        /*detailMoedl에 들어갈 데이터들*/
        QStringList str;
        str << QString::number(oid) << date
            << QString::number(quantity);

        /*QStandardItem*로 자료형 변환*/
        QList<QStandardItem*> items;
        for(int i = 0; i < str.size(); i++)
        {
            items << new QStandardItem(str[i]);
        }

        /*detailMoedl에 데이터 추가*/
        detailModel->appendRow(items[0]);
        detailModel->setItem(cnt, 1, items[1]);
        detailModel->setItem(cnt, 8, items[2]);
    }
    else
    {
        int id = tableModel->record(tableModel->rowCount()-1).      //두번째 이후 데이터를 추가할 경우
                    value("o_id").toInt()+1;

        /*첫 번째 테이블 뷰에 데이터를 추가하는 쿼리*/
        query->exec("INSERT INTO order_info VALUES ("+
                    QString::number(id) + ", '" +
                    date + "', " +
                    QString::number(quantity) + ", " +
                    QString::number(cid) + ", " +
                    QString::number(pid) + ");");
        tableModel->select();                                       //데이터 새로 고침

        /*detailMoedl에 들어갈 데이터들*/
        QStringList str;
        str << QString::number(id) << date
            << QString::number(quantity);

        /*QStandardItem*로 자료형 변환*/
        QList<QStandardItem*> items;
        for(int i = 0; i < str.size(); i++)
        {
            items << new QStandardItem(str[i]);
        }

        /*detailMoedl에 데이터 추가*/
        detailModel->appendRow(items[0]);
        detailModel->setItem(cnt, 1, items[1]);
        detailModel->setItem(cnt, 8, items[2]);
    }

    /*주문정보가 추가됐다는 시그널을 고객 정보 클래스와 제품 정보 클래스에 방출*/
    emit orderAddedClient(cid);
    emit orderAddedProduct(pid);

    /*입력란 초기화*/
    Oui->orderDateLineEdit1->clear();
    Oui->orderQuantityLineEdit1->clear();

    /*콤보박스 상태 초기화*/
    Oui->clientIDComboBox1->setCurrentText(tr("select item"));
    Oui->productIDComboBox1->setCurrentText(tr("select item"));
}

void OrderHandlerForm::addReturnClient(QList<QString> cinfo)        //등록 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                   //테이블 위젯을 채우는 슬롯함수

    //int oid = tableModel->record(cnt).value("o_id").toInt();

    /*넘어온 고객 정보를 각 변수로 저장 */
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << name << phoneNum << address;

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(cnt, 2, items[0]);
    detailModel->setItem(cnt, 3, items[1]);
    detailModel->setItem(cnt, 4, items[2]);
}

void OrderHandlerForm::addReturnProduct(QList<QString> pinfo)       //등록 탭에서 보낸 시그널로 넘어온 제품 정보로
{                                                                   //테이블 위젯을 채우는 슬롯함수

//    int oid = tableModel->record(cnt).value("o_id").
//            toInt();
    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();

    /*detailModel에 필요한 기존 정보 저장*/
    int quantity = tableModel->record(cnt).
                    value("order_quantity").toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << sort << name << QString::number(price)
        <<QString::number(quantity) << QString::number(totalPrice);

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(cnt, 5, items[0]);
    detailModel->setItem(cnt, 6, items[1]);
    detailModel->setItem(cnt, 7, items[2]);
    detailModel->setItem(cnt, 8, items[3]);
    detailModel->setItem(cnt, 9, items[4]);

    cnt++;                                                          //다음 행 입력을 위한 cnt 증가
}

void OrderHandlerForm::on_searchPushButton_clicked()                //검색 버튼을 눌렀을 때
{
    searchModel->removeRows(0,searchModel->rowCount());             //이전 검색 데이터 초기화

    int oid = Oui->searchLineEdit->text().toInt();                  //입력된 주문 id 저장

    /*현재 주문 id와 일치하는 필터를 적용*/
    tableModel->setFilter("o_id = " + QString::number(oid));
    tableModel->select();

    /*현재 주문 id와 일치하는 고객 정보 저장*/
    QString date = tableModel->record(0).value("order_date").
                    toString();
    int quantity = tableModel->record(0).value("order_quantity").
                    toInt();
    int cid = tableModel->record(0).value("c_id").toInt();
    int pid = tableModel->record(0).value("p_id").toInt();

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << QString::number(oid) << date
        << QString::number(quantity);

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    searchModel->appendRow(items[0]);
    searchModel->setItem(0, 1, items[1]);
    searchModel->setItem(0, 8, items[2]);

    emit orderSearchedClient(cid);                                  //고객 정보 클래스에 고객 정보를
                                                                    //요청하는 시그널 방출
    emit orderSearchedProduct(pid);                                 //제품 정보 클래스에 제품 정보를
                                                                    //요청하는 시그널 방출

    Oui->searchLineEdit->clear();                                   //입력란 초기화
}

void OrderHandlerForm::on_resetPushButton_clicked()                 //초기화 버튼을 눌렀을 때
{
    searchModel->removeRows(0,searchModel->rowCount());             //검색 결과 초기화
}

void OrderHandlerForm::searchReturnClient(QList<QString> cinfo)     //검색 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                   //테이블 위젯을 채우는 슬롯함수
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << name << phoneNum << address;

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    searchModel->setItem(0, 2, items[0]);
    searchModel->setItem(0, 3, items[1]);
    searchModel->setItem(0, 4, items[2]);
}

void OrderHandlerForm::searchReturnProduct(QList<QString> pinfo)    //검색 탭에서 보낸 시그널로 넘어온 제품 정보로
{                                                                   //테이블 위젯을 채우는 슬롯함수
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int quantity = tableModel->record(0).
                    value("order_quantity").toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << sort << name << QString::number(price)
        <<QString::number(quantity) << QString::number(totalPrice);

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    searchModel->setItem(0, 5, items[0]);
    searchModel->setItem(0, 6, items[1]);
    searchModel->setItem(0, 7, items[2]);
    searchModel->setItem(0, 8, items[3]);
    searchModel->setItem(0, 9, items[4]);

    /*모든 정보를 표시하도록 tableModel의 필터 재설정*/
    tableModel->setFilter("o_id LIKE'%0%'");
}

void OrderHandlerForm::on_removePushButton_clicked()                //삭제 버튼을 눌렀을 때
{
//    if(!Oui->tableView4->currentIndex().isValid())   return;

    int row = Oui->tableView4->currentIndex().row();
    int oid = tableModel->record(row).value("o_id").toInt();

    query->exec("DELETE FROM order_info WHERE o_id = "+
                QString::number(oid)+";");
    tableModel->select();

    detailModel->removeRows(row, 1);

//    if(detailModel->rowCount() > 0)
//        cnt = detailModel->rowCount();
}

void OrderHandlerForm::on_modifyPushButton_clicked()                //수정 버튼을 눌렀을 때
{
    /*수정될 정보가 입력된 lineEdit 위젯 모음*/
    QVector<QLineEdit*> lineEdit;
    lineEdit << Oui->orderIDLineEdit << Oui->orderDateLineEdit2
             << Oui->orderQuantityLineEdit2;

    /*현재 lineEdit에 있는 정보를 각 변수에 저장*/
    int oid = lineEdit[0]->text().toInt();
    QString date = lineEdit[1]->text();
    int quantity = lineEdit[2]->text().toInt();

    /*주문 정보 수정 탭에 있는 콤보박스 모음*/
    QVector<QComboBox*> comboBox;
    comboBox << Oui->clientInfoComboBox
             << Oui->productInfoComboBox;

    /*수정하려는 데이터가 하나라도 비어 있는 경우 수정 불가*/
    for(int i = 0; i < lineEdit.size(); i++)
    {
        if(lineEdit[i]->text() == "")
            return;
    }
    if(comboBox[0]->currentText() == tr("select item")) return;
    if(comboBox[1]->currentText() == tr("select item")) return;

    int cBoxIndex = Oui->clientInfoComboBox->currentIndex();        //고객 정보 콤보박스에서 현재 선택된
                                                                    //데이터의 index 출력
    int pBoxIndex = Oui->productInfoComboBox->currentIndex();       //제품 정보 콤보박스에서 현재 선택된
                                                                    //데이터의 index 출력

    /*고객 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int cid = Oui->clientIDComboBox1->itemText(cBoxIndex).toInt();  //고객 ID 콤보박스에서 현재 선택된
                                                                    //데이터의 cid를 출력

    /*제품 정보 콤보박스와 고객 ID 콤보박스의 등록 순서가 동일한 점을 이용*/
    int pid = Oui->productIDComboBox1->itemText(pBoxIndex).toInt(); //제품 ID 콤보박스에서 현재 선택된
                                                                    //데이터의 pid를 출력

    int row = Oui->tableView5->currentIndex().row();                //현재 선택된 인덱스의 행을 저장

    query->exec("UPDATE order_info SET "
                "order_date = '" + date + "', "
                "order_quantity = " + QString::number(quantity) + ", "
                "c_id = " + QString::number(cid) + ", "
                "p_id = " + QString::number(pid) +
                " WHERE o_id = " + QString::number(oid) + ";");
    tableModel->select();

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << QString::number(oid) << date
        << QString::number(quantity);

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 0, items[0]);
    detailModel->setItem(row, 1, items[1]);
    detailModel->setItem(row, 8, items[2]);

    emit orderModifiedClient(cid, row);                             //고객 정보 클래스에 주문 정보 변경에
                                                                    //필요한 고객 정보를 요청하는 시그널 방출
    emit orderModifiedProduct(pid, row);                            //제품 정보 클래스에 주문 정보 변경에
                                                                    //필요한 제품 정보를 요청하는 시그널 방출

    /*입력란 초기화*/
    Oui->orderIDLineEdit->clear();
    Oui->orderDateLineEdit2->clear();
    Oui->orderQuantityLineEdit2->clear();

    /*콤보박스 상태 초기화*/
    Oui->clientInfoComboBox->setCurrentText(tr("select item"));
    Oui->productInfoComboBox->setCurrentText(tr("select item"));
}

/*현재 주문 정보를 입력란에 채워주는 슬롯함수*/
void OrderHandlerForm::on_tableView5_clicked(const QModelIndex &index)
{

    QVector<QLineEdit*> lineEdit;                                   //현재 주문 정보를 대입할 LineEdit 위젯 저장
    lineEdit << Oui->orderIDLineEdit << Oui->orderDateLineEdit2
             << Oui->orderQuantityLineEdit2;

    int row = index.row();                                          //현재 행을 저장

    /*위젯에 설정할 주문 정보 저장*/
    int oid = detailModel->item(row, 0)->text().toInt();
    QString date = detailModel->item(row, 1)->text();
    int quantity = detailModel->item(row, 8)->text().toInt();
    QString cName = detailModel->item(row, 2)->text();
    QString phoneNum = detailModel->item(row, 3)->text();
    QString pSort = detailModel->item(row, 5)->text();
    QString pName = detailModel->item(row, 6)->text();

    /*입력란에 고객 및 제품 정보 설정*/
    lineEdit[0]->setText(QString::number(oid));
    lineEdit[1]->setText(date);
    lineEdit[2]->setText(QString::number(quantity));

    /*콤보박스에 고객 및 제품 정보 설정*/
    Oui->clientInfoComboBox->setCurrentText
            (cName + "(" + phoneNum + ")");                         //이름(전화번호) -> 동명이인 구분을 위함
    Oui->productInfoComboBox->setCurrentText
            (pName + "(" + pSort + ")");                            //제품명(제품 종류) -> 제품 정보 식별을 위함
}

void OrderHandlerForm::modifyReturnClient(QList<QString> cinfo, int row)    //수정 탭에서 보낸 시그널로 넘어온 고객 정보로
{                                                                           //테이블 위젯을 채우는 슬롯함수
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << name << phoneNum << address;

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 2, items[0]);
    detailModel->setItem(row, 3, items[1]);
    detailModel->setItem(row, 4, items[2]);
}

void OrderHandlerForm::modifyReturnProduct(QList<QString> pinfo, int row)
{
    /*넘어온 고객 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();
    int quantity = detailModel->item(row, 8)->text().toInt();
    int totalPrice = price * quantity;

    /*detailMoedl에 들어갈 데이터들*/
    QStringList str;
    str << sort << name << QString::number(price)
        <<QString::number(quantity) << QString::number(totalPrice);

    /*QStandardItem*로 자료형 변환*/
    QList<QStandardItem*> items;
    for(int i = 0; i < str.size(); i++)
    {
        items << new QStandardItem(str[i]);
    }

    /*detailMoedl에 데이터 추가*/
    detailModel->setItem(row, 5, items[0]);
    detailModel->setItem(row, 6, items[1]);
    detailModel->setItem(row, 7, items[2]);
    detailModel->setItem(row, 8, items[3]);
    detailModel->setItem(row, 9, items[4]);
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

void OrderHandlerForm::clientRemoved(int cid, QString name, QString phoneNum)
{                                                                   //고객 정보가 삭제됐다는 시그널을 받은 슬롯함수
    emit clientComboBox(Oui->clientIDComboBox1,
                        Oui->clientInfoComboBox);                   //고객 정보 관련 콤보박스 정보를 재설정

    /*삭제된 고객 ID를 포함하는 주문 ID를 저장*/
    QList<int> oidList;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("c_id").toInt()== cid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidList << oid;
        }
    }

    /*저장된 주문 ID가 포함된 주문 내역 삭제*/
    for(int i = 0; i < oidList.size(); i++)
    {
        query->exec("DELETE FROM order_info WHERE "
                    "c_id = " + QString::number(cid) +";");
        tableModel->select();

        int row;                                                    //수정될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidList[i])))
        {
            row = k->index().row();
            detailModel->removeRows(row, 1);
        }
    }
}

void OrderHandlerForm::productRemoved(int pid, QString sort, QString name)
{                                                                   //제품 정보가 삭제됐다는 시그널을 받은 슬롯함수
    emit productComboBox(Oui->productIDComboBox1,
                         Oui->productInfoComboBox);                 //제품 정보 관련 콤보박스 정보를 재설정

    /*삭제된 제품 ID를 포함하는 주문 ID를 저장*/
    QList<int> oidList;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("p_id").toInt()== pid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidList << oid;
        }
    }

    /*저장된 주문 ID가 포함된 주문 내역 삭제*/
    for(int i = 0; i < oidList.size(); i++)
    {
        query->exec("DELETE FROM order_info WHERE "
                    "p_id = " + QString::number(pid) +";");
        tableModel->select();

        int row;                                                    //수정될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidList[i])))
        {
            row = k->index().row();
            detailModel->removeRows(row, 1);
        }
    }
}

void OrderHandlerForm::clientModified(int cid, QList<QString> cinfo)
{                                                                   //고객 정보가 수정됐다는 시그널을 받는 슬롯함수
    emit clientComboBox(Oui->clientIDComboBox1,
                        Oui->clientInfoComboBox);                   //고객 정보 관련 콤보박스 정보를 재설정

    /*수정된 고객 ID를 포함하는 주문 ID를 저장*/
    QList<int> oidList;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("c_id").toInt()== cid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            oidList << oid;
        }
    }

    /*넘어온 고객 정보를 각 변수로 저장*/
    QString name = cinfo[0];
    QString phoneNum = cinfo[1];
    QString address = cinfo[2];

    for(int i = 0; i < oidList.size(); i++)
    {
        int row;                                                    //수정될 정보가 있는 행

        foreach (auto k, detailModel->
                 findItems(QString::number(oidList[i])))
        {
            row = k->index().row();

            /*detailMoedl에 들어갈 데이터들*/
            QStringList str;
            str << name << phoneNum << address;

            /*QStandardItem*로 자료형 변환*/
            QList<QStandardItem*> items;
            for(int i = 0; i < str.size(); i++)
            {
                items << new QStandardItem(str[i]);
            }

            /*detailMoedl에 데이터 수정*/
            detailModel->setItem(row, 2, items[0]);
            detailModel->setItem(row, 3, items[1]);
            detailModel->setItem(row, 4, items[2]);
        }
    }
}

void OrderHandlerForm::productModified(int pid, QList<QString> pinfo)
{                                                                   //제품 정보가 수정됐다는 시그널을 받는 슬롯함수

    emit productComboBox(Oui->productIDComboBox1,                   //제품 정보 관련 콤보박스 정보를 재설정
                         Oui->productInfoComboBox);

    /*수정된 제품 ID를 포함하는 주문 ID와 주문 수량을 저장*/
    QList<int> oidList;
    QList<int> quantityList;
    for(int i = 0; i < tableModel->rowCount(); i++)
    {
        if(tableModel->record(i).value("p_id").toInt()== pid)
        {
            int oid = tableModel->record(i).value("o_id").toInt();
            int quantity = tableModel->record(i).
                    value("order_quantity").toInt();
            oidList << oid;
            quantityList << quantity;
        }
    }

    /*넘어온 제품 정보를 각 변수로 저장*/
    QString sort = pinfo[0];
    QString name = pinfo[1];
    int price = pinfo[2].toInt();

    /*저장된 주문 ID가 포함된 주문 내역 수정*/
    for(int i = 0; i < oidList.size(); i++)
    {
        int totalPrice = price * quantityList[i];

        /*detailMoedl에 들어갈 데이터들*/
        QStringList str;
        str << sort << name << QString::number(price)
            <<QString::number(quantityList[i])
            << QString::number(totalPrice);

        /*QStandardItem*로 자료형 변환*/
        QList<QStandardItem*> items;
        for(int i = 0; i < str.size(); i++)
        {
            items << new QStandardItem(str[i]);
        }

        int row;                                                    //수정될 정보가 있는 행
        foreach (auto k, detailModel->
                 findItems(QString::number(oidList[i])))
        {
            row = k->index().row();

            /*detailMoedl의 데이터 수정*/
            detailModel->setItem(row, 5, items[0]);
            detailModel->setItem(row, 6, items[1]);
            detailModel->setItem(row, 7, items[2]);
            detailModel->setItem(row, 8, items[3]);
            detailModel->setItem(row, 9, items[4]);
        }
    }
}
