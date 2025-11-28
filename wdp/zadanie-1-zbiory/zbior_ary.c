// Mikolaj Kowalski
// code review: Bartosz Tarnowski


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "zbior_ary.h"

static long long q; //trzymam globalnie stala zmienna q

void zaalokuj(ciag ** tmp, int n){
    if(n <= 0){
        *tmp = NULL;
        return;
    }
    *tmp = malloc((size_t)n * sizeof(ciag));
    assert(*tmp != NULL);
    for(int i = 0; i < n; i ++) (*tmp)[i] = (ciag){0, 0, -1};
}

void stworz_zbior(zbior_ary *tmp, int n){ // ta funkcja tworzy mi pusta tablice w mojej reprezentacji zbioru na tyle elementow ile potrzebuje
    zaalokuj(&tmp->tab, n);
    tmp->roz = n;
}

void wyczysc_zbior(zbior_ary *tmp){
    if(!tmp) return;
    if(tmp->roz == 0){
        free(tmp->tab);
        tmp->tab = NULL;
        return;
    }
    zbior_ary nowy_zbior;
    stworz_zbior(&nowy_zbior, tmp->roz);
    if(nowy_zbior.tab == NULL) return;
    for(int i = 0; i < tmp->roz; i ++) nowy_zbior.tab[i] = tmp->tab[i];
    free(tmp -> tab);
    *tmp = nowy_zbior;
}

bool porownaj(ciag a, ciag b){ // standardowe sprawdzenie ktory przedzial jest wczesniej na osi
    if(a.pocz < b.pocz) return 1;
    if(a.pocz > b.pocz) return 0;
    return (a.kon < b.kon);
}

zbior_ary ciag_arytmetyczny(int a, int Q, int b){ //inicjalizacja ciagu
    zbior_ary wynik; // wynik - zbior ktory chce zwrocic
    q = Q; // Q jest takie same w kazdym wywolaniu funkcji wiec ustalam je globalnie
    stworz_zbior(&wynik, 1);
    wynik.tab[0].pocz = (long long)a; wynik.tab[0].kon = (long long)b;
    wynik.tab[0].mod = (long long)a % q;
    if(wynik.tab[0].mod < 0) wynik.tab[0].mod += q;
    return wynik;
}

zbior_ary singleton(int a){ // singleton to ciag [a, a]
    return ciag_arytmetyczny(a, (int)q, a);
}

void wrzuc(int *x, int *y, ciag *X, ciag *Y) { //na indeks x w X daje element z indeksu y w Y i inkrementuje x oraz y
    X[*x] = Y[*y];
    ++(*x);
    ++(*y);
}


zbior_ary suma(zbior_ary A, zbior_ary B){
    zbior_ary wynik; // wynik - zbior ktory chce zwrocic
    stworz_zbior(&wynik, A.roz + B.roz); // rozmiar tablicy w wynik na pewno nie bedzie wiekszy niz A.roz + B.roz
    int wskaznik_wynik = 0, wskaznik_A = 0, wskaznik_B = 0;
    // sa to nasze iteratory, wskaznik_wynik - ktory bedzie nastepny ciag ktory dodamy do zbioru wynik, wskaznik_X - nastepny rozpatrywany ciag w zbiorze X
    while(wskaznik_A != A.roz || wskaznik_B != B.roz){ // dopoki nie rozpatrzymy wszystkich ciagow w obu zbiorach
        if(wskaznik_A == A.roz){ // jesli rozpatrzymy juz wszystko w A to do wyniku dodajemy juz tylko ciagi z B
            wrzuc(&wskaznik_wynik, &wskaznik_B, wynik.tab, B.tab);
            continue;
        }
        if(wskaznik_B == B.roz){ // analogicznie
            wrzuc(&wskaznik_wynik, &wskaznik_A, wynik.tab, A.tab);
            continue;
        }
        // teraz przypadek ze oba zbiory nie sa jeszcze do konca rozpatrzone
        if(wskaznik_A != A.roz && wskaznik_B != B.roz){
            // ciagi w zbiorze trzymamy posortowane najpierw po q, wiec jesli w ktoryms ze zbiorow aktualnie rozpatrujemy ciag ktory ma modulo mniejsze niz aktualnie rozpatrywany ciag w drugim zbiorze, to dajemy go do wyniku najpierw
            if(A.tab[wskaznik_A].mod < B.tab[wskaznik_B].mod){
                wrzuc(&wskaznik_wynik, &wskaznik_A, wynik.tab, A.tab);
                continue;
            }
            if(A.tab[wskaznik_A].mod > B.tab[wskaznik_B].mod){
                wrzuc(&wskaznik_wynik, &wskaznik_B, wynik.tab, B.tab);
                continue;
            }
            // teraz przypadek ze wszystkie przedzialy maja to samo modulo q, to znaczy ze bede musial je jakos scalic
            long long aktualne_modulo = A.tab[wskaznik_A].mod;
            // dla wygody dla kazdego mozliwego modulo q stworze tymczasową tablice ktora trzyma mi posortowane ciagi nalezace do zbioru A lub B posortowane po kolejnosci na osi
            ciag *tmp;
            zaalokuj(&tmp, A.roz + B.roz);
            int wskaznik_tmp = 0; // ten wskaznik reprezentuje nastepne miejsce gdzie dodam cos do tablicy tmp
            while((wskaznik_A != A.roz && A.tab[wskaznik_A].mod == aktualne_modulo) || (wskaznik_B != B.roz && B.tab[wskaznik_B].mod == aktualne_modulo)){ // dopoki zbiory sie nie skonczyly i mam aktualne modulo
                if(wskaznik_A == A.roz || A.tab[wskaznik_A].mod != aktualne_modulo){ // jesli A sie skonczyl lub nastepny ciag ma juz inne modulo to dodajemy ciag z B
                    wrzuc(&wskaznik_tmp, &wskaznik_B, tmp, B.tab);
                    continue;
                }
                if(wskaznik_B == B.roz || B.tab[wskaznik_B].mod != aktualne_modulo){ // analogicznie
                    wrzuc(&wskaznik_tmp, &wskaznik_A, tmp, A.tab);
                    continue;
                }
                // zatem wiemy ze obecnie rozpatrywane elementy w obu ciagach maja to samo modulo wiec teraz do tymczasowej tablicy dodajemy ten ciag ktory jest wczesniej
                if(porownaj(A.tab[wskaznik_A], B.tab[wskaznik_B])){
                    wrzuc(&wskaznik_tmp, &wskaznik_A, tmp, A.tab);
                    continue;
                }
                else{
                    wrzuc(&wskaznik_tmp, &wskaznik_B, tmp, B.tab);
                    continue;
                }
            }
            // w tym momencie mamy w tablicy tmp posortowane przedzialy o tym samym mod q wiec mozemy zastosowac standardowy algorytm dodawania przedzialow
            // iteruje sie po posortowanych przedzialach i jesli moge rozszerzyc jakis przedzial bo sie "styka" z innym to rozszerzam
            long long a = tmp[0].pocz, b = tmp[0].kon;
            for(int i = 1; i < wskaznik_tmp; i ++){
                if(tmp[i].pocz - q <= b){
                    if(tmp[i].kon > b) b = tmp[i].kon;
                }
                else{
                    wynik.tab[wskaznik_wynik] = (ciag){a, b, aktualne_modulo};
                    ++ wskaznik_wynik;
                    a = tmp[i].pocz;
                    b = tmp[i].kon;
                }
            }
            wynik.tab[wskaznik_wynik] = (ciag){a, b, aktualne_modulo};
            ++ wskaznik_wynik;
            free(tmp); // zwalniamy pamiec po tymczasowej tablicy
        }
    }
    int licznik = 0; // licze moj aktualny rozmiar zbioru wynikowego
    for(int i = 0; i < A.roz + B.roz; i ++){
        if(wynik.tab[i].mod != -1) ++ licznik; // sprawdzam czy komorka jest niepusta
    }
    wynik.roz = licznik;
    wyczysc_zbior(&wynik);
    return wynik;
}

zbior_ary iloczyn(zbior_ary A, zbior_ary B){
    zbior_ary wynik; // zbior ktory chce zwrocic
    stworz_zbior(&wynik, A.roz + B.roz); // rozmiar tablicy w wynik na pewno nie bedzie wiekszy niz A.roz + B.roz
    int wskaznik_wynik = 0, wskaznik_A = 0, wskaznik_B = 0;
    // wskazniki oznaczaja to samo co w funkcji suma
    while(wskaznik_A != A.roz && wskaznik_B != B.roz){ // jesli w ktorymkolwiek zbiorze rozpatrzymy wszystko to na pewno nie dodamy wiecej do iloczynu
        if(A.tab[wskaznik_A].mod < B.tab[wskaznik_B].mod){
            ++ wskaznik_A;
            continue;
        }
        if(A.tab[wskaznik_A].mod > B.tab[wskaznik_B].mod){
            ++ wskaznik_B;
            continue;
        }
        // zeby znalezc jakakolwiek czesc wspolną to modulo w ciagach z obu zbiorow musi byc takie samo, zatem jesli jest inne to rozpatrujemy kolejny ciag w zbiorze w ktorym modulo aktualnie rozpatrywanego ciagu jest mniejsze
        // teraz zakladamy ze modulo w aktualnie rozpatrywanych ciągach w obu zbiorach jest takie samo, wiec potencjalnie czesc wspolna moze istniec
        if(A.tab[wskaznik_A].mod == B.tab[wskaznik_B].mod){
            long long aktualne_modulo = A.tab[wskaznik_A].mod;
            ciag *tmp_A; // pomocnicza tablica dla zbioru A
            ciag *tmp_B; // pomocnicza tablica dla zbioru B
            zaalokuj(&tmp_A, A.roz);
            zaalokuj(&tmp_B, B.roz);
            // dla wygody, analogicznie jak w sumie trzymam dwie pomocnicze tablice trzymające posortowane ciagi z aktualnym modulo w danym zbiorze
            int wskaznik_tmp_A = 0, wskaznik_tmp_B = 0;
            // te iteratory oznacają ktory nastepny ciag dodam w danym zbiorze pomocniczym
            while(wskaznik_A != A.roz && A.tab[wskaznik_A].mod == aktualne_modulo){
                wrzuc(&wskaznik_tmp_A, &wskaznik_A, tmp_A, A.tab);
            }
            while(wskaznik_B != B.roz && B.tab[wskaznik_B].mod == aktualne_modulo){
                wrzuc(&wskaznik_tmp_B, &wskaznik_B, tmp_B, B.tab);
            }
            // standardowo uzupelniamy tablice pomocnicze
            //teraz juz mozemy zrobic standardowy algorytm szukania czesci wspolnej
            // iteruje sie po przedzialach w obu zbiorach i jesli sie przecinaja to dodaje do wyniku przeciecie
            // przesuwam wskaznik na tym zbiorze gdzie aktualny przedzial sie konczy szybciej bo ten pozniejszy moze sie jeszcze z czyms przeciac
            int akt_A = 0, akt_B = 0; //aktualne iteratory po zbiorach pomocniczych
            while(akt_A != wskaznik_tmp_A && akt_B != wskaznik_tmp_B){ // wiemy ze wskaznik_tmp_X to po algorytmie uzupelniania zbiorow pomocniczych to rozmiar danego zbioru pomocniczego
                long long pocz = tmp_A[akt_A].pocz;
                if(tmp_B[akt_B].pocz > pocz) pocz = tmp_B[akt_B].pocz;
                long long kon = tmp_A[akt_A].kon;
                if(tmp_B[akt_B].kon < kon) kon = tmp_B[akt_B].kon;
                if(pocz <= kon){
                    wynik.tab[wskaznik_wynik] = (ciag){pocz, kon, aktualne_modulo};
                    ++ wskaznik_wynik;
                }
                if(tmp_A[akt_A].kon < tmp_B[akt_B].kon) ++ akt_A;
                else ++ akt_B;   
            }
            free(tmp_A);
            free(tmp_B);
            // zwalniamy pamiec w tablicach pomocniczych
        }
    }
    int licznik = 0; //licze moj aktualny rozmiar zbioru wynikowego
    for(int i = 0; i < A.roz + B.roz; i ++){
        if(wynik.tab[i].mod != -1) ++ licznik;
    }
    wynik.roz = licznik;
    wyczysc_zbior(&wynik);
    return wynik;
}

zbior_ary roznica(zbior_ary A, zbior_ary B){
    zbior_ary wynik;
    stworz_zbior(&wynik, A.roz + B.roz);
    int wskaznik_wynik = 0, wskaznik_A = 0, wskaznik_B = 0;
    // linijki powyzej standardowo tak jak w funkcjach suma i iloczyn
    while(wskaznik_A != A.roz){
        // jesli rozpatrzymy juz wszystko w A to na pewno nic wiecej nie dodamy do wyniku
        if(wskaznik_B == B.roz){ // jesli rozpatrzymy wszystko w B to na pewno mozemy do wyniku dodac wszystko co zostalo nierozpatrzone w A
            while(wskaznik_A != A.roz){
                wrzuc(&wskaznik_wynik, &wskaznik_A, wynik.tab, A.tab);
            }
            break;
        }
        // zatem w tym momencie wiemy ze na pewno cos zostalo do rozpatrzenia w obu zbiorach
        if(A.tab[wskaznik_A].mod < B.tab[wskaznik_B].mod){ // jesli modulo w aktualnie rozpatrywanym ciagu w A jest mniejsze niz w -||- w B to na pewno juz nic z B nam nie zabierze tego ciągu, wiec mozemy go dodac do wyniku
            wrzuc(&wskaznik_wynik, &wskaznik_A, wynik.tab, A.tab);
            continue;
        }
        if(A.tab[wskaznik_A].mod > B.tab[wskaznik_B].mod){ // jesli modulo w aktualnie rozpatrywanym ciagu w B jest mniejsze niz w -||- w A to pewno aktualny ciag z B nic nam nie zabiera z A wiec powinnismy rozpatrywac kolejne ciagi z B
            wskaznik_B ++;
            continue;
        }
        if(A.tab[wskaznik_A].mod == B.tab[wskaznik_B].mod){ // teraz juz wiemy ze aktualnie rozpatrywane ciagi w A i B mają to samo modulo
            // zrobmy cale odejmowanie ciagow z modulo rownym temu aktualnemu za jednym razem
            long long aktualne_modulo = A.tab[wskaznik_A].mod;
            ciag *tmp_A, *tmp_B;
            zaalokuj(&tmp_A, A.roz); zaalokuj(&tmp_B, B.roz);
            // trzymam dwie pomocnicze tablice reprezentujace ciagi z aktualnym modulo w obu zbiorach
            int wskaznik_tmp_A = 0, wskaznik_tmp_B = 0;
            while(wskaznik_A != A.roz && A.tab[wskaznik_A].mod == aktualne_modulo){
                wrzuc(&wskaznik_tmp_A, &wskaznik_A, tmp_A, A.tab);
            }
            while(wskaznik_B != B.roz && B.tab[wskaznik_B].mod == aktualne_modulo){
                wrzuc(&wskaznik_tmp_B, &wskaznik_B, tmp_B, B.tab);
            }
            // teraz juz mozemy zrobic standardowy algorytm odejmowania
            // szukam przedzialu w B ktory mi sie przecina z aktualnym przedzialem w A
            // przechodze do kolejnych przedzialow w B dopoki mi sie przecinaja z aktualnym przedzialem w A
            // jesli zostala jakas koncowka w A to dodaje ja do wyniku
            // przechodze do kolejnego przedzialu z B
            int act_B = 0;
            for(int i = 0; i < wskaznik_tmp_A; i ++){
                while(act_B < wskaznik_tmp_B && tmp_B[act_B].kon < tmp_A[i].pocz) ++ act_B;
                long long pocz = tmp_A[i].pocz;
                while(act_B < wskaznik_tmp_B && tmp_B[act_B].pocz <= tmp_A[i].kon){
                    if(tmp_B[act_B].pocz > pocz){
                        wynik.tab[wskaznik_wynik] = (ciag){pocz, tmp_B[act_B].pocz - q, aktualne_modulo};
                        wskaznik_wynik ++;
                    }
                    if(tmp_B[act_B].kon >= pocz) pocz = tmp_B[act_B].kon + q;
                    if(pocz > tmp_A[i].kon) break;
                    ++ act_B;
                }
                if(pocz <= tmp_A[i].kon){
                    wynik.tab[wskaznik_wynik] = (ciag){pocz, tmp_A[i].kon, aktualne_modulo};
                    wskaznik_wynik ++;
                }
            }
            free(tmp_A);
            free(tmp_B);
        }
    }
    int licznik = 0; //licze moj aktualny rozmiar zbioru wynikowego
    for(int i = 0; i < A.roz + B.roz; i ++){
        if(wynik.tab[i].mod != -1) ++ licznik;
    }
    wynik.roz = licznik;
    wyczysc_zbior(&wynik);
    return wynik;
}

bool nalezy(zbior_ary A, int b){
    if(A.roz == 0) return 0;
    long long aktualne_modulo = b % q;
    if(aktualne_modulo < 0) aktualne_modulo += q;
    if(A.tab[A.roz - 1].mod < aktualne_modulo) return 0; // jezeli wszystkie ciagi w zbiorze mają modulo mniejsze niz modulo liczby b to na pewno b nie nalezy do zbioru (wystarczy sprawdzic ostatni ciag bo sa one posortowane wzgledem q)
    // szukamy takiego przedzialu ktory ma modulo q
    int pocz = 0, kon = 0;
    // [pocz, kon] - maksymalny przedzial indeksow ze wszystkie ciagi z tego przedzialu mają modulo rowne aktualne_modulo
    // na poczatek szukamy binarnie pierwszego takiego indeksu, ze modulo >= aktualne_modulo
    int l = 0, r = A.roz - 1;
    while(l < r){
        int mid = (l + r) >> 1;
        if(A.tab[mid].mod < aktualne_modulo) l = mid + 1;
        else r = mid;
    }
    pocz = l; // ten indeks jest poczatkiem naszego przedzialu ciagow modulo aktualne_modulo
    if(A.tab[pocz].mod != aktualne_modulo) return 0; // nalezy oczywiscie jeszcze sprawdzic czy ciag na tym indeksie ma w ogole modulo rowne b bo mogloby sie zdarzyc ze ma on modulo wieksze od b i wtedy zaden ciag w zbiorze nie ma modulo 
    if(A.tab[A.roz - 1].mod == aktualne_modulo) kon = A.roz - 1; // jezeli ostatni ciag w zbiorze tez ma modulo b to nasz szukany przedzial jest do konca zbioru
    else{
        // teraz pierwszy taki ze modulo jest > nasze modulo, wtedy ten indeks - 1 to ostatni taki ze modulo = q;
        l = 0; r = A.roz - 1;
        while(l != r){
            int mid = (l + r) >> 1;
            if(A.tab[mid].mod <= aktualne_modulo) l = mid + 1;
            else r = mid;
        }
        kon = l - 1;
    }
    // teraz mamy pewnosc ze w przedziale [pocz, kon] wszystko ma dobre modulo
    // musimy teraz tu znalezc przedzial ktory odpowiada naszej liczbie 
    //musimy znalezc ostatnie takie i w tym przedziale, ze tab[i].pocz <= b
    //czyli musimy znalezc pierwsze takie i w tym przedziale, ze tab[i].pocz > b;
    //najpierw sprawdzmy czy to w ogole istnieje
    if(A.tab[kon].pocz <= b){ // jezeli ostatni element w przedziale [pocz, kon] ma tab[i].pocz <= b to jest on naszym szukanym indeksem
        if(A.tab[kon].kon >= b) return 1;
        else return 0;
    }
    if(A.tab[pocz].pocz > b) return 0; // jezeli pierwszy przedzial w przedziale ma poczatek za b to tez na pewno b nie nalezy do zbioru
    l = pocz; r = kon;
    while(l != r){
        int mid = (l + r) >> 1;
        if(A.tab[mid].pocz <= b) l = mid + 1;
        else r = mid;
    }
    // na indeksie l mamy pierwszy taki ciag ktory wiemy ze daje dobre modulo i poczatek jest wiekszy od b
    // wiec na indeksie l - 1 mamy ostatni taki ciag ktory wiemy ze daje dobre modulo i poczatek jest mniejszy rowny b
    if(l - 1 >= pocz && l - 1 <= kon && A.tab[l - 1].pocz <= b){ // dla bezpieczenstwa sprawdzmy czy l - 1 na pewno jest w przedziale
        if(A.tab[l - 1].kon >= b) return 1;
        else return 0;
    }
    else return 0;
}

unsigned moc(zbior_ary A){
    long long licznik = 0;
    for(int i = 0; i < A.roz; i ++) licznik += (long long)(1ll + (((long long)(A.tab[i].kon - A.tab[i].pocz)) / q)); 
    return (unsigned int)licznik;
}

unsigned ary(zbior_ary A){
    return (unsigned int)A.roz;
}
