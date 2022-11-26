#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>
using namespace std;


class DataBase : public vector<string>{
public:
    DataBase(){
        ifstream in("cuvinte_wordle.txt");
        string x;
        while(in >> x)
            push_back(x);
    }
};

namespace Simulare{
    // simuleaza rezultatul
    bool pres[128];
    /// rez = rezultat simulare, ans = ce raspuns am dat jocului, sr = ce cuvant ales random cred ca are jocul 
    void sim(const string & sr, const string & ans, string & rez){
        for(int i = 0; i < 5; i++){
            pres[sr[i]] = pres[ans[i]] = false;
            if(sr[i] == ans[i])
                rez[i] = 'G';
        }
        for(int i = 0; i < 5; i++)
            pres[sr[i]] = true;
        for(int i = 0; i < 5; i++)
            if(sr[i] != ans[i]){
                if(pres[ans[i]] == true)
                    rez[i] = 'Y';
                else
                    rez[i] = 'W';
            }
    }
};

const int dbSize = 11454;

int main(int argc, char * argv[]){
    if(argc != 4 && argc != 1){
        return -1;
    }
    int ll, rr;
    ofstream logFile;
    if(argc == 1){ /// nu s-a pasat nimic, deci jucatorul e uman
        ll = rand() % dbSize, rr = ll + 1;
        logFile = ofstream("desfasurare.txt");
        cout << "\n\n|+|        WORDLE - Romana        |+|\n\n\n";
    }
    else{ /// connect-ul paseaza intervalul pe care ar trebui sa il simuleze jocul
        ll = atoi(argv[1]), rr = atoi(argv[2]);
        string logName(argv[3]); /// numarul instantei de joc
        logFile = ofstream("desfasurare" + logName + ".txt");
    }
    
    DataBase db;
    int nr = 0;
    for(int i = ll; i < rr; i++){
        // cerr << i << "\n";
        string sr = db[i];
        logFile << sr << ", ";
        cout << "Go\n";
        cout.flush();
        int nnn = 0;
        while(true){
            string t;
            cin >> t;
            nnn++;
            logFile << t << " ";
            string rez;
            rez.resize(5);
            Simulare::sim(sr, t, rez);
            cout << rez << "\n";
            nr++;
            cout.flush();
            if(rez == "GGGGG"){
                logFile << "\n";
                //logFile << " : " << nnn << "\n";
                // ^^^ afiseaza numarul de incercari per sesiune ^^^

                break;
            }
        }
    }
    cout << "Stop\n";
    cout.flush();
    if(argc != 1){ /// afiseaza numarul de incercari din instanta
        logFile << nr << "\n";
    }
    return 0;
}
