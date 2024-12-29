//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_BUILDSTATE_H
#define BEADANDO_II_BUILDSTATE_H

#include "Stock.h"
#include "GridPosition.h"


//Adott pozícióban az adott stockból kell rakni, illetve jelöljük, ha készen van a puzzle

class BuildState {
private:
    GridPosition position;
    Stock stock;
    bool isReady;
    Tile currentTile;
    Rotation rotation;
public:
    BuildState(const GridPosition &position, const Stock &stock, const bool isReady, const Tile currentTile, Rotation rotation);

    Rotation getRotation() const;

    void setRotation(Rotation rotation);

    BuildState(const BuildState &buildState);

    const GridPosition &getPosition() const;

    const Stock &getStock() const;

    const bool getIsReady() const;

    const Tile &getCurrentTile() const;



};


#endif //BEADANDO_II_BUILDSTATE_H
