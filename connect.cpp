#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <iomanip>

int main(int argc, char *argv[]) {
    int pids[2], pipes[2][2], i;
    const int nrCuvinteWordle = 11454;
    // const int nrCuvinteWordle = 100;
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <game> <player>\n";
        return -1;
    }

    std::cout << "Numar threaduri disponibile? (<= 16)\n";
    std::cout.flush();
    int nrThreaduri;
    std::cin >> nrThreaduri;

    if(nrThreaduri <= 0 || nrThreaduri > 16){
        std::cout << "Numar incorect de thread-uri\n";
        return -1;
    }

    for(int nn = 0; nn < nrThreaduri; nn++){
        /// calculam capetele stanga-dreapta asociate fiecarui fork
        int ll = nn * (nrCuvinteWordle / nrThreaduri), rr = ll + (nrCuvinteWordle / nrThreaduri);
        if(nn == nrThreaduri - 1)
            rr = nrCuvinteWordle;

        // cerem sistemului sa ne faca niste pipe-uri
        for (i = 0; i < 2; i++) {
            if (pipe(pipes[i]) == -1) {
                std::cerr << "can't create pipe\n";
                return -1;
            }
        }

        for (i = 0; i < 2; i++) {
            pids[i] = fork();
            /// dupa fork, copilul va avea 0 iar parintele va avea id-ul copilului
            if (pids[i] == -1) {
                std::cerr << "can't fork\n";
                return -1;
            }
            if (pids[i] == 0) { /// deci procesul e copil
                /// dup2 ne echivaleaza FD-urile unor pipe-uri
                if (dup2(pipes[i ^ 1][0], 0) == -1) { ///output-ul (ca file descriptor) celuilalt program devine stdin pentru asta
                    std::cerr << "dup2 broke\n";
                    return -1;
                }
                if (dup2(pipes[i][1], 1) == -1) { ///input-ul celuilalt program devine stdout pentru asta
                    std::cerr << "dup2 broke\n";
                    return -1;
                }
                // executam
                if(i == 0){ ///adica deschidem game-ul
                    char temp1[10], temp2[10], temp3[10];

                    sprintf(temp1, "%d", ll);
                    sprintf(temp2, "%d", rr);
                    sprintf(temp3, "%d", nn);
                    char * argvv[] = {argv[1], temp1, temp2, temp3, NULL};
                    execv(argv[i + 1], argvv);
                    std::cerr << "execve doesn't work\n";
                    return -1;
                }
                else{ ///adica deschidem player-ul
                    execv(argv[i + 1], NULL);
                    std::cerr << "execve doesn't work\n";
                    return -1;
                }
                
            } else { /// deci procesul e parinte
                std::cout << "Program " << argv[i + 1] << " started with id " << pids[i] << "\n";
            }
        }
        /// inchidem pipe-urile (adica renuntam la FD-urile create artificial, programele de mai sus avand deja echivalate pipe-urile proprii)
        for (i = 0; i < 2; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
    }

    int running;
    // asteptam sa se termine toate procesele pornite cu fork
    for (running = 2 * nrThreaduri; running > 0; ) {
        int status;
        pid_t pid = waitpid(-1, &status, 0);
        if (pid == -1) {
            std::cerr << "waitpid returned -1\n";
        }
        /// vedem daca exit-ul e normal
        if (WIFEXITED(status)) {
            std::cout << "Program " << pid << " exited with status " << WEXITSTATUS(status) << "\n";
        } else if (WIFSIGNALED(status)) {
            std::cout << "Program " << pid << " killed by signal " << WTERMSIG(status) << "\n";
        }
        running -= 1;
    }

    /// cleanup
    int ans = 0;
    char linie[101];
    std::ofstream outFin("desfasurareFin.txt");
    for(int i = 0; i < nrThreaduri; i++){
        using namespace std;
        char temp1[10];
        sprintf(temp1, "%d", i);
        string numeTempRead = "desfasurare" + string(temp1) + ".txt";
        ifstream tempRead(numeTempRead);
        
        string gg;
        while(getline(tempRead, gg)){
            if(gg[0] >= 'A' && gg[0] <= 'Z'){
                outFin << gg << "\n";
            }
            else if(gg[0] != '\n'){
                ans += stoi(gg);
            }
        }

        tempRead.close();
        remove(numeTempRead.c_str());
    }

    std::cout << "Numarul mediu de incercari este " << std::fixed << std::setprecision(5) << ((long double) ans) / nrCuvinteWordle << "\n"; 

    return 0;
}
