#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTableWidget>
#include <QHeaderView>
#include <vector>
#include <fstream>
#include "MyDataReader.h"

class mytable : public QTableWidget {
    Q_OBJECT

    public:
      // mytable constructor, takes the file as an argument.
      mytable(QWidget *parent = 0, std::ifstream* in = nullptr);
      // stores the names of the coins.
      std::vector<std::string> names;
   
   
    public slots:
      // gets the usd,eur,gbp values from the website.
      void dothings(std::vector<std::string>* ids);
      // creates the table.
      void replyFinished(QNetworkReply * reply) ; 

    private:
      QTableWidget *table;
      QNetworkAccessManager *manager;
      MyDataReader *read;
} ;