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

class PipeLine {
private:
    const Grid grid;
    QHash<GridPosition, Tile> tiles;
    QStack<std::shared_ptr<BuildState>> states;
    bool overlappingEnabled = true;
public:

    const Grid &getGrid() const;

    PipeLine &put(const GridPosition position, const Tile &tile);

    explicit PipeLine(const Grid &grid);

    /**
     * Megvizsgálja, hogy az adott pozícióra lehet-e rakni a megadott Tile-t.

     */
    bool canPut(const GridPosition position, const Tile &tile) const;

    Tile operator[](const GridPosition &position) const;

    bool checkConnection(const GridPosition &position, const GridPositionStep step, bool otherLevel) const;

    BuildState addElementFromStock(const BuildState &state);

    bool isEmpty(const GridPosition &position) const;

    bool clear(const GridPosition &position);

    QList<GridPosition> getSourcePositions(TileColor color) const;

    QList<GridPosition> getSinkPositions(TileColor color) const;

    QList<GridPosition> getValvePositions() const;


    QString toQString(bool prefix = false) const;

    static PipeLine fromString(const QString &string);


    QPair<bool, BuildState> stepBack();

    void disableOverlap();


    PipeLine resizeGrid(int width, int height) const;

    void resetBuildStates();

    void removePostIts();

    long getAlternativeCount();

private:

    bool isEmptyAndAvailable(GridPosition position);

    Rotation findRotation(GridPosition position, Tile tile) const;

    bool hasValidNeighbourghs(const GridPosition &position, const Tile &tile) const;

    bool
    hasValidNeighbourgh(const GridPosition &position, const Tile &tile, GridPositionStep step, int selfConnectionMask,
                        int otherConnectionMask) const;
};


#endif //BEADANDO_II_PIPELINE_H
