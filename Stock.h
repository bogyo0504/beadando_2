//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_STOCK_H
#define BEADANDO_II_STOCK_H


#include <QMap>
#include "Tile.h"

/**
 * A Stock osztály reprezentálja a lefutás során használható elemeket.
 */


class Stock {
private:
    QMap<Tile, int> stock;
public:
    Stock();

    explicit Stock(const QMap<Tile, int> &stock);

    Stock addTile(const Tile &tile, int howMany) const;
    /**
     * Kivesz egy elemet a készletből.
     */
    Stock operator-(const Tile &tile) const;

    /**
     * Megadja a készlet következő elemét,mivel a Tileban van rendezettség, emiatt a következő egyértelmű.
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

    static Stock fromString(const QString &string);

    QMap<Tile, int> enumStock() const;

    QString toQString();

    long getCountOfAlternatives(const Tile &tile) const;
};


#endif //BEADANDO_II_STOCK_H
