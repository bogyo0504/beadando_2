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

class PipeLine {
private:
    const Grid grid;
     QMap<GridPosition, Tile> tiles;
     QStack<std::shared_ptr<BuildState>> states;
public:


    PipeLine& put(const GridPosition position, const Tile& tile);

    explicit PipeLine(const Grid &grid);

    /**
     * Megvizsgálja, hogy az adott pozícióra lehet-e rakni a megadott Tile-t.

     */
    bool canPut(const GridPosition position, const Tile& tile) const;

    Tile operator[](const GridPosition &position) const;

    bool checkConnection(GridPositionStep step, const GridPosition position) const;

    BuildState addElementFromStock(const BuildState& state) ;

    bool isEmpty(const GridPosition &position) const;

    bool clear(const GridPosition &position);
};


#endif //BEADANDO_II_PIPELINE_H
