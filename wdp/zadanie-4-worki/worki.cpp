#include "worki.h"
#include <algorithm>
#include <list>

struct synowie{ // gdy przedstawimy sytuacje jako drzewo, to synami worka lub biurka sa przedmioty i worki ktore sie w nim znajduja
    // wsrod zbioru synow interesuje nas tylko kto jest ich ojcem i ile wsrod nich (bezposrednio lub posrednio) jest przedmiotow
    worek* ojciec = nullptr;
    int ile = 0; 
};

static synowie* biurko = new synowie; // globalna zmienna reprezentujaca co znajduje sie na biurku
static int ile_workow = 0; // ile aktualnie stworzylem workow (potrzebne do numerowania)

std::list<przedmiot*> lista_przedmiotow;
std::list<worek*> lista_workow;
std::list<synowie*> lista_synow = {biurko};


przedmiot* nowy_przedmiot(){
    przedmiot* p = new przedmiot;
    p->gdzie = biurko;
    ++ biurko->ile;
    lista_przedmiotow.push_back(p);
    return p;
}

worek* nowy_worek(){
    worek* w = new worek;
    w->nr = ile_workow ++;
    w->gdzie = biurko;
    synowie* s = new synowie;
    w->moi_synowie = s;
    lista_synow.push_back(s);
    w->moi_synowie->ojciec = w;
    lista_workow.push_back(w);
    return w;
}

void wloz(przedmiot *co, worek *gdzie){ // wkladam przedmiot co do worka gdzie
    co->gdzie = gdzie->moi_synowie;
    ++ gdzie->moi_synowie->ile;
}

void wloz(worek *co, worek *gdzie){ // wkladam worek co do worka gdzie (analogicznie jak przedmiot do worka)
    co->gdzie = gdzie->moi_synowie;
    gdzie->moi_synowie->ile += co->moi_synowie->ile;
}

void wyjmij(przedmiot *p){ // wyjmuje przedmiot p z worka lezacego na biurku
    -- p->gdzie->ile;
    p->gdzie = biurko;
}

void wyjmij(worek *w){ // wyjmuje worek w z worka lezacego na biurku
    w->gdzie->ile -= w->moi_synowie->ile;
    w->gdzie = biurko;
}

int w_ktorym_worku(przedmiot *p){ // sprawdzam ojca listy synow gdzie znajduje sie p
    if(p->gdzie->ojciec == nullptr) return -1;
    else return p->gdzie->ojciec->nr;
}

int w_ktorym_worku(worek *w){ // analogicznie jak przedmiot
    if(w->gdzie->ojciec == nullptr) return -1;
    else return w->gdzie->ojciec->nr;
}

int ile_przedmiotow(worek *w){ // sprawdzam licznik listy synow w
    return w->moi_synowie->ile;
}

void na_odwrot(worek *w){
    int nowe_biurko = biurko->ile, nowe_w = biurko->ile - w->moi_synowie->ile; // tyle beda miec w sobie przedmiotow biurko i w po zamianie
    -- biurko->ile;
    w->gdzie = nullptr; // usuwam w z biurka
    std::swap(w->moi_synowie, biurko); // zamiana;
    w->moi_synowie->ojciec = w;
    biurko->ojciec = nullptr; // ojcowie sie musza zgadzac
    w->gdzie = biurko; // w jest na biurku
    w->moi_synowie ->ile = nowe_w;
    biurko->ile = nowe_biurko;
}

void gotowe(){
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
