//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_STOCK_H
#define BEADANDO_II_STOCK_H


#include <QMap>
#include "Tile.h"

/**
 * A Stock osztály reprezentálja a lefutás során használható elemeket.
 * A Stock osztály immutábilis, minden módosítás új példányt hoz létre, amely tartalmazza az adott módosítást.
 * Ez azért jó, mert a visszalépés esetén mindig tudjuk az előző állapotot.
 */


class Stock {
private:
    QMap<Tile, int> stock;
public:
    Stock();

    explicit Stock(const QMap<Tile, int> &stock);

    /**
     * Hozzáadja az adott elemet adott számmal a készlethez.
     */

    Stock addTile(const Tile &tile, int howMany) const;
    /**
     * Kiveszi az adott elemet a készletből.
     */
    Stock operator-(const Tile &tile) const;

    /**
     * Megadja a készlet következő elemét,mivel a Tileban van rendezettség, emiatt a következő egyértelmű.
     * A lowerbound függvényt használja, megkeresi az következő elemet, ami a paraméterként kapott elemnél nagyobb vagy egyenlő.
     * PostIt-et ad vissza, ha a tile az utolsó elem.
     * @param tile - az aktuális elem
     * @return a következő elem vagy PostIt
     */
    Tile getNextTile(const Tile &tile) const;

    /**
     * Megadja, hogy az adott elem megtalálható-e a készletben.
     * @param tile - az elem
     * @return true, ha megtalálható, false egyébként
     */
    bool contains(const Tile &tile) const;

    /**
     * A toQString függvény egy QString-gé alakítja a készletet.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Minden sor egy elem, elsőként az elemszám, majd a csempe szóközzel elválasztva.
     * Később a fájlba való kiírásnál használjuk, illetve a tesztekben.
     */

    static Stock fromString(const QString &string);

    /**
     * Visszaadja a készletet.
     * Nem adjuk ki a stock belső szerkezetét, hanem egy másolatot adunk vissza, ezzel megakadályozva a készlet bármiféle módosítását.
     */
    QMap<Tile, int> enumStock() const;

    /**
     * A toQString függvény egy QString-gé alakítja a készletet.
     * A "signs.md" fájlban találhatóak a karakterek és a leírása.
     * Minden sor egy elem, elsőként az elemszám, majd a csempe szóközzel elválasztva.
     * Később a fájlba való kiírásnál használjuk, illetve a tesztekben.
     */

    QString toQString();

};


#endif //BEADANDO_II_STOCK_H
