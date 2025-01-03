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
 * A stack megadja, hgy van-e egymáson két Tile, a horizontal az adott Tile vízszintes pozíciója,
 a vertical az adott Tile függőleges pozíciója.
 *
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

    GridPosition step(const GridPositionStep step) const;

    bool isLast() const;

    QString toQString() const;

    bool covers(const GridPosition &position) const;
};

const GridPosition INVALID_POSITION = GridPosition(Grid(0, 0), -1, -1, -1);

size_t qHash(GridPosition key, size_t seed = 0);

#endif //BEADANDO_II_GRIDPOSITION_H

