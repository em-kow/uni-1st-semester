#include "worki.h"
#include <algorithm>

struct elem{ // element listy
    elem* prev = nullptr; // wskaznik na poprzedni
    elem* next = nullptr; // wskaznik na nastepny
    worek* wsk_worek = nullptr; // wskaznik na worek, bedacy tym elementem listy
    przedmiot* wsk_przedmiot = nullptr; // wskaznik na przedmiot, bedacy tym elementem listy
    // zawsze maksymalnie jedno z wsk_worek i wsk_przedmiot nie sa null, bo element listy to albo worek albo przedmiot
};

struct synowie{ // gdy przedstawimy sytuacje jako drzewo, to synami worka lub biurka sa przedmioty i worki ktore sie w nim znajduja
    worek* ojciec = nullptr; // jaki worek jest ojcem tego zbioru synow, ojciec == null to biurko
    elem* head = nullptr; // poczatek listy synow
    elem* tail = nullptr; // koniec listy synow
    int ile = 0; // ile lacznie przedmiotow jest w tym poddrzewie
};

void dodaj_syna(synowie* s, elem* e){ // dodaje syna wierzcholkowi, czyli wrzuca nowy przedmiot lub worek do worka lub na biurko
    e->prev = s->tail;
    e->next = nullptr;
    if (s->tail) s->tail->next = e;
    else s->head = e;
    s->tail = e;
    if(e->wsk_przedmiot != nullptr) ++ s->ile; // jesli wrzucam przedmiot to zwiekszam liczbe przedmiotow
    if(e->wsk_worek != nullptr) s->ile += ((e->wsk_worek)->moi_synowie)->ile;
}

void usun_syna(synowie *s, elem* e){ // usuwam syna wierzcholkowi, czyli wyjmuje przedmiot lub worek z worka lub biurka
    if (e->prev != nullptr) e->prev->next = e->next;
    else s->head = e->next;
    if (e->next != nullptr) e->next->prev = e->prev;
    else s->tail = e->prev;
    e->prev = e->next = nullptr;
    if(e->wsk_przedmiot != nullptr) -- s->ile;
    if(e->wsk_worek != nullptr) s->ile -= ((e->wsk_worek)->moi_synowie)->ile;
}

static synowie* biurko = new synowie; // globalna zmienna reprezentujaca co znajduje sie na biurku

static int ile_workow = 0; // ile aktualnie stworzylem workow (potrzebne do numerowania)

przedmiot* nowy_przedmiot(){
    przedmiot* p = new przedmiot;
    elem* e = new elem;
    e->wsk_przedmiot = p;
    p->gdzie = biurko;
    p->odpowiednik = e;
    dodaj_syna(biurko, e); // dodaje ten przedmiot na biurko
    return p;
}

worek* nowy_worek(){
    worek* w = new worek;
    elem* e = new elem;
    e->wsk_worek = w;
    synowie* moi = new synowie;
    moi->ojciec = w;
    w->nr = (ile_workow ++);
    w->gdzie = biurko;
    w->odpowiednik = e;
    w->moi_synowie = moi;
    dodaj_syna(biurko, e);
    return w;
}

void wloz(przedmiot *co, worek *gdzie){ // wkladam przedmiot co do worka gdzie
    usun_syna(biurko, co->odpowiednik); // usuwam ten przedmiot z listy biurka
    dodaj_syna(gdzie->moi_synowie, co->odpowiednik); // dodaje ten przedmiot do listy tego, co jest w worku
    co->gdzie = gdzie->moi_synowie; // daje workowi informacje wsrod jakich synow jest
    ++ biurko->ile; // mimo usuniecia przedmiotu z biurka, dalej sie on na nim znajduje posrednio
}

void wloz(worek *co, worek *gdzie){ // wkladam worek co do worka gdzie (analogicznie jak przedmiot do worka)
    usun_syna(biurko, co->odpowiednik);
    dodaj_syna(gdzie->moi_synowie, co->odpowiednik);
    co->gdzie = gdzie->moi_synowie;
    biurko->ile += co->moi_synowie->ile;
}

void wyjmij(przedmiot *p){ // wyjmuje przedmiot p z worka lezacego na biurku
    usun_syna(p->gdzie, p->odpowiednik); // usuwam przedmiot z listy worka
    dodaj_syna(biurko, p->odpowiednik); // i dodaje na biurko
    p->gdzie = biurko;
    -- biurko->ile; // przedmiot caly czas sie posrednio znajduje na biurko, a niepotrzebnie zwiekszylismy licznik biurka
}

void wyjmij(worek *w){ // wyjmuje worek w z worka lezacego na biurku, analogicznie jak przedmiot
    usun_syna(w->gdzie, w->odpowiednik);
    dodaj_syna(biurko, w->odpowiednik);
    w->gdzie = biurko;
    biurko->ile -= w->moi_synowie->ile;
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
    usun_syna(biurko, w->odpowiednik); // na poczatek wyjmuje ten worek z biurka, aby przy zamianie list nie bylo sytuacji ze W jest w W
    std::swap(w->moi_synowie, biurko); // zamieniam listy synow, bo jedynie to sie zmienia przy na_odwrot
    w->moi_synowie->ojciec = w;
    biurko->ojciec = nullptr;
    // ustalam aby ojcowie list synow sie zgadzali
    dodaj_syna(biurko, w->odpowiednik);
    w->gdzie = biurko;
    // z powrotem wrzucam W na biurko
}

void sprzataj_synow(synowie* s){ // korzystajac ze struktury drzewa puszczam rekurencyjny algorytm zwalniajacy pamiec
    elem* e = s->head;
    while(e != nullptr){
        elem* next = e->next;
        if (e->wsk_przedmiot != nullptr) delete e->wsk_przedmiot;
        else if(e->wsk_worek){
            sprzataj_synow(e->wsk_worek->moi_synowie);
            delete e->wsk_worek->moi_synowie;
            delete e->wsk_worek;
        }
        delete e;
        e = next;
    }
    s->head = s->tail = nullptr;
}

void gotowe() {
    sprzataj_synow(biurko); // biurko jest korzeniem drzewa
    delete biurko;
}
