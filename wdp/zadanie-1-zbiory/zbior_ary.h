#ifndef _ZBIOR_ARY_H_
#define _ZBIOR_ARY_H_

#include <stdbool.h>

typedef struct ciag{
    long long pocz, kon, mod; // ciag arytmetyczny [pocz, pocz + q, ..., kon] reprezentuje jako [pocz, kon]
    // dla wygody trzymam jeszcze wartosc mod ktora jest rowna mod = pocz % q = kon % q
} ciag;

typedef struct zbior_ary{
  int roz;
  ciag * tab;
  // zbiory arytmetyczne reprezentuje jako tablica maksymalnych rozlacznych ciagow arytmetycznych
  // trzymam je posortowane na poczatek po modulo, a potem tak jak sie standardowo porownuje przedzialy (wczesniejszy na osi wczesniej)
  // roz to realny rozmiar tego zbioru czyli ile elementow tablicy jest zajete
} zbior_ary;

void zaalokuj(ciag ** tmp, int n);

void stworz_ciag(zbior_ary *tmp, int n); // to mi tworzy tablice n-elementowa w moim strukcie

void wyczysc_zbior(zbior_ary *tmp);

void wrzuc(int *x, int *y, ciag *X, ciag *Y);


bool porownaj(ciag a, ciag b); // ta funkcja mi zwraca ktory ciag arytmetyczny jest wczesniej na osi

/* Najlepiej nie modyfikowac nic ponizej. */

// Funkcja daje w wyniku zbior reprezentujacy ciag arytmetyczny o elemencie poczatkowym a, końcowym b i roznicy q>0, tj. {a,a+q,...,b}.
// Mozesz zalozyc, ze we wszystkich zapytaniach w danym tescie wartosc q bedzie taka sama.
// Mozesz zalozyc, ze ta funkcja zostanie wywolana jako pierwsza.
zbior_ary ciag_arytmetyczny(int a, int q, int b);

// Daje w wyniku zbior zlozony z samego elementu a, tj. {a}.
zbior_ary singleton(int a);

// Daje w wyniku zbior reprezentujacy teoriomnogosciowa sume zbiorow A i B.
zbior_ary suma(zbior_ary A, zbior_ary B);

// Daje w wyniku zbior reprezentujacy czesc wspolna zbiorow A i B.
zbior_ary iloczyn(zbior_ary A, zbior_ary B);

// Daje w wyniku zbior A \ B.
zbior_ary roznica(zbior_ary A, zbior_ary B);

// Daje w wyniku true wtw. gdy liczba b nalezy do zbioru A.
bool nalezy(zbior_ary A, int b);

// Wynikiem funkcji jest liczba elementow w zbiorze A.
unsigned moc(zbior_ary A);

// Wynikiem funkcji jest Ary_q(A), czyli minimalna liczba parami rozlacznych ciagow arytmetycznych o roznicy q, ktorych suma jest zbior A.
unsigned ary(zbior_ary A);

#endif
