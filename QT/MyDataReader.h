#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>
#include <fstream>
using namespace std;

class MyDataReader : public QObject
{
    Q_OBJECT

    public:
      // constructor for data reader.
      MyDataReader(string url, ifstream& in);
      // the file that program reads.
      ifstream* in;
	 
	signals:
		void done(vector<string>* ids);

    public slots:
      void replyFinished(QNetworkReply * reply) ; 

    private:
      QNetworkAccessManager *manager;

} ;