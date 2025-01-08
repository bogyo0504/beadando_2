//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_PIPELINE_H
#define BEADANDO_II_PIPELINE_H


#include <QMap>
#include <QLinkedList>
#include <QStack>
#include "Grid.h"
#include "Tile.h"
#include "GridPosition.h"
#include "Stock.h"
#include "BuildState.h"
#include <memory>
#include <QHash>

/**
 * A PipeLine reprezentál egy (parciálisan) elkészült játéktáblát.
 * A PipeLine nem immutábilis folyamatosan épül és kerülnek rá csempék, illetve ki belőle
 */
class PipeLine {
private:
    const Grid grid;
    /**
     * A tiles tárolja a csempéket a pozíciójuk alapján.
     * A QHash haszálata gyorsítja valamennyire a keresést.
     */
    QHash<GridPosition, Tile> tiles;
    /**
     * Az overlappingEnabled változó azt mondja meg, hogy a PipeLine-ban lehetnek-e egymásra rakható csempék.
     */
    bool overlappingEnabled = true;
public:

    const Grid &getGrid() const;

    /**
     * A put függvény egy csempét helyez el a megadott pozícióra.
     */
    PipeLine &put(const GridPosition position, const Tile &tile);

    explicit PipeLine(const Grid &grid);

    /**
     * Megvizsgálja, hogy az adott pozícióra lehet-e rakni a megadott Tile-t.
     *
     */
    bool canPut(const GridPosition position, const Tile &tile) const;

    /**
     * A [] operátor visszaadja a megadott pozícióban lévő csempét.
     * Ha nincs csempe a pozícióban, akkor PostIt csempét ad vissza.
     */
    Tile operator[](const GridPosition &position) const;

    /**
 * A checkConnection függvény ellenőrzi, hogy a két Tile között van-e kapcsolat.
 * A kapcsolat akkor van, ha az egyik Tile-ban van egy csatlakozási pont, ami a másik Tile-ban van.
 * A csatlakozási pontokat a CSP_TOP, CSP_RIGHT, CSP_BOTTOM, CSP_LEFT konstansokkal lehet ellenőrizni.
 * A függvény visszatérési értéke true, ha van kapcsolat, false, ha nincs.
 */
    bool checkConnection(const GridPosition &position, const GridPositionStep step, bool otherLevel) const;

    /**
     * Megpróbálja alkalmazni az adott BuildState-t a PipeLine-ra, ha sikeres, akkor berak egy
     * pozícióra egy csempét és beilleszti az állapotot a states stackbe vagyis elmozdul lefelé az építési fában.
     * Megjegyzés: a paraméterként megadott BuildState-t "elmozdíthatja" ezzel felgyorsítva az algoritmust.
     */
    BuildState applyBuildState(const BuildState &state, QStack<std::shared_ptr<BuildState>> &states);

    /**
     * Megvizsgálja, hogy az adott pozíció üres-e, tehát nincs-e azon a helyen semmi, még PostIt sem.
     * Az üres pozíció olyan, ahol nincs csempe.
     * Az elérhető pozíció olyan, ahol nincs csempe és a másik stacken sincs csempe.
     */
    bool isEmpty(const GridPosition &position) const;

    /**
     * Törli a megadott pozícióban lévő csempét.
     * Ha a pozícióban nincs csempe, akkor nem történik semmi.
     */
    bool clear(const GridPosition &position);
    /**
     * A getSourcePositions függvény visszaadja az összes forrás helyét a PipeLine-ban.
     */
    QList<GridPosition> getSourcePositions(TileColor color) const;

    /**
     * A getSinkPositions függvény visszaadja az összes fogyasztó helyét PipeLine-ban.
     */
    QList<GridPosition> getSinkPositions(TileColor color) const;

    /**
     * A getValvePositions függvény visszaadja az összes szelep helyét PipeLine-ban.
     */
    QList<GridPosition> getValvePositions() const;

    /**
     * A toQString függvény egy QString-gé alakítja a PipeLine-t.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Később a fájlba való kiírásnál használjuk, illetve a tesztekben.
     * A prefix paramétert a tesztekben használjuk, illetve a PostIt-ek különböző jelölését is.
     */
    QString toQString(bool prefix = false) const;

    /**
     * A fromString függvény egy QString-ből készít egy PipeLine-t.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Később a fájlból való beolvasásnál használjuk, illetve a tesztekben.
     */
    static PipeLine fromString(const QString &string);

    /**
     * A disableOverlap függvény bekapcsolja az overlappingEnabled változót.
     * Az overlappingEnabled változó azt mondja meg, hogy a PipeLine-ban lehetnek-e egymásra rakható csempék.
     */
    void disableOverlap();

    /**
     *A resizeGrid függvény átméretezi a PipeLine-t.
     * Ezt a felhasználó felületnél használjuk, amikor a felhasználó megváltoztatja a játéktábla méretét.
     */
    PipeLine resizeGrid(int width, int height) const;

    /**
     * Mivel az építéskor a teljes PipeLine-t alapnak vesszük beleérve a PostIt-eket is, ezért a PostIt-ek
     * helyére nem rakna a program semmit, ezért az építés előtt removePostIts függvény segítségével eltávolítjuk a PostIt-eket.
     */
    void removePostIts();

private:
    /**
     * A isEmptyAndAvailable függvény megadja, hogy az adott pozíció üres és elérhető-e.
     * Az üres pozíció olyan, ahol nincs csempe.
     * Az elérhető pozíció olyan, ahol nincs csempe és a másik stacken sincs csempe.
     */
    bool isEmptyAndAvailable(GridPosition position);

    /**
     * A findRotation függvény megadja, hogy a megadott pozícióban lévő csempét milyen forgatással lehet elhelyezni.
     * A legelső lehetsége forgatással tér vissza vagy INVALID_ROTATION-t ad vissza, ha nem lehet rátenni a csempét.
     */
    Rotation findRotation(GridPosition position, Tile tile) const;

    /**
     * A hasValidNeighbourghs függvény megadja, hogy minden irányban megnézi, hogy az adott csempe és a szomszédja abban az irányban
     * megfelelően összeilleszthető
     * Nem összeilleszthető, ha a szomszédcsempéről vagy az adott csempéről anyag kifolyik.
     * A függvény nem garantálja teljesen, hogy az elkészült PipeLine helyes lesz, de segít a helyes irányba terelni.
     * Példa eset:
     * Ha alsó jobb kanyart összeköti az alatta lévő függőlegest és a jobbra mellette lévő vízszintessel, de a balra mellette lévő
     * vízszintes nem köti össze a felette lévő függőlegessel, mert olyan nem lesz a készleten, mert bal felső kanyar nincs már a készleten
     * ennek ellenére a jobb alsót rá tudja rakni (kanyarok egymásra rakása (már megint mivel van BAJJJJ))
     * Másik eset lehet, hogy a berakandó függőleges alatt még nincs semmi, akkor be kell tudni rakni, de később nem lesz a készleten
     * olyan amit valójában be lehetne rakni
     * A helyességez majd a Flow és a PipeLine osztály ellenőrzi.
     */
    bool hasValidNeighbourghs(const GridPosition &position, const Tile &tile) const;

    /**
     * A hasValidNeighbourgh függvény megadja, hogy az adott pozícióban lévő csempe és a szomszédja abban az irányban
     * megfelelően összeilleszthető-e.
     * Nem összeilleszthető, ha a szomszédcsempéről vagy az adott csempéről anyag kifolyik.
     */
    bool
    hasValidNeighbourgh(const GridPosition &position, const Tile &tile, GridPositionStep step, int selfConnectionMask,
                        int otherConnectionMask) const;
};


#endif //BEADANDO_II_PIPELINE_H
