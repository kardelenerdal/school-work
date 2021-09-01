#include <QtGui>
#include "mytable.h"
#include <QRegExp>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDebug>
#include <QTextStream>
#include<fstream>
using namespace std;

mytable::mytable(QWidget *parent, ifstream* in ) : QTableWidget(parent) {

	this->names={};

    // creating the table.
    table = new QTableWidget();
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(table);
    setLayout(layout);

    string url0 = "https://api.coingecko.com/api/v3/coins/list";

    // creating a data reader object.
    read = new MyDataReader(url0, *in);

    // if data is read, table should start to do its job.
    connect(read, &MyDataReader::done, this, &mytable::dothings);
}

void mytable::dothings(vector<string>* ids) {
    
    // getting the ids of coins as an argument.
    this->names = *ids;
    manager = new QNetworkAccessManager(this) ; 

    // if data is read, create the table.
    connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(replyFinished(QNetworkReply *)));

    // creating the url from scratch.
    std::string url = "https://api.coingecko.com/api/v3/simple/price?ids=" + this->names[0];
     for(unsigned int i = 1; i<this->names.size(); i++){
        url += "," + this->names[i];
     }
     url += "&vs_currencies=usd,eur,gbp";

     // doing the network request with the url.
     QString qurl = QString::fromStdString(url);
     manager->get(QNetworkRequest(QUrl(qurl)));

}

void mytable::replyFinished(QNetworkReply *reply)  {

	// read the data fetched from the web site.
    QString data = (QString) reply->readAll();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(QStringList() << tr("") <<tr("USD") << tr("EUR") << tr("GBP"));
    
    // hiding the row indexes.
    QHeaderView* vh=new QHeaderView(Qt::Vertical);
    vh->hide();
    table->setVerticalHeader(vh);

    // the loop that creates the table.
	for(unsigned int i=0; i<this->names.size(); i++){

		QString usd, eur, gbp; 
    	int pos = 0;
    	
    	std::string pat = "\"" + this->names[i] + "\":\\{\"usd\":(\\d+\\.?\\d*e?\\-?\\d+),\"eur\":(\\d+\\.?\\d*e?\\-?\\d+),\"gbp\":(\\d+\\.?\\d*e?\\-?\\d+)";
    	QString pattern = QString::fromStdString(pat);
     	QRegExp rx(pattern);

        // getting the corresponding coin's data.
	 	if ( rx.indexIn(data, pos) != -1 ) {
	       usd =  rx.cap(1);    
	       eur =  rx.cap(2);
	       gbp =  rx.cap(3);
	    } else {
	       usd = QString("Error"); 
	       eur = QString("Error"); 
	       gbp = QString("Error"); 
	    }

        // putting the current coin to the table.
	    table->insertRow(table->rowCount());
	    table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(QString::fromStdString(this->names[i])));
		table->setItem(table->rowCount()-1, 1, new QTableWidgetItem(usd));
		table->setItem(table->rowCount()-1, 2, new QTableWidgetItem(eur));
		table->setItem(table->rowCount()-1, 3, new QTableWidgetItem(gbp));
	}
    
}