#include <QtGui>
#include "MyDataReader.h"
#include <QRegExp>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>
#include <fstream>

using namespace std;

// gets the file and gets the data from the website
MyDataReader::MyDataReader(string url, ifstream& infile){

	this->in = &infile;
	manager = new QNetworkAccessManager(this);

	// signal to get the data from the website. 
    connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(replyFinished(QNetworkReply *)));
    QString qurl = QString::fromStdString(url);
    manager->get(QNetworkRequest(QUrl(qurl)));

}

// one of the most important functions of the project. gets the ids of the coins from the website.
void MyDataReader::replyFinished(QNetworkReply *reply)  {
		
    	QString data = (QString) reply->readAll();
    	string token; 
    	std::vector<std::string> ids = {};

    	while((this->in)->peek() != EOF) {

			getline(*(this->in), token);
			
			// regex part if symbol is given.
			string pat = "([a-zA-Z\\-\\d]+)\",\"symbol\":\"" + token +"\"";
			QString pattern = QString::fromStdString(pat);
			QRegExp rx(pattern);

			int pos = 0;
			if(rx.indexIn(data, pos) != -1){
				ids.push_back(rx.cap(1).toStdString());
				continue;
			}

			// regex part if name is given.
			string pat2 = "([a-zA-Z\\-\\d]+)\",\"symbol\":\"([a-zA-Z\\-\\d\\s]+)\",\"name\":\"" + token + "\"";
			QString pattern2 = QString::fromStdString(pat2);
			QRegExp rx2(pattern2);
			if(rx2.indexIn(data, pos) != -1){
				ids.push_back(rx2.cap(1).toStdString());
			}
		
		}

		// done signal.
		emit done(&ids);
}