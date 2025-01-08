//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_BUILDSTATE_H
#define BEADANDO_II_BUILDSTATE_H

#include "Stock.h"
#include "GridPosition.h"

/**
 * A BuildStatus enum a BuildState osztályban a különböző állapotokat reprezentálja.
 * ERROR - Hiba történt, READY - Kész a puzzle, IN_PROGRESS - Folyamatban van a puzzle, TRY_NEXT - Próbálja a következőt, OUT_OF_STOCK - Elfogyott a készlet
 */
enum BuildStatus {
    ERROR, READY, IN_PROGRESS, TRY_NEXT, OUT_OF_STOCK
};

/**
 * A BuildState osztály a pályaépítési fa éleit reprezentálja.
 * Ezt használjuk a visszalépéses algoritmusban, BuildState-nként haladunk és lépünk vissza.
 * Tudjuk az aktuális pozíciót, a készletet, amelyet még fel tudunk használni, a építés állapotát,
  az aktuális csempét, amelyet leraktunk és annak a  forgatási állapotát.
 */
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
