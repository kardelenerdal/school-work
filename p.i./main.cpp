#include <iostream>
#include <fstream>
using namespace std;

bool isPrime(int n){
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i < n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {

    ifstream file;
    string fileName = argv[1];
    file.open(fileName);
    string line;
    int nofLines = 0;
    while (getline(file, line)) {
        nofLines++;
    }
    file.close();
    ifstream infile;
    infile.open(fileName);

    int nofNodes = 1;
    int tree[nofLines][nofLines];
    for(int i=0; i<nofLines; i++){
        for(int j=0; j<nofNodes; j++){
            infile >> tree[i][j];

        }
        for(int j=nofNodes; j<nofLines; j++){
            tree[i][j] = 0;
        }
        nofNodes++;
    }
    //cout << tree[0][0]<< endl;
    int border = nofLines-1;
    for(int i=nofLines-2; i>=0; i--){
        //cout <<border << endl;
        for(int j=0; j<border; j++){
            int sum1 = -1;
            int sum2 = -1;
            if(!isPrime(tree[i][j])){
                if(i==nofLines-2){
                    if(!isPrime(tree[i+1][j])){
                        sum1 = tree[i][j] + tree[i+1][j];
                    }
                    if(!isPrime(tree[i+1][j+1])){
                        sum2 = tree[i][j] + tree[i+1][j+1];
                    }
                } else {
                    sum1 = tree[i][j] + tree[i+1][j];
                    sum2 = tree[i][j] + tree[i+1][j+1];
                }

            }
            if(sum1==-1 && sum2==-1){
                tree[i][j] = -999999;
            } else {
                tree[i][j] = max(sum1, sum2);
            }
            //cout << sum1 << " sum " << sum2<< endl;
            //cout << tree [i][j]<< " ";
        }
        border--;
    }
    cout << tree[0][0];

    return 0;
}
