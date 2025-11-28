// Mikołaj Kowalski
// code review: Grzegorz Kaczmarek

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>

typedef struct{ // struktura trzymajaca pary intow
    int st, nd; 
} para;

typedef struct{ // motele reprezentuja jako ich siec i polozenie na osi
    // aby zachowac czytelnosc nie uzywam structu para tylko tworze osobny
    int siec, pozycja;
} motel;

bool trzy_rozne(int a, int b, int c){ // funkcja sprawdza czy trzy liczby sa parami rozne
    return (a != b && b != c && c != a);
}

int max(int a, int b){
    if(a > b) return a;
    else return b;
}

int min(int a, int b){
    if(a < b) return a;
    else return b;
}

// do rozwiazania potrzebuje dla kazdego motelu znac trzy kolejne motele nalezace do roznych sieci znajdujace sie na lewo i 
// prawo od danego motelu. Trzymam to w tablicy lewo[i][3] i prawo[i][3]. Zakladam ze najblizszy motel zarowno z lewej jak
// i prawej strony dla kazdego motelu to on sam, zatem zachodzi lewo[i][0] = prawo[i][0] = i. Jesli nie istnieja trzy mote-
// le roznych sieci z danej strony to zaznaczam w tablicy -1
// tablice lewo[i] mozna latwo wyliczyc dynamicznie na podstawie tablicy lewo[i - 1], poniewaz wiemy ze tablica lewo[i - 1]
// z definicji wskazuje na trzy rozne motele, to wystarczy rozpatrzyc przypadki czy ktorys z tych trzech moteli nalezy do 
// tej samej sieci co motel i
// tablice prawo[i] uzupelniamy analogicznie
// ze wzgledu na duze podobienstwo w uzupelnaniu tablic lewo i prawo mozna to zrobic przy pomocy jednej funkcji, wystarczy 
// wiedziec czy idziemy od poczatku czy od konca oraz czy wyliczamy wartosci na podstawie tablicy wczensiej czy z pozniej

void uzupelnij(int n, int tab[][3], motel *motele, bool strona){ // strona: 0 - licze lewo, 1 - licze prawo
    for(int k = 0; k < 3; k ++){
        for(int i = 0; i < n; i ++) tab[i][k] = -1;
    }
    int start, roznica;
    if(strona == 0){ // gdy licze lewo to zaczynam od indeksu 0 i odwoluje sie do tablicy od indeksu wczesniejszego
        start = 0;
        roznica = 1;
    }
    else{ // gdy licze prawo to zaczynam od indeksu n - 1 i odwoluje sie do tablicy od indeksu pozniejszego
        start = n - 1;
        roznica = -1;
    }
    tab[start][0] = start;
    for(int i = start + roznica; (i >= 0 && i < n); i += roznica){
        tab[i][0] = i; // niezmiennik
        // rozpatruje przypadki czy ktorys motel z tablicy do ktorej sie aktualnie odwoluje (czyli i-1 lub i+1 
        // w zaleznosci od strony) nalezy do tej samej sieci co motel i
        if(motele[tab[i - roznica][0]].siec == motele[i].siec){
            tab[i][1] = tab[i - roznica][1];
            tab[i][2] = tab[i - roznica][2];
        }
        else if(tab[i - roznica][1] != -1 && motele[tab[i - roznica][1]].siec == motele[i].siec){
            tab[i][1] = tab[i - roznica][0];
            tab[i][2] = tab[i - roznica][2];
        }
        else{
            tab[i][1] = tab[i - roznica][0];
            tab[i][2] = tab[i - roznica][1];
        }
    }
}

para znajdz_wynik(int n, int (*lewo)[3], int (*prawo)[3], motel *motele){
    para wynik = {INT_MAX, INT_MIN}; // pierwsza wspolrzedna - odl najblizszej trojki, druga - najdalszej
    bool ok = 0; // sprawdzam czy w ogole istnieja motele nalezace do trzech roznych sieci
    for(int act_b = 1; act_b < n - 1; act_b ++){ // iteruje sie po kandydacie na motel B
        // szukam najblizszej trojki
        // mozna udowodnic ze dla kazdego motelu B optymalny dla niego motel A to ktorys z trzech
        // moteli nalezacych do roznych sieci polozonych bezposrednio na lewo, a motel C - bezposrednio na prawo
        // zatem sprawdzam wszystkie opcje (upewniam sie czy motele na pewno naleza do roznych sieci)
        for(int i = 0; i < 3; i ++){
            for(int j = 0; j < 3; j ++){
                int act_a = lewo[act_b - 1][i];
                int act_c = prawo[act_b + 1][j];
                if(act_a == -1 || act_c == -1) continue;
                if(trzy_rozne(motele[act_a].siec, motele[act_b].siec, motele[act_c].siec)){
                    wynik.st = min(wynik.st, max(motele[act_b].pozycja - motele[act_a].pozycja, 
                    motele[act_c].pozycja - motele[act_b].pozycja));
                    ok = 1;
                }
            }
        }
        // szukam najdalszej trojki
        // mozna udowodnic ze dla kazdego motelu B optymalny dla niego motel A to ktorys z trzech moteli
        // nalezacych do roznych sieci polozonych jak najdalej na lewo, a motel C - jak najdalej na prawo
        // sa to zatem motele bezposrednio na prawo motelu na indexie 0 i na lewo motelu o indexie n - 1
        // zatem sprawdzam wszystkie opcje (upewniam sie czy motele na pewno naleza do roznych sieci i czy 
        // sa w kolejnosci A-B-C)
        for(int i = 0; i < 3; i ++){
            for(int j = 0; j < 3; j ++){
                int act_a = prawo[0][i];
                int act_c = lewo[n - 1][j];
                if(act_a == -1 || act_c == -1) continue;
                if(trzy_rozne(motele[act_a].siec, motele[act_b].siec, motele[act_c].siec) && 
                    act_a < act_b && act_b < act_c){
                         wynik.nd = max(wynik.nd, min(motele[act_b].pozycja - motele[act_a].pozycja, 
                            motele[act_c].pozycja - motele[act_b].pozycja));
                            ok = 1;
                }
            }
        }
    }
    if(ok == 0) return (para){0, 0};
    return wynik;
}

int main(){
    int n;
    if(scanf("%d", &n) != 1){
        fprintf(stderr, "Nie udalo sie wczytac n\n");
        return 1;
    }
    motel *motele = malloc((size_t)n * sizeof(motel));
    assert(motele != NULL);
    for(int i = 0; i < n; i ++){
        if(scanf("%d %d",&motele[i].siec, &motele[i].pozycja) != 2){
            fprintf(stderr, "Nie udalo sie wczytac danych motelu %d.\n", i);
        }
    }
    int (*lewo)[3] = malloc((size_t)n * sizeof *lewo);
    assert(lewo != NULL);
    int (*prawo)[3] = malloc((size_t)n * sizeof *prawo);
    assert(prawo != NULL);
    uzupelnij(n, lewo, motele, 0);
    uzupelnij(n, prawo, motele, 1);
    para wynik = znajdz_wynik(n, lewo, prawo, motele);
    printf("%d %d\n", wynik.st, wynik.nd);
    return 0;
}
