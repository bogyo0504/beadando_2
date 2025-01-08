//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_TILE_H
#define BEADANDO_II_TILE_H

#include <QString>
#include "GridPosition.h"

#define ROTATION_MASK 3
#define INVALID_ROTATION (-1)
typedef short int Rotation;

#define CSP_TOP 8
#define CSP_RIGHT 4
#define CSP_BOTTOM 2
#define CSP_LEFT 1

/**
 * A TileType felsorolás a csempe típusát reprezentálja.
 * A NORMAL a sima csempe, a SOURCE a forrás, a SINK a nyelő, a VALVE a szelep.
 */
enum TileType {
    NORMAL, SOURCE, SINK, VALVE
};

/**
 * A TileColor felsorolás a csempe színét reprezentálja.
 * A NONE jelentése, hogy a csempének nincs szín, a RED a piros, a CIAN a cián, a BLUE a kék, a GREEN a zöld, a YELLOW a sárga.
 */
enum TileColor {
    NONE, RED, CIAN, BLUE, GREEN, YELLOW
};


/**
 * A Tile class reprezentálja a játéktábla egy mezőjét.
 * A Tile osztály rendezett, a rendezés a csatlakoizási pontok számával, majd  a típussal és utána a színnel történik.
 * Tile osztály teljesen rendezett halmaz :D
 * A sarkok vannak utoljára, mert  így a program először a többi csempét kezdi el lerakni és ha
 *   úgy nem tudja egymásra rakás nélkül megoldani a program utána kezdi a sarkokat egymásra rakni,
 * A legelső elem mindig a PostIt, a program azt próbálja először lerakni.
 * Az algoritmusokban igyekezzük megtartani az immutabilitást, azaz a Tile osztály példányai nem módosítjuk. A felhasznláló felületen módosítjuk csak.
 */

class Tile {
private:
    /**
     * A mezőn lévő csatlakozási pontok bitmezője (0-15).
     *
     * A CSP_TOP, CSP_RIGHT, CSP_BOTTOM, CSP_LEFT konstansokat használja (megadja, hogy melyik irányba van csatlakozási pont)
     */
    int connections;
    /**
     * A mező típusa, a TileType enum-ból.
     */
    TileType type;
    /**
     * A mező színe, a TileColor enum-ból.
     */
    TileColor color;
public:
    Tile(int connections, TileType type);

    Tile(int connections, TileType type, TileColor color);

    Tile();

    TileColor getColor() const;

    void setColor(TileColor color);

    int getConnections() const;

    void setConnections(int connections);

    TileType getType() const;

    void setType(TileType type);

    bool operator==(const Tile &rhs) const;

    bool operator!=(const Tile &rhs) const;

    bool operator<(const Tile &rhs) const;

    bool operator>(const Tile &rhs) const;

    bool operator<=(const Tile &rhs) const;

    bool operator>=(const Tile &rhs) const;
    /**
     * Megadja, hogy egy Tile sarok-e, később a két sarok egymásra rakásához kell.
     */

    bool isCorner() const;

    /**
     * Megadja, hogy egy Tile PostIt-e, azaz üres-e.
     * Kiemelt szerepű, azokra a mezőkre rakjuk rá, amelyekre másik mezőt nem lehet rakni, később pedig feltöltjük velük az
      üresen hagyott mezőket.
     * Extremális értékkel bír, mert magában a készletben nincsen, például akkor kapjuk, ha a készletben nincs több elem.
     */

    bool isPostIt() const;

    /**
     * A forgatásért felelős, az óramutató járásával megegyező irányban forgatja a csempét.
     * A forgatás a connections mezőt módosítja.
     * A bitmezők miatt egy kettvel való szorzással helyettesíthető, a programban minden bitet
      eltolunk egy helyet jobbra és az első bitet a legutolsó helyre tesszük. (|=-vel az utcsó bit)
     */

    Tile rotate(Rotation rotation) const;

    /**
     * A fromString függvény egy QStringből készít egy Tile-t.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * * Később a fájlból való kiírásnál használjuk, illetve a tesztekben.
     */

    static Tile fromString(const QString &string);

    /**
     * A getMaxPossibleRotation függvény megadja, hogy hány különböző forgatása van a csempének.
     * A keresztnek (15) 1 db, az egyenesnek (5 vagy 10)  pedig 2 db, a PostItnek nincs, a többi csempének pedig 4 db.
     * Ez azért fontos, mert ezzel lehet rövidíteni a keresést.
     */
    Rotation getMaxPossibleRotation() const;

    /**
     * A hasConnectionInDirection függvény megadja, hogy az adott irányban van-e csatlakozási pont.
     * Később a csőrendszer felépítésénél használjuk, hogy megállapítsuk, hogy a két csempét az adott iráynban
      össze lehet e kötni (CSP Mask), ezzel rövidítve a keresést.
     */
    bool hasConnectionInDirection(int direction) const;

    /**
     * A hasConnentionInStep függvény megadja, hogy az adott irányban van-e csatlakozási pont.
     * Később a csőrendszer felépítésénél használjuk, hogy megállapítsuk, hogy a két csempét az adott iráynban
      össze lehet e kötni, de ez felsorolással, ezzel rövidítve a keresést.
    * Azért kell mindkettő, mert néha csak egy irányunk van, néha pedig egy "MASK"- unk
     */

    bool hasConnentionInStep(GridPositionStep step) const;

    /**
     * A toQString függvény egy QString-gé alakítja a csempét.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Később a fájlba való kiírásnál használjuk, illetve a tesztekben.
     */

    QString toQString() const;

    /**
     * A typeAndColorToChar függvény egy QString-gé alakítja a csempének a típusát és a színét .
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Később a fájlba való kiírásnál használjuk, illetve a tesztekben.
     */

    QString typeAndColorToChar(bool hasRightItem) const;
};


/**
 * A PostIt egy konstans Tile, ami a játéktábla üres mezőjét reprezentálja.
 */

const Tile PostIt(0, NORMAL);


#endif //BEADANDO_II_TILE_H
