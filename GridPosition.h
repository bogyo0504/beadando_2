//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_GRIDPOSITION_H
#define BEADANDO_II_GRIDPOSITION_H

#include <QString>
#include "Grid.h"
#include "QHash"

enum GridPositionStep {
    OTHER_STACK, UP, DOWN, LEFT, RIGHT
};

/**
 * A GridPosition megadja egy adott Tile pozícióját a Griden belül.
 * Ez egy háromdimenziós tér, a vízszintes, a függőleges és a stack dimenziókból áll.
 * A stack megadja, hgy van-e egymáson két Tile (vagy PostIt), a horizontal az adott Tile vízszintes pozíciója,
 a vertical az adott Tile függőleges pozíciója.
 * A GridPosition osztály rendezett, a rendezés a stack, majd a horizontal, majd a vertical szerint történik.
 */
class GridPosition {
private:
    Grid grid;
    int stack;
    int horizontal;
    int vertical;
public:

    GridPosition(const Grid &grid, const int stack, const int horizontal, const int vertical);

    const int getStack() const;

    const int getHorizontal() const;

    const int getVertical() const;

    bool operator==(const GridPosition &rhs) const;

    bool operator!=(const GridPosition &rhs) const;

    bool operator<(const GridPosition &rhs) const;

    bool operator>(const GridPosition &rhs) const;

    bool operator<=(const GridPosition &rhs) const;

    bool operator>=(const GridPosition &rhs) const;

    GridPosition operator++() const;

    GridPosition operator--() const;

    /**
     * A step függvény egy adott irányba lépteti a GridPosition-t, majd visszaadja a lépés utáni pozíciót.
     */

    GridPosition step(const GridPositionStep step) const;

    /**
     * A isLast függvény megadja, hogy az adott pozíció az utolsó-e a Griden belül, tehát a második szinten (stacken),
       és a griden belül a legutolsó pozíció-e.
     */

    bool isLast() const;

    /**
     * A toQString függvény egy QString-gé alakítja a GridPosition-t, először a  stackat,
     a vízszintes helyzetet, majd a függőleges helyzetet írja le.
     * Később a tesztekben használjuk.
     */

    QString toQString() const;

    /**
     * Ha ez a gridposition megegyezik a paraméterként kapott gridpositionnel vagy
     ha a mintában a stack -1 akkor a stacket nem veszi figyelembe, illetve ha a horizontal -1, akkor azt sem veszi figyelmbe
     és igazat ad vissza
     * A debugolásnál használjuk, hogy megnézzük, hogy egy adott pozíció a mintában van-e.
     */
    bool covers(const GridPosition &position) const;
};
//Ez az extremális érték, amit akkor adunk vissza, ha egy pozíció nem létezik.
const GridPosition INVALID_POSITION = GridPosition(Grid(0, 0), -1, -1, -1);

size_t qHash(GridPosition key, size_t seed = 0);

#endif //BEADANDO_II_GRIDPOSITION_H

