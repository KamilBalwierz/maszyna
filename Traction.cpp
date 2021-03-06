/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/
/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others

*/

#include "system.hpp"
#include "classes.hpp"
#pragma hdrstop

#include "Traction.h"
#include "mctools.hpp"
#include "Globals.h"
#include "Usefull.h"
#include "TractionPower.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
/*

=== Koncepcja dwustronnego zasilania sekcji sieci trakcyjnej, Ra 2014-02 ===
0. Ka�de prz�s�o sieci mo�e mie� wpisan� nazw� zasilacza, a tak�e napi�cie
nominalne i maksymalny pr�d, kt�re stanowi� redundancj� do danych zasilacza.
Rezystancja mo�e si� zmienia�, materia� i grubo�� drutu powinny by� wsp�lny
dla segmentu. Podanie punkt�w umo�liwia ��czenie prz�se� w listy dwukierunkowe,
co usprawnia wyszukiwania kolejnych przese� podczas jazdy. Dla bie�ni wsp�lnej
powinna by� podana nazwa innego prz�s�a w parametrze "parallel". Wska�niki
na prz�s�a bie�ni wsp�lnej maj� by� uk�adane w jednokierunkowy pier�cie�.
1. Problemem jest ustalenie topologii sekcji dla linii dwutorowych. Nad ka�dym
torem powinna znajdowa� si� oddzielna sekcja sieci, aby mog�a zosta� od��czona
w przypadku zwarcia. Sekcje nad r�wnoleg�ymi torami s� r�wnie� ��czone
r�wnolegle przez kabiny sekcyjne, co zmniejsza p�yn�ce pr�dy i spadki napi��.
2. Drugim zagadnieniem jest zasilanie sekcji jednocze�nie z dw�ch stron, czyli
sekcja musi mie� swoj� nazw� oraz wskazanie dw�ch zasilaczy ze wskazaniem
geograficznym ich po�o�enia. Dodatkow� trudno�ci� jest brak po��czenia
pomi�dzy segmentami napr�ania. Podsumowuj�c, ka�dy segment napr�ania powinien
mie� przypisanie do sekcji zasilania, a dodatkowo skrajne segmenty powinny
wskazywa� dwa r�ne zasilacze.
3. Zasilaczem sieci mo�e by� podstacja, kt�ra w wersji 3kV powinna generowa�
pod obci��eniem napi�cie maksymalne rz�du 3600V, a spadek napi�cia nast�puje
na jej rezystancji wewn�trznej oraz na przewodach trakcyjnych. Zasilaczem mo�e
by� r�wnie� kabina sekcyjna, kt�ra jest zasilana z podstacji poprzez przewody
trakcyjne.
4. Dla uproszczenia mo�na przyj��, �e zasilanie pojazdu odbywa� si� b�dzie z
dw�ch s�siednich podstacji, pomi�dzy kt�rymi mo�e by� dowolna liczba kabin
sekcyjnych. W przypadku wy��czenia jednej z tych podstacji, zasilanie mo�e
by� pobierane z kolejnej. ��cznie nale�y rozwa�a� 4 najbli�sze podstacje,
przy czym do oblicze� mo�na przyjmowa� 2 z nich.
5. Prz�s�a sieci s� ��czone w list� dwukierunkow�, wi�c wystarczy nazw�
sekcji wpisa� w jednym z nich, wpisanie w ka�dym nie przeszkadza.
Alternatywnym sposobem ��czenia segment�w napr�ania mo�e by� wpisywanie
nazw prz�se� jako "parallel", co mo�e by� uci��liwe dla autor�w scenerii.
W skrajnych prz�s�ach nale�a�oby dodatkowo wpisa� nazw� zasilacza, b�dzie
to jednocze�nie wskazanie prz�s�a, do kt�rego pod��czone s� przewody
zasilaj�ce. Konieczne jest odr�nienie nazwy sekcji od nazwy zasilacza, co
mo�na uzyska� r�nicuj�c ich nazwy albo np. specyficznie ustawiaj�c warto��
pr�du albo napi�cia prz�s�a.
6. Je�li dany segment napr�ania jest wsp�lny dla dw�ch sekcji zasilania,
to jedno z prz�se� musi mie� nazw� "*" (gwiazdka), co b�dzie oznacza�o, �e
ma zamontowany izolator. Dla uzyskania efekt�w typu �uk elektryczny, nale�a�o
by wskaza� po�o�enie izolatora i jego d�ugo�� (ew. typ).
7. R�wnie� w parametrach zasilacza nale�a�o by okre�li�, czy jest podstacj�,
czy jedynie kabin� sekcyjn�. R�ni� si� one b�d� fizyk� dzia�ania.
8. Dla zbudowanej topologii sekcji i zasilaczy nale�a�o by zbudowa� dynamiczny
schemat zast�pczy. Dynamika polega na wy��czaniu sekcji ze zwarciem oraz
przeci��onych podstacji. Musi by� te� mo�liwo�� wy��czenia sekcji albo
podstacji za pomoc� eventu.
9. Dla ka�dej sekcji musi by� tworzony obiekt, wskazuj�cy na podstacje
zasilaj�ce na ko�cach, stan w��czenia, zwarcia, przepi�cia. Do tego obiektu
musi wskazywa� ka�de prz�s�o z aktywnym zasilaniem.

          z.1                  z.2             z.3
   -=-a---1*1---c-=---c---=-c--2*2--e---=---e-3-*-3--g-=-
   -=-b---1*1---d-=---d---=-d--2*2--f---=---e-3-*-3--h-=-

   nazwy sekcji (@): a,b,c,d,e,f,g,h
   nazwy zasilaczy (#): 1,2,3
   prz�s�o z izolatorem: *
   prz�s�a bez wskazania nazwy sekcji/zasilacza: -
   segment napr�zania: =-x-=
   segment napr�ania z izolatorem: =---@---#*#---@---=
   segment napr�ania bez izolatora: =--------@------=

Obecnie brak nazwy sekcji nie jest akceptowany i ka�de prz�s�o musi mie� wpisan�
jawnie nazw� sekcji, ewentualnie nazw� zasilacza (zostanie zast�piona wskazaniem
sekcji z s�siedniego prz�s�a).
*/

TTraction::TTraction()
{
    pPoint1 = pPoint2 = pPoint3 = pPoint4 = vector3(0, 0, 0);
    // vFront=vector3(0,0,1);
    // vUp=vector3(0,1,0);
    // vLeft=vector3(1,0,0);
    fHeightDifference = 0;
    iNumSections = 0;
    iLines = 0;
    //    dwFlags= 0;
    Wires = 2;
    //    fU=fR= 0;
    uiDisplayList = 0;
    asPowerSupplyName = "";
    //    mdPole= NULL;
    //    ReplacableSkinID= 0;
    hvNext[0] = hvNext[1] = NULL;
    iLast = 1; //�e niby ostatni drut
    psPowered = psPower[0] = psPower[1] = NULL; // na pocz�tku zasilanie nie pod��czone
    psSection = NULL; // na pocz�tku nie pod��czone
    hvParallel = NULL; // normalnie brak bie�ni wsp�lnej
    fResistance[0] = fResistance[1] = -1.0; // trzeba dopiero policzy�
    iTries = 0; // ile razy pr�bowa� pod��czy�, ustawiane p�niej
}

TTraction::~TTraction()
{
    if (!Global::bUseVBO)
        glDeleteLists(uiDisplayList, 1);
}

void TTraction::Optimize()
{
    if (Global::bUseVBO)
        return;
    uiDisplayList = glGenLists(1);
    glNewList(uiDisplayList, GL_COMPILE);

    glBindTexture(GL_TEXTURE_2D, 0);
    //    glColor3ub(0,0,0); McZapkie: to do render

    //    glPushMatrix();
    //    glTranslatef(pPosition.x,pPosition.y,pPosition.z);

    if (Wires != 0)
    {
        // Dlugosc odcinka trakcji 'Winger
        double ddp = hypot(pPoint2.x - pPoint1.x, pPoint2.z - pPoint1.z);

        if (Wires == 2)
            WireOffset = 0;
        // Przewoz jezdny 1 'Marcin
        glBegin(GL_LINE_STRIP);
        glVertex3f(pPoint1.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pPoint1.y,
                   pPoint1.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
        glVertex3f(pPoint2.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pPoint2.y,
                   pPoint2.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
        glEnd();
        // Nie wiem co 'Marcin
        vector3 pt1, pt2, pt3, pt4, v1, v2;
        v1 = pPoint4 - pPoint3;
        v2 = pPoint2 - pPoint1;
        float step = 0;
        if (iNumSections > 0)
            step = 1.0f / (float)iNumSections;
        float f = step;
        float mid = 0.5;
        float t;

        // Przewod nosny 'Marcin
        if (Wires != 1)
        {
            glBegin(GL_LINE_STRIP);
            glVertex3f(pPoint3.x, pPoint3.y, pPoint3.z);
            for (int i = 0; i < iNumSections - 1; i++)
            {
                pt3 = pPoint3 + v1 * f;
                t = (1 - fabs(f - mid) * 2);
                if ((Wires < 4) || ((i != 0) && (i != iNumSections - 2)))
                    glVertex3f(pt3.x, pt3.y - sqrt(t) * fHeightDifference, pt3.z);
                f += step;
            }
            glVertex3f(pPoint4.x, pPoint4.y, pPoint4.z);
            glEnd();
        }

        // Drugi przewod jezdny 'Winger
        if (Wires > 2)
        {
            glBegin(GL_LINE_STRIP);
            glVertex3f(pPoint1.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pPoint1.y,
                       pPoint1.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
            glVertex3f(pPoint2.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pPoint2.y,
                       pPoint2.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
            glEnd();
        }

        f = step;

        if (Wires == 4)
        {
            glBegin(GL_LINE_STRIP);
            glVertex3f(pPoint3.x, pPoint3.y - 0.65f * fHeightDifference, pPoint3.z);
            for (int i = 0; i < iNumSections - 1; i++)
            {
                pt3 = pPoint3 + v1 * f;
                t = (1 - fabs(f - mid) * 2);
                glVertex3f(
                    pt3.x,
                    pt3.y - sqrt(t) * fHeightDifference -
                        ((i == 0) || (i == iNumSections - 2) ? 0.25f * fHeightDifference : +0.05),
                    pt3.z);
                f += step;
            }
            glVertex3f(pPoint4.x, pPoint4.y - 0.65f * fHeightDifference, pPoint4.z);
            glEnd();
        }

        f = step;

        // Przewody pionowe (wieszaki) 'Marcin, poprawki na 2 przewody jezdne 'Winger
        if (Wires != 1)
        {
            glBegin(GL_LINES);
            for (int i = 0; i < iNumSections - 1; i++)
            {
                float flo, flo1;
                flo = (Wires == 4 ? 0.25f * fHeightDifference : 0);
                flo1 = (Wires == 4 ? +0.05 : 0);
                pt3 = pPoint3 + v1 * f;
                pt4 = pPoint1 + v2 * f;
                t = (1 - fabs(f - mid) * 2);
                if ((i % 2) == 0)
                {
                    glVertex3f(pt3.x, pt3.y - sqrt(t) * fHeightDifference -
                                          ((i == 0) || (i == iNumSections - 2) ? flo : flo1),
                               pt3.z);
                    glVertex3f(pt4.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pt4.y,
                               pt4.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
                }
                else
                {
                    glVertex3f(pt3.x, pt3.y - sqrt(t) * fHeightDifference -
                                          ((i == 0) || (i == iNumSections - 2) ? flo : flo1),
                               pt3.z);
                    glVertex3f(pt4.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset, pt4.y,
                               pt4.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset);
                }
                if ((Wires == 4) && ((i == 1) || (i == iNumSections - 3)))
                {
                    glVertex3f(pt3.x, pt3.y - sqrt(t) * fHeightDifference - 0.05, pt3.z);
                    glVertex3f(pt3.x, pt3.y - sqrt(t) * fHeightDifference, pt3.z);
                }
                // endif;
                f += step;
            }
            glEnd();
        }
        glEndList();
    }
}
/*
void TTraction::InitCenter(vector3 Angles, vector3 pOrigin)
{
    pPosition= (pPoint2+pPoint1)*0.5f;
    fSquaredRadius= SquareMagnitude((pPoint2-pPoint1)*0.5f);
} */

void TTraction::RenderDL(float mgn) // McZapkie: mgn to odleglosc od obserwatora
{
    // McZapkie: ustalanie przezroczystosci i koloru linii:
    if (Wires != 0 && !TestFlag(DamageFlag, 128)) // rysuj jesli sa druty i nie zerwana
    {
        // glDisable(GL_LIGHTING); //aby nie u�ywa�o wektor�w normalnych do kolorowania
        glColor4f(0, 0, 0, 1); // jak nieznany kolor to czarne nieprzezroczyste
        if (!Global::bSmoothTraction)
            glDisable(GL_LINE_SMOOTH); // na liniach kiepsko wygl�da - robi gradient
        float linealpha = 5000 * WireThickness / (mgn + 1.0); //*WireThickness
        if (linealpha > 1.2)
            linealpha = 1.2; // zbyt grube nie s� dobre
        glLineWidth(linealpha);
        if (linealpha > 1.0)
            linealpha = 1.0;
        // McZapkie-261102: kolor zalezy od materialu i zasniedzenia
        float r, g, b;
        switch (Material)
        { // Ra: kolory podzieli�em przez 2, bo po zmianie ambient za jasne by�y
        // trzeba uwzgl�dni� kierunek �wiecenia S�o�ca - tylko ze S�o�cem wida� kolor
        case 1:
            if (TestFlag(DamageFlag, 1))
            {
                r = 0.00000;
                g = 0.32549;
                b = 0.2882353; // zielona mied�
            }
            else
            {
                r = 0.35098;
                g = 0.22549;
                b = 0.1; // czerwona mied�
            }
            break;
        case 2:
            if (TestFlag(DamageFlag, 1))
            {
                r = 0.10;
                g = 0.10;
                b = 0.10; // czarne Al
            }
            else
            {
                r = 0.25;
                g = 0.25;
                b = 0.25; // srebrne Al
            }
            break;
        // tymczasowo pokazanie zasilanych odcink�w
        case 4:
            r = 0.5;
            g = 0.5;
            b = 1.0;
            break; // niebieskie z pod��czonym zasilaniem
        case 5:
            r = 1.0;
            g = 0.0;
            b = 0.0;
            break; // czerwone z pod��czonym zasilaniem 1
        case 6:
            r = 0.0;
            g = 1.0;
            b = 0.0;
            break; // zielone z pod��czonym zasilaniem 2
        case 7:
            r = 1.0;
            g = 1.0;
            b = 0.0;
            break; //��te z pod��czonym zasilaniem z obu stron
        }
        if (DebugModeFlag)
            if (hvParallel)
            { // je�li z bie�ni� wsp�ln�, to dodatkowo przyciemniamy
                r *= 0.6;
                g *= 0.6;
                b *= 0.6;
            }
        r *= Global::ambientDayLight[0]; // w zale�no�ci od koloru swiat�a
        g *= Global::ambientDayLight[1];
        b *= Global::ambientDayLight[2];
        if (linealpha > 1.0)
            linealpha = 1.0; // trzeba ograniczy� do <=1
        glColor4f(r, g, b, linealpha);
        if (!uiDisplayList)
            Optimize(); // generowanie DL w miar� potrzeby
        glCallList(uiDisplayList);
        glLineWidth(1.0);
        glEnable(GL_LINE_SMOOTH);
        // glEnable(GL_LIGHTING); //bez tego si� modele nie o�wietlaj�
    }
}

int TTraction::RaArrayPrepare()
{ // przygotowanie tablic do skopiowania do VBO (zliczanie wierzcho�k�w)
    // if (bVisible) //o ile w og�le wida�
    switch (Wires)
    {
    case 1:
        iLines = 2;
        break;
    case 2:
        iLines = iNumSections ? 4 * (iNumSections)-2 + 2 : 4;
        break;
    case 3:
        iLines = iNumSections ? 4 * (iNumSections)-2 + 4 : 6;
        break;
    case 4:
        iLines = iNumSections ? 4 * (iNumSections)-2 + 6 : 8;
        break;
    default:
        iLines = 0;
    }
    // else iLines=0;
    return iLines;
};

void TTraction::RaArrayFill(CVertNormTex *Vert)
{ // wype�nianie tablic VBO
    CVertNormTex *old = Vert;
    double ddp = hypot(pPoint2.x - pPoint1.x, pPoint2.z - pPoint1.z);
    if (Wires == 2)
        WireOffset = 0;
    // jezdny
    Vert->x = pPoint1.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
    Vert->y = pPoint1.y;
    Vert->z = pPoint1.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
    ++Vert;
    Vert->x = pPoint2.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
    Vert->y = pPoint2.y;
    Vert->z = pPoint2.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
    ++Vert;
    // Nie wiem co 'Marcin
    vector3 pt1, pt2, pt3, pt4, v1, v2;
    v1 = pPoint4 - pPoint3;
    v2 = pPoint2 - pPoint1;
    float step = 0;
    if (iNumSections > 0)
        step = 1.0f / (float)iNumSections;
    float f = step;
    float mid = 0.5;
    float t;
    // Przewod nosny 'Marcin
    if (Wires > 1)
    { // lina no�na w kawa�kach
        Vert->x = pPoint3.x;
        Vert->y = pPoint3.y;
        Vert->z = pPoint3.z;
        ++Vert;
        for (int i = 0; i < iNumSections - 1; i++)
        {
            pt3 = pPoint3 + v1 * f;
            t = (1 - fabs(f - mid) * 2);
            Vert->x = pt3.x;
            Vert->y = pt3.y - sqrt(t) * fHeightDifference;
            Vert->z = pt3.z;
            ++Vert;
            Vert->x = pt3.x; // drugi raz, bo nie jest line_strip
            Vert->y = pt3.y - sqrt(t) * fHeightDifference;
            Vert->z = pt3.z;
            ++Vert;
            f += step;
        }
        Vert->x = pPoint4.x;
        Vert->y = pPoint4.y;
        Vert->z = pPoint4.z;
        ++Vert;
    }
    // Drugi przewod jezdny 'Winger
    if (Wires == 3)
    {
        Vert->x = pPoint1.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
        Vert->y = pPoint1.y;
        Vert->z = pPoint1.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
        ++Vert;
        Vert->x = pPoint2.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
        Vert->y = pPoint2.y;
        Vert->z = pPoint2.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
        ++Vert;
    }
    f = step;
    // Przewody pionowe (wieszaki) 'Marcin, poprawki na 2 przewody jezdne 'Winger
    if (Wires > 1)
    {
        for (int i = 0; i < iNumSections - 1; i++)
        {
            pt3 = pPoint3 + v1 * f;
            pt4 = pPoint1 + v2 * f;
            t = (1 - fabs(f - mid) * 2);
            Vert->x = pt3.x;
            Vert->y = pt3.y - sqrt(t) * fHeightDifference;
            Vert->z = pt3.z;
            ++Vert;
            if ((i % 2) == 0)
            {
                Vert->x = pt4.x - (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
                Vert->y = pt4.y;
                Vert->z = pt4.z - (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
            }
            else
            {
                Vert->x = pt4.x + (pPoint2.z / ddp - pPoint1.z / ddp) * WireOffset;
                Vert->y = pt4.y;
                Vert->z = pt4.z + (-pPoint2.x / ddp + pPoint1.x / ddp) * WireOffset;
            }
            ++Vert;
            f += step;
        }
    }
    if ((Vert - old) != iLines)
        WriteLog("!!! Wygenerowano punkt�w " + AnsiString(Vert - old) + ", powinno by� " +
                 AnsiString(iLines));
};

void TTraction::RenderVBO(float mgn, int iPtr)
{ // renderowanie z u�yciem VBO
    if (Wires != 0 && !TestFlag(DamageFlag, 128)) // rysuj jesli sa druty i nie zerwana
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_LIGHTING); // aby nie u�ywa�o wektor�w normalnych do kolorowania
        glColor4f(0, 0, 0, 1); // jak nieznany kolor to czarne nieprzezroczyste
        if (!Global::bSmoothTraction)
            glDisable(GL_LINE_SMOOTH); // na liniach kiepsko wygl�da - robi gradient
        float linealpha = 5000 * WireThickness / (mgn + 1.0); //*WireThickness
        if (linealpha > 1.2)
            linealpha = 1.2; // zbyt grube nie s� dobre
        glLineWidth(linealpha);
        // McZapkie-261102: kolor zalezy od materialu i zasniedzenia
        float r, g, b;
        switch (Material)
        { // Ra: kolory podzieli�em przez 2, bo po zmianie ambient za jasne by�y
        // trzeba uwzgl�dni� kierunek �wiecenia S�o�ca - tylko ze S�o�cem wida� kolor
        case 1:
            if (TestFlag(DamageFlag, 1))
            {
                r = 0.00000;
                g = 0.32549;
                b = 0.2882353; // zielona mied�
            }
            else
            {
                r = 0.35098;
                g = 0.22549;
                b = 0.1; // czerwona mied�
            }
            break;
        case 2:
            if (TestFlag(DamageFlag, 1))
            {
                r = 0.10;
                g = 0.10;
                b = 0.10; // czarne Al
            }
            else
            {
                r = 0.25;
                g = 0.25;
                b = 0.25; // srebrne Al
            }
            break;
        // tymczasowo pokazanie zasilanych odcink�w
        case 4:
            r = 0.5;
            g = 0.5;
            b = 1.0;
            break; // niebieskie z pod��czonym zasilaniem
        case 5:
            r = 1.0;
            g = 0.0;
            b = 0.0;
            break; // czerwone z pod��czonym zasilaniem 1
        case 6:
            r = 0.0;
            g = 1.0;
            b = 0.0;
            break; // zielone z pod��czonym zasilaniem 2
        case 7:
            r = 1.0;
            g = 1.0;
            b = 0.0;
            break; //��te z pod��czonym zasilaniem z obu stron
        }
        r = r * Global::ambientDayLight[0]; // w zaleznosci od koloru swiatla
        g = g * Global::ambientDayLight[1];
        b = b * Global::ambientDayLight[2];
        if (linealpha > 1.0)
            linealpha = 1.0; // trzeba ograniczy� do <=1
        glColor4f(r, g, b, linealpha);
        glDrawArrays(GL_LINES, iPtr, iLines);
        glLineWidth(1.0);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_LIGHTING); // bez tego si� modele nie o�wietlaj�
    }
};

int TTraction::TestPoint(vector3 *Point)
{ // sprawdzanie, czy prz�s�a mo�na po��czy�
    if (!hvNext[0])
        if (pPoint1.Equal(Point))
            return 0;
    if (!hvNext[1])
        if (pPoint2.Equal(Point))
            return 1;
    return -1;
};

void TTraction::Connect(int my, TTraction *with, int to)
{ //��czenie segmentu (with) od strony (my) do jego (to)
    if (my)
    { // do mojego Point2
        hvNext[1] = with;
        iNext[1] = to;
    }
    else
    { // do mojego Point1
        hvNext[0] = with;
        iNext[0] = to;
    }
    if (to)
    { // do jego Point2
        with->hvNext[1] = this;
        with->iNext[1] = my;
    }
    else
    { // do jego Point1
        with->hvNext[0] = this;
        with->iNext[0] = my;
    }
    if (hvNext[0]) // je�li z obu stron pod��czony
        if (hvNext[1])
            iLast = 0; // to nie jest ostatnim
    if (with->hvNext[0]) // temu te�, bo drugi raz ��czenie si� nie nie wykona
        if (with->hvNext[1])
            with->iLast = 0; // to nie jest ostatnim
};

bool TTraction::WhereIs()
{ // ustalenie przedostatnich prz�se�
    if (iLast)
        return (iLast == 1); // ma ju� ustalon� informacj� o po�o�eniu
    if (hvNext[0] ? hvNext[0]->iLast == 1 : false) // je�li poprzedni jest ostatnim
        iLast = 2; // jest przedostatnim
    else if (hvNext[1] ? hvNext[1]->iLast == 1 : false) // je�li nast�pny jest ostatnim
        iLast = 2; // jest przedostatnim
    return (iLast == 1); // ostatnie b�d� dostawa� zasilanie
};

void TTraction::Init()
{ // przeliczenie parametr�w
    vParametric = pPoint2 - pPoint1; // wektor mno�nik�w parametru dla r�wnania parametrycznego
};

void TTraction::ResistanceCalc(int d, double r, TTractionPowerSource *ps)
{ //(this) jest prz�s�em zasilanym, o rezystancji (r), policzy� rezystancj� zast�pcz� s�siednich
    if (d >= 0)
    { // pod��anie we wskazanym kierunku
        TTraction *t = hvNext[d], *p;
        if (ps)
            psPower[d ^ 1] = ps; // pod��czenie podanego
        else
            ps = psPower[d ^ 1]; // zasilacz od przeciwnej strony ni� idzie analiza
        d = iNext[d]; // kierunek
        // double r; //sumaryczna rezystancja
        if (DebugModeFlag) // tylko podczas test�w
            Material = 4; // pokazanie, �e to prz�s�o ma pod��czone zasilanie
        while (t ? !t->psPower[d] : false) // je�li jest jaki� kolejny i nie ma ustalonego zasilacza
        { // ustawienie zasilacza i policzenie rezystancji zast�pczej
            if (DebugModeFlag) // tylko podczas test�w
                if (t->Material != 4) // prz�s�a zasilaj�cego nie modyfikowa�
                {
                    if (t->Material < 4)
                        t->Material = 4; // tymczasowo, aby zmieni�a kolor
                    t->Material |= d ? 2 : 1; // kolor zale�ny od strony, z kt�rej jest zasilanie
                }
            t->psPower[d] = ps; // skopiowanie wska�nika zasilacza od danej strony
            t->fResistance[d] = r; // wpisanie rezystancji w kierunku tego zasilacza
            r += t->fResistivity * Length3(t->vParametric); // doliczenie oporu kolejnego odcinka
            p = t; // zapami�tanie dotychczasowego
            t = p->hvNext[d ^ 1]; // pod��anie w t� sam� stron�
            d = p->iNext[d ^ 1];
            // w przypadku zap�tlenia sieci mo�e si� zawiesi�?
        }
    }
    else
    { // pod��anie w obu kierunkach, mo�na by rekurencj�, ale szkoda zasob�w
        r = 0.5 * fResistivity *
            Length3(vParametric); // powiedzmy, �e w zasilanym prz�le jest po�owa
        if (fResistance[0] == 0.0)
            ResistanceCalc(0, r); // do ty�u (w stron� Point1)
        if (fResistance[1] == 0.0)
            ResistanceCalc(1, r); // do przodu (w stron� Point2)
    }
};

void TTraction::PowerSet(TTractionPowerSource *ps)
{ // pod��czenie prz�s�a do zasilacza
    if (ps->bSection)
        psSection = ps; // ustalenie sekcji zasilania
    else
    { // ustalenie punktu zasilania (nie ma jeszcze po��cze� mi�dzy prz�s�ami)
        psPowered = ps; // ustawienie bezpo�redniego zasilania dla prz�s�a
        psPower[0] = psPower[1] = ps; // a to chyba nie jest dobry pomys�, bo nawet zasilane prz�s�o
        // powinno mie� wskazania na inne
        fResistance[0] = fResistance[1] = 0.0; // a liczy si� tylko rezystancja zasilacza
    }
};

double TTraction::VoltageGet(double u, double i)
{ // pobranie napi�cia na prz�le po pod��czeniu do niego rezystancji (res) - na razie jest to pr�d
    if (!psSection)
        if (!psPowered)
            return NominalVoltage; // jak nie ma zasilacza, to napi�cie podane w prz�le
    // na pocz�tek mo�na za�o�y�, �e wszystkie podstacje maj� to samo napi�cie i nie p�ynie pr�d
    // pomi�dzy nimi
    // dla danego prz�s�a mamy 3 �r�d�a zasilania
    // 1. zasilacz psPower[0] z rezystancj� fResistance[0] oraz jego wewn�trzn�
    // 2. zasilacz psPower[1] z rezystancj� fResistance[1] oraz jego wewn�trzn�
    // 3. zasilacz psPowered z jego wewn�trzn� rezystancj� dla prz�se� zasilanych bezpo�rednio
    double res = (i != 0.0) ? fabs(u / i) : 10000.0;
    if (psPowered)
        return psPowered->CurrentGet(res) *
               res; // yB: dla zasilanego nie baw si� w gwiazdy, tylko bierz bezpo�rednio
    double r0t, r1t, r0g, r1g;
    double u0, u1, i0, i1;
    r0t = fResistance[0]; //�redni pomys�, ale lepsze ni� nic
    r1t = fResistance[1]; // bo nie uwzgl�dnia spadk�w z innych pojazd�w
    if (psPower[0] && psPower[1])
    { // gdy prz�s�o jest zasilane z obu stron - mamy tr�jk�t: res, r0t, r1t
        // yB: Gdy wywali podstacja, to zaczyna si� robi� nieciekawie - napi�cie w sekcji na jednym
        // ko�cu jest r�wne zasilaniu,
        // yB: a na drugim ko�cu jest r�wne 0. Kolejna sprawa to rozr�nienie uszynienia sieci na
        // podstacji/od��czniku (czyli
        // yB: potencja� masy na sieci) od braku zasilania (czyli od��czenie �r�d�a od sieci i brak
        // jego wp�ywu na napi�cie).
        if ((r0t > 0.0) && (r1t > 0.0))
        { // rezystancje w mianowniku nie mog� by� zerowe
            r0g = res + r0t + (res * r0t) / r1t; // przeliczenie z tr�jk�ta na gwiazd�
            r1g = res + r1t + (res * r1t) / r0t;
            // pobierane s� pr�dy dla ka�dej rezystancji, a suma jest mno�ona przez rezystancj�
            // pojazdu w celu uzyskania napi�cia
            i0 = psPower[0]->CurrentGet(r0g); // oddzielnie dla sprawdzenia
            i1 = psPower[1]->CurrentGet(r1g);
            return (i0 + i1) * res;
        }
        else if (r0t >= 0.0)
            return psPower[0]->CurrentGet(res + r0t) * res;
        else if (r1t >= 0.0)
            return psPower[1]->CurrentGet(res + r1t) * res;
        else
            return 0.0; // co z tym zrobi�?
    }
    else if (psPower[0] && (r0t >= 0.0))
    { // je�li odcinek pod��czony jest tylko z jednej strony
        return psPower[0]->CurrentGet(res + r0t) * res;
    }
    else if (psPower[1] && (r1t >= 0.0))
        return psPower[1]->CurrentGet(res + r1t) * res;
    return 0.0; // gdy nie pod��czony wcale?
};
