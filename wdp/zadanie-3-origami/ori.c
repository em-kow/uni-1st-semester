// Mikolaj Kowalski
// code review: Mateusz Wesolowski

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define ld double

// jako ze operuje na liczbach zmiennoprzecinkowych,
// rownania i nierownosci sprawdzam z dokladnoscia co do epsilona
const ld eps = 2e-6;

typedef struct{ // struct reprezentujacy punkt plaszczyzny
    ld x, y;
} punkt;

typedef struct{ // struct reprezentujacy prostakat (lewy dolny i prawy gorny rog)
    punkt p1, p2;
} Prostokat;

typedef struct{ // struct reprezentujacy kolo (srodek i promien)
    punkt p; ld r;
} Kolo;

typedef struct{ // struct reprezentujacy zgiecie (index zgietej kartki i dwa punkty przez
                    // ktore przechodzi prosta przez ktora zginamy)
    int k; punkt p1, p2;
} Zgiecie;

typedef struct{ // kartka - moze miec ona jeden z trzech typow - prostokat, kolo, zgiecie
    char typ;
    union{
        Prostokat prostokat;
        Kolo kolo;
        Zgiecie zgiecie;
    };
} kartka;

bool w_prostokacie(punkt p, Prostokat prost){ // sprawdzenie czy punkt jest w prostokacie
    // czyli czy wspolrzedne x i y sie zgadzaja (sa w dobrym przedziale)
    if(p.x + eps >= prost.p1.x && p.x <= prost.p2.x + eps &&
        eps + p.y >= prost.p1.y && p.y <= prost.p2.y + eps) return 1;
    return 0;
}

bool w_kole(punkt p, Kolo kol){ // sprawdzenie czy punkt jest w kole
    // czy kwadrat odleglosci punktu od srodka jest mniejsza rowna kwadratowi promienia
    if((p.x - kol.p.x) * (p.x - kol.p.x) + (p.y - kol.p.y) * (p.y - kol.p.y) <= kol.r * kol.r + eps){
        return 1;
    }
    return 0;
}

int polozenie(punkt p, punkt p1, punkt p2){ // sprawdzenie po ktorej stronie prostej jest punkt
    // czyli badamy znak iloczynu wektorowego  wektorow [p1, p2] i [p, p1]
    ld wyznacznik = (p2.x - p1.x) * (p.y - p1.y) - 
        (p.x - p1.x) * (p2.y - p1.y);
    if(wyznacznik > eps) return 1;
    if(wyznacznik < -eps) return -1;
    return 0;
}

punkt odbicie(punkt p, punkt p1, punkt p2){ // szukamy odbicie punktu p wzgledem prostej [p1, p2]
    ld dx = p2.x - p1.x, dy = p2.y - p1.y; // niech v = p2 - p1 = [dx, dy];
    ld dlugosc = dx * dx + dy * dy; // dlugosc do kwadratu v
    // jako ze kazdy punkt na prostej [p1, p2] to p1 + t * v,
    // aby znalezc rzut p na [p1, p2] musimy znalezc t aby [p, p1 + t * v] bylo prostopadle do [p1, p2]
    // z warunku prostopadlosci mamy rownosc (p1 + t * v - p) * v = 0
    // co implikuje t = ((p - p1) * v) / (v * v)
    // mozemy juz wyliczyc t uzywajac wlasnosci iloczynu skalarnego wektorow
    ld t = ((p.x - p1.x) * dx + (p.y - p1.y) * dy) / dlugosc;
    // zatem mozemy juz wyliczyc punkt p zrzutowany na prosta p1, p2
    punkt rzut = (punkt){p1.x + t * dx, p1.y + t * dy};
    // oraz odbicie tego punktu przez ta prosta, bo wiemy ze odbity to bedzie taki punkt,
    // ze rzut jest srodkiem odcinka p, odbity
    punkt odbity = (punkt){2 * rzut.x - p.x, 2 * rzut.y - p.y};
    return odbity;
}

int wynik(int k, punkt p, kartka *kartki){ // funkcja zwraca ile razy punkt p przebije kartke o indexie k
    if(kartki[k].typ == 'P'){
        // jesli kartka jest prostokatem to jesli punkt lezy w tym prostokacie to przebija kartke raz
        // jak nie lezy w tym prostokacie to zero razy
        if(w_prostokacie(p, kartki[k].prostokat)) return 1;
        return 0;
    }
    if(kartki[k].typ == 'K'){
        // jesli kartka jest kolem to jesli punkt lezy w tym kole to przebija kartke raz
        // jak nie lezy w tym kole to zero razy
        if(w_kole(p, kartki[k].kolo)) return 1;
        return 0;
    }
    // skoro wiemy ze kartka nie jest ani prostokatem ani kolem, to jest zgieciem
    // czyli zgieciem innej kartki (nazwijmy ja poprzednia kartka) przez jakas prosta
    // sprawdzamy polozenie tego punktu wzgledem tej prostej
    int pol = polozenie(p, kartki[k].zgiecie.p1, kartki[k].zgiecie.p2);
    // pol = -1 <-> punkt na prawo od prostej,    pol = 0 <-> na prostej,    pol = 1 <-> na lewo
    if(pol == -1){ // jesli punkt lezy na prawo od tej prostej to po zgieciu poprzedniej kartki
        // przez ta prosta punkt nie bedzie w ogole przebijac nowej odbitej kartki
        return 0;
    }
    if(pol == 0){ // jesli punkt lezy dokladnie na tej prostej to po zgieciu poprzedniej kartki
        // przez ta prosta punkt bedzie przebijac nowa odbita kartke dokladnie tyle samo razy co przebijal poprzednia
        return wynik(kartki[k].zgiecie.k, p, kartki);
    }
    // jesli punkt lezy po lewej stronie od tej prostej to po zgieciu poprzedniej kartki
    // przez ta prosta punkt bedzie przebijac nowa kartke dokladnie tyle razy co przebijal poprzednia kartke
    // plus tyle razy co jego odbicie wzgledem prostej przebijalo poprzednia kartke
    // (bo odbicie sie nalozy na ten punkt po odbiciu calej kartki przez prosta)
    punkt p_odbity = odbicie(p, kartki[k].zgiecie.p1, kartki[k].zgiecie.p2);
    return wynik(kartki[k].zgiecie.k, p, kartki) + wynik(kartki[k].zgiecie.k, p_odbity, kartki);
}

int main(){
    int n, q; 
    if(scanf("%d%d", &n, &q) != 2){
        fprintf(stderr, "Nie udalo sie wczytac liczby kartek i liczby zapytan\n");
        return 1;
    }
    kartka *kartki = malloc((size_t)n * sizeof(kartka));
    assert(kartki != NULL);
    for(int i = 0; i < n; i ++){
        char typ; // wczytujemy typ kartki
        if(scanf(" %c", &typ) != 1){
            fprintf(stderr, "Nie udalo sie wczytac typu kartki\n");
            return 1;
        }
        kartki[i].typ = typ;
        // w zaleznosci od typu wczytujemy dalsze dane
        if(typ == 'P'){
            ld x1, x2, y1, y2; 
            if(scanf("%lf%lf%lf%lf",&x1,&y1,&x2,&y2) != 4){
                fprintf(stderr, "Nie udalo sie wczytac wspolrzednych prostokata\n");
                return 1;
            }
            kartki[i].prostokat = (Prostokat){{x1, y1}, {x2, y2}};
        }
        if(typ == 'K'){
            ld x, y, r; 
            if(scanf("%lf%lf%lf", &x, &y, &r) != 3){
                fprintf(stderr, "Nie udalo sie wspolrzednych srodka i promienia kola\n");
                return 1;
            };
            kartki[i].kolo = (Kolo){{x, y}, r};
        }
        if(typ == 'Z'){
            int k; ld x1, y1, x2, y2; 
            if(scanf("%d%lf%lf%lf%lf", &k, &x1, &y1, &x2, &y2) != 5){
                fprintf(stderr, "Nie udalo sie wczytac indexu zginanej kartki i punktow na prostej wzgledem ktorej zginamy\n");
                return 1;
            }
            -- k; // kartki indexuje od 0 do n - 1 !!!
            kartki[i].zgiecie = (Zgiecie){k, {x1, y1}, {x2, y2}};
        }
    }
    while(q --){
        int k; punkt p;
        if(scanf("%d%lf%lf", &k, &p.x, &p.y) != 3){
            fprintf(stderr, "Nie udalo sie wczytac indexu kartki i wspolrzednych punktu\n");
            return 1;
        };
        -- k; // kartki indexuje od 0 do n - 1 !!!
        printf("%d\n", wynik(k, p, kartki));
    }
    free(kartki);
    return 0;
}
