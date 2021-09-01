#include<iostream>
#include<fstream>
#include<vector>
#include <QtCore>
#include <QApplication>
#include <QTableWidget>
#include "mytable.h"
#include <QDebug>

using namespace std;

int main(int argc, char *argv[]) {
	
	string path = getenv ("MYCRYPTOCONVERT");

	ifstream infile;  
	infile.open(path);
	QApplication a(argc, argv);
	mytable my(0, &infile);
	my.show(); 

   	return a.exec();
	
}
