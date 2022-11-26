#include <bits/stdc++.h>
#pragma GCC target ("avx2")
#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")
using namespace std;

struct Word{
    char l[6];
};

class DataBase : public vector<Word>{
    
public:
    DataBase(){
        ifstream in("cuvinte_wordle.txt");
        Word t;
        while(in >> t.l)
            push_back(t);
    }
};

unsigned char put3[] = {1, 3, 9, 27, 81, 243};
unsigned char GYW[128];

unsigned char toInt(char * r){
    return GYW[r[0]] * put3[4] + GYW[r[1]] * put3[3] + GYW[r[2]] * put3[2] + GYW[r[3]] * put3[1] + GYW[r[4]] * put3[0]; 
}

namespace Simulare{
    // simuleaza rezultatul pentru cuvRand = sr, raspTastatura = ans; returneaza prin rez
    bool pres[128];
    /*
        GYW = 012
    */
    char rez[6];
    unsigned char sim(const char * sr, const char * ans){
        
        rez[5] = '\0';
        unsigned char ns = 0;
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
        return toInt(rez);
    }
};

class Univers : public vector<short>{
public:
    Univers() { }
    Univers(DataBase & db) {
        for(int i = 0; i < db.size(); i++)
            push_back(i);
    }
    // universul de posibilitati se restrange in functie de compatibilitate
    void shrink(const char * ans, const unsigned char rez, DataBase & db){
        int v = 0;
        for(int i = 0; i < (int)size(); i++){
            if(Simulare::sim(db[(*this)[i]].l, ans) == rez)
                (*this)[v++] = (*this)[i];
        }
        resize(v);
    }
};
Univers univStart[256];

int mpp[256];

void solve(DataBase & db){
    Univers unv(db);
    cout << "TAREI\n";
    cout.flush();
    Word rez;
    cin >> rez.l;
    if(strcmp(rez.l, "GGGGG") == 0)
        return;

    unv = univStart[toInt(rez.l)];

    while(true){
        double entAns = 0;
        Word ans = db[unv[0]];
        double lgUnvSize = log2(unv.size()); /// precalculam log2(dimensiunea_universului)
        for(int i = 0; i < db.size(); i++){ /// fixam cuvintele candidate pe care sa le afisam jocului
            double entAct = 0;
            fill(mpp, mpp + 243, 0);
            for(auto & u : unv) /// pentru fiecare cuvand ce apartine universului (adica multimii de potentiale solutii la joc) simulam raspunsul pe care l-ar da jocul pentru cuvantul candidat actual
                mpp[Simulare::sim(db[u].l, db[i].l)]++;
        
            for(int x = 0; x < 243; x++) /// calculam entropia (folosind valorile precalculate si formula rescrisa pentru a reduce numarul de operatii)
                if(mpp[x] != 0)
                    entAct += (-log2(mpp[x]) + lgUnvSize) * mpp[x];

            entAct /= unv.size();
            if(entAns < entAct) /// daca obtinem un cuvant cu entropie mai mare, il inlocuim pe cel cu care raspundem
                ans = db[i], entAns = entAct;
        }
        cout << ans.l << "\n"; /// pasam jocului cuvantul candidat
        cout.flush();
        Word rez;
        cin >> rez.l; /// citim raspunsul jocului
        if(strcmp(rez.l, "GGGGG") == 0)
            return;
        unv.shrink(ans.l, toInt(rez.l), db); /// restrangem potentialele solutii (eliminam cuvintele ce nu returneaza configuratia "rez" pentru "ans" returnat de player)
    }
}

int main(){
    GYW['G'] = 0, GYW['Y'] = 1, GYW['W'] = 2;
    DataBase db;

    /// cod generare universuri de inceput
    int tt = 0;
    for(auto &x:db){
        univStart[Simulare::sim(x.l, "TAREI")].push_back(tt);
        tt++;
    }

    while(true){
        Word isGo;
        cin >> isGo.l;
        if(isGo.l[0] == 'S')
            break;
        solve(db);
    }
        

    return 0;
}
