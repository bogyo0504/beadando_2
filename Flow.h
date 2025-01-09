//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_FLOW_H
#define BEADANDO_II_FLOW_H
#include "QMap"
#include "GridPosition.h"
#include "PipeLine.h"
#include "Phase.h"
/**
 * A fázis konkrét szétfolyása
 */
class Flow {
    /**
     * Benne van, hogy az adott fázisban az adott pozícióban melyik szín van.
     */
    QMap<GridPosition,TileColor> positions;
    Grid grid = Grid(0,0);

    /**
     * A makeFlow az adott szabályok szerint megpróbálja a fázist végrehajtani, tehát a szín nem folyik ki a csővezetékből,
     * nem folyik át más színbe és a szelepek is megfelelően vannak beállítva.
     */
    int makeFlow(const PipeLine &pipeLine, const Phase &phase, int valveConfig);
public:
    /**
     * A makeFlow függvény végrehajtja a folyás szabályait az összes szelep beállítással.
     * Ha a folyás helyes, akkor a függvény visszatér a folyás hosszával, egyébként -1-el tér vissza.
     */
    int makeFlow(const PipeLine &pipeLine, const Phase &phase);

    /**
     * A toQString függvény egy QString-gé alakítja a folyást.
     * A debugolásnál használjuk.
     */
    QString toQString() const;

    /**
     *Meg tudja adni, egy adott fázisban, egy adott pozícióban folyik-e szín és ha folyik akkor melyik szín.
     * Ezt a felhasználói felületen használjuk, mikor kirajzoljuk a folyást.
    */
    TileColor getTileColorAt(GridPosition &position) const;
};


#endif //BEADANDO_II_FLOW_H
