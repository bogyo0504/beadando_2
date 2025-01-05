//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_BUILDSTATE_H
#define BEADANDO_II_BUILDSTATE_H

#include "Stock.h"
#include "GridPosition.h"

enum BuildStatus {
    ERROR, READY, IN_PROGRESS, TRY_NEXT, OUT_OF_STOCK
};

//Adott pozícióban az adott stockból kell rakni, illetve jelöljük, ha készen van a puzzle

class BuildState {
private:
    GridPosition position;
    Stock stock;
    BuildStatus status;
    Tile currentTile;
    Rotation rotation;
 public:
    BuildState(const GridPosition &position, const Stock &stock, BuildStatus status, const Tile currentTile,
               Rotation rotation);

    Rotation getRotation() const;

    void setRotation(Rotation rotation);

    BuildState(const BuildState &buildState);

    const GridPosition &getPosition() const;

    const Stock &getStock() const;

    BuildStatus getStatus() const;

    const Tile &getCurrentTile() const;



};


#endif //BEADANDO_II_BUILDSTATE_H
