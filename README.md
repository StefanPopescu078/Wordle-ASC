# Wordle

## Conectorul (fișierul "connect.cpp")

Acesta realizează atât comunicarea dintre joc și jucător (prin intermediul unor pipe-uri), cât și paralelizarea instanțelor de joc (perechilor "game"-"player" active).
Fiecare proces de tip "game" pornit de conector va genera (ca rezultat al pasării unor argumente prin "execv()" ) un fișier "desfășurareX.txt" (unde "X" reprezintă numărul instanței de joc), în care se vor păstra încercările și numărul acestora.
În final, conectorul va calcula media încercărilor și va concatena desfășurările jocurilor în fișierul "desfășurareFin.txt", ștergând ulterior fișierele temporare.

## Jocul (fișierul "game.cpp")

Jocul implementează un comportament de tip "buggy" (în care literele ce se potrivesc sunt marcate cu verde, iar cele prezente în cuvânt -marcate deja sau nu- sunt colorate cu galben).
Jocul afișează la începutul unei sesiuni de joc cuvântul "Go", iar la final cuvântul "Stop" (în scopul comunicării cu programul "player"; la fiecare "Go" al jocului, player-ul incepe o sesiune nouă de joc).
Cuvintele se citesc din fișierul "cuvinte_wordle.txt".

## Playerul (fișierul "player.cpp")

Playerul calculează la fiecare pas cel mai bun mod de a face o alegere pentru a obține entropia maximă pentru răspunsul dat. Acesta utilizează o strategie de tip greedy, alegând cuvântul ce generează maximul de informație din perspectiva unei singure tranziții de joc, neluând în calcul potențialele decizii ulterioare (din motive de performanță). Algoritmul din spatele "player"-ului (în pseudocod) este următorul:

1. _Pentru_ $r \in R$( $R$ este mulțimea raspunsurilor) _execută_
2. $\space\space\space\space$_calculează entropia lui_ $r$
3. $\space\space\space\space$_Daca_ $r$ _dă o entropie mai buna Atunci_
4. $\space\space\space\space\space\space\space\space$_salvează_ $r$ _drept cel mai bun răspuns de pană acum_
5. $\space\space\space\space$_sfârșit_
6. _sfârșit_
7. _Folosește răspunsul calculat la pașii 1-6_
8. _Obține_ $c$ _configurația de la joc și setează_ $S:=S\setminus \\{ s \in S | feedback(best, s)\ne c\\}$ ( $S$ este mulțimea soluțiilor posibile)
9. _Dacă_ $\|S\|\ne1$ _Atunci_
10.  $\space\space\space\space$ _Repetă de la pasul 1_
11. _sfârșit_

## Benchmark-uri

Media încercărilor este 4,00.
Numărul maxim de încercări per cuvânt este 7 (pentru cuvantul "NUNII").
Întreaga simulare durează aproximativ 4 min. (pe un Ryzen 7 4800H, cu setarea de 16 thread-uri dată programului "connect").

### Formula entropiei Shannon

Entropia unui răspuns $r$ este:
$$E(r)=\sum_{c\in C} P(r, c)*log_2{1\over P(r, c)}$$
Unde $C$ este mulțimea configurațiilor(vezi Note) $P(r, c)$ este probabilitatea ca dat fiind răspunsul $r$, jocul să returneze configurația $c$.
Aceasta se poate calcula drept $P(r, c)={\|\\{s\in S| feedback(r, s)=c\\}\|\over\|S\|}$, unde $feedback(r, s)$ este configurația întoarsă de joc dacă am da răspunsul $r$ si soluția corectă ar fi $s$.

### Optimizări (de timp)

* Cuvântul oferit inițial de "player" este întotdeauna **"TAREI"** (din lipsă de alte informații), astfel încat îl afișăm mereu înainte de orice calcul al entropiei (e hard-coded).
* De asemenea, algoritmul inițializează spațiul soluțiilor doar după ce a primit configurația corespunzatoare răspunsului inițial. Această optimizare are rolul de a reduce numărul de calcule efectuate la începutul unei sesiuni.
* În calculul entropiei, având în vedere că, pentru un cuvand candidat fixat, configurații egale returnate de joc oferă informație egală, algoritmul salvează frecvențele configurațiilor întoarse și le folosește drept ponderi în calculul entropiei (echivalent cu a da factor comun termenii egali în formula entropiei).

### Compilare si execuție

Pentru a rula simularea folosiți comenzile:
1. g++ game.cpp -o game
2. g++ player.cpp -o player
3. g++ connect.cpp -o connect
4. ./connect game player

### Note

O configurație este reprezentată de un șir de 5 caractere din mulțimea $\\{'G', 'Y', 'W'\\}$. De exemplu: $'WYYGG'$ reprezintă o configurație ce spune că ultimele 2 caractere sunt pe poziția corectă și sunt caractere corecte, al doilea și al treilea sunt caractere corecte, dar nu pe poziții corecte, iar primul este greșit. Această configurație poate fi primită în cazul când soluția finală este $'CARTE'$ iar răspunsul dat este $'FRATE'$. (convenția culorilor este 'G' = verde, 'Y' = galben, 'W' = gri)

### Realizatori

1. Ilie Dumitru
2. Popescu Ștefan - Alexandru
