#include "worki.h"
#include <algorithm>
#include <list>
#include <assert.h>

struct przedmiot::synowie{ // gdy przedstawimy sytuacje jako drzewo, to synami worka lub biurka sa przedmioty i worki ktore sie w nim znajduja
    // wsrod zbioru synow interesuje nas tylko kto jest ich ojcem i ile wsrod nich (bezposrednio lub posrednio) jest przedmiotow
    worek* ojciec = nullptr; // jezeli ojciec = nullptr, to przedmiot lub worek lezy na biurku
    int ile = 0; 
};

using synowie = przedmiot::synowie;

static synowie* biurko = new synowie; // globalna zmienna reprezentujaca co znajduje sie na biurku
static int ile_workow = 0; // ile aktualnie stworzylem workow (potrzebne do numerowania)

// listy potrzebne do zwolnienia pamieci
std::list<przedmiot*> lista_przedmiotow;
std::list<worek*> lista_workow;
std::list<synowie*> lista_synow = {biurko};


przedmiot* nowy_przedmiot(){
    przedmiot* p = new przedmiot;
    p->polozenie = biurko; // nowy przedmiot lezy na biurku
    ++ biurko->ile;
    lista_przedmiotow.push_back(p);
    return p;
}

worek* nowy_worek(){
    worek* w = new worek;
    w->nr = ile_workow ++;
    w->polozenie = biurko; // nowy worek lezy na biurku
    synowie* s = new synowie; // jego synowie sa na razie pusci, nic w nim nie ma
    w->moi_synowie = s;
    lista_synow.push_back(s);
    w->moi_synowie->ojciec = w; // worek jest oczywiscie ojcem swoich synow
    lista_workow.push_back(w);
    return w;
}

void wloz(przedmiot *co, worek *gdzie){ // wkladam przedmiot co do worka gdzie

    assert(co->polozenie->ojciec == nullptr); // zalozenie: przedmiot lezy na biurku
    assert(gdzie->polozenie->ojciec == nullptr); // zalozenie: worek lezy na biurku

    co->polozenie = gdzie->moi_synowie; // dodaje przedmiot do synow worka
    ++ gdzie->moi_synowie->ile; // zwiekszam ile przedmiotow jest w worku
}

void wloz(worek *co, worek *gdzie){ // wkladam worek co do worka gdzie

    // zalozenie: oba worki leza na biurku
    assert(co->polozenie->ojciec == nullptr);
    assert(gdzie->polozenie->ojciec == nullptr);

    co->polozenie = gdzie->moi_synowie;  // dodaje worek co do listy synow worka gdzie
    gdzie->moi_synowie->ile += co->moi_synowie->ile; // w worku gdzie znajduja sie teraz wszystkie przedmioty z worka co
}

void wyjmij(przedmiot *p){ // wyjmuje przedmiot p z worka lezacego na biurku

    assert(p->polozenie->ojciec != nullptr); // zalozenie: p nie lezy na biurku
    assert(p->polozenie->ojciec->polozenie->ojciec == nullptr); // zalozenie: worek w ktorym jest p lezy na biurku

    -- p->polozenie->ile; // zmniejszam liczbe przedmiotow w worku w ktorym byl p
    p->polozenie = biurko; // p lezy teraz na biurku
}

void wyjmij(worek *w){ // wyjmuje worek w z worka lezacego na biurku

    assert(w->polozenie->ojciec != nullptr); // zalozenie: w nie lezy na biurku
    assert(w->polozenie->ojciec->polozenie->ojciec == nullptr); // worek gdzie lezy ten worek jest na biurku

    w->polozenie->ile -= w->moi_synowie->ile; // z worka gdzie lezal worek ktory wyjmujemy usuwaja sie wszystkie przedmioty
    // z wyjmowanego worka
    w->polozenie = biurko; // wyjmowany worek lezy teraz na biurku
}

int w_ktorym_worku(przedmiot *p){ // sprawdzam ojca listy synow gdzie znajduje sie p
    if(p->polozenie->ojciec == nullptr) return -1; // przedmiot na biurku
    else return p->polozenie->ojciec->nr;
}

int w_ktorym_worku(worek *w){ // sprawdzam ojca listy synow gdzie znajduje sie w
    if(w->polozenie->ojciec == nullptr) return -1; // worek na biurku
    else return w->polozenie->ojciec->nr;
}

int ile_przedmiotow(worek *w){ // sprawdzam licznik listy synow w
    return w->moi_synowie->ile;
}

void na_odwrot(worek *w){

    assert(w->polozenie->ojciec == nullptr); // zalozenie: w jest na biurku

    int nowe_biurko = biurko->ile, nowe_w = biurko->ile - w->moi_synowie->ile; // tyle beda miec w sobie przedmiotow biurko i w po zmianie
    // liczba przedmiotow na biurku sie nie zmieni, liczba przedmiotow w worku to bedzie liczba przedmiotow na biurku - te z worka
    std::swap(w->moi_synowie, biurko); // zamieniam adresy list synow biurka i worka
    w->moi_synowie->ojciec = w;
    w->polozenie = biurko; // w jest na biurku
    biurko->ojciec = nullptr; // ojcowie sie musza zgadzac
    w->moi_synowie->ile = nowe_w;
    biurko->ile = nowe_biurko; // liczby przedmiotow w worku i na biurku sie musza zgadzac
}

void gotowe(){ // zwalniamy pamiec po przedmiotach, workach i synach
    while(lista_przedmiotow.size()){
        auto *p = lista_przedmiotow.back();
        delete p;
        lista_przedmiotow.pop_back();
    }
    while(lista_workow.size()){
        auto *w = lista_workow.back();
        delete w;
        lista_workow.pop_back();
    }
    while(lista_synow.size()){
        auto *s = lista_synow.back();
        delete s;
        lista_synow.pop_back();
    }
}
