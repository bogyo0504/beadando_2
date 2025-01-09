//
// Created by gyberni15 on 12/29/24.
//

#include <QQueue>
#include <QList>
#include "Flow.h"

//A valveConfig egy bitMask, minden bit egy szelep és megmondja, hogy az adott szelep zárva van-e (az 1 azt jelenti zárva van)
int Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase, int valveConfig) {
    //Ez a csapok pozícióit tartalmazza
    QList<GridPosition> valves = pipeLine.getValvePositions();
    QMap<GridPosition, bool> closedValves;
    grid = pipeLine.getGrid();
    for (const GridPosition &valve: valves) {
        if ((valveConfig & 1) != 0) {
            closedValves[valve] = true;
        }
        valveConfig >>= 1;
    }
    //Végigmegyünk az aktív színeken
    for (const TileColor color: phase.getActiveColors()) {
        //A források pozícióit tartalmazó lista
        QList<GridPosition> sources = pipeLine.getSourcePositions(color);
        //A folyadék haladását reprezentáló QQueue, a típus azért jó, mert mindig csak a sor elejéről vesszük ki az elemeket
        // és a sor végére tesszük az új elemeket
        QQueue<GridPosition> flow;
        //Végigmegyünk a források pozícióin
        //Elindítjuk a forrásokból a folyadékot
        for (const GridPosition &sourcePosition: sources) {
            //Ha a pozíción folyik valami,
            if (positions.contains(sourcePosition) && positions[sourcePosition] != NONE) {
                //Ha nem az a szín folyik, amilyen színű a forrás akkor nem jó
                if (positions[sourcePosition] != color) {
                    return -1;
                }
                continue;
            }
            //A zárt csapon nem folyhat át a folyadék, ez valójában nem történhet meg, mert
            //nem lehet valami egyszerre forrás és zárt szelep
            if (closedValves.contains(sourcePosition)) {
                continue;
            }
            //Betesszük a forrást a sorba és beletesszük a folyamba
            flow.enqueue(sourcePosition);
            positions[sourcePosition] = color;
        }
        //Idáig a source-okat raktuk be a folyamba és itt kezdjük el a folyadékot terjeszteni
        while (!flow.isEmpty()){
            //Egyesével kivesszük a folyadékot a sor elejéről, ahol éppen az adott szín folyhat
            GridPosition current = flow.dequeue();
            //Megnézzük mind a 4 irányba, hogy merre tud tovább folyni
            for (GridPositionStep step: {UP, DOWN, LEFT, RIGHT}) {
                bool hasNoConnection = true;
                //Itt nézzük meg, hogy van-e kapcsolat a két csempe között az adott irányba
                if (pipeLine.checkConnection(current, step, false)) {
                    hasNoConnection = false;
                    //Ha van kapcsolat, akkor ellépünk abba az irányba
                    const GridPosition &newPosition = current.step(step);
                    //Ha már folyik valami a pozíción, akkor megnézzük, hogy az a szín-e, amit éppen folyatunk
                    if (positions.contains(newPosition) && positions[newPosition] != NONE) {
                        //Ha az a szín, amit éppen folyatunk, akkor nem történik semmi
                        if (positions[newPosition] == color) {
                            continue;
                        }
                        //Ha nem az a szín, amit éppen folyatunk, akkor nem jó a folyam
                        if (pipeLine[newPosition].getColor() != color) {
                            return -1;
                        }
                    }
                    //Ha a pozíció zárt szelep, akkor nem folyhat át rajta a folyadék
                    if (closedValves.contains(newPosition)) {
                        continue;
                    }
                    //Ha üres volt a pozíció és folyhat arra a szín akkor berakjuk a folyamba és beállítjuk a színét
                    positions[newPosition] = color;
                    flow.enqueue(newPosition);
                }
                //Ugyanazt mint előbb megnézi a másik szinten is
                if (pipeLine.checkConnection(current, step, true)) {
                    hasNoConnection = false;
                    const GridPosition &newPosition = current.step(step).step(OTHER_STACK);
                    if (positions.contains(newPosition) && positions[newPosition] != NONE) {
                        if (positions[newPosition] == color) {
                            continue;
                        }
                        if (pipeLine[newPosition].getColor() != color) {
                            return -1;
                        }
                    }
                    if (closedValves.contains(newPosition)) {
                        continue;
                    }
                    positions[newPosition] = color;
                    flow.enqueue(newPosition);
                }
                if (hasNoConnection) {
                    if (pipeLine[current].hasConnentionInStep(step)) {
                        return -1;
                    }
                }
            }
        }
        //Végigmegyünk a lehetséges lefolyási irányokon és megnézzük, hogy minden szín lefolyik-e
        const QList<GridPosition> &sinks = pipeLine.getSinkPositions(color);
        for (const GridPosition &sinkPosition: sinks) {
            //Ha fogyasztó nem szerepel a folyamban, akkor nem jó
            if (!positions.contains(sinkPosition) || positions[sinkPosition] != color) {
                return -1;
            }
        }
    }
    //A függvény -1-el tér vissza, ha nem jó a folyam, egyébként a folyam hosszával
    //annyi helyen van flow, ahány position-ön végigfolyik a folyadék, ez későb az optilmális megoldás megtalálásához kell
    return positions.size();
}
//ezt a feladatot annyira bonyolult, hogy meglátta a halál és inkább megölte magát.


//Ez a függvény fogja meghívni a makeFlow függvényt minden lehetséges szelep beállítással,
//később ez a függvényt használja a PipeLineValidator
int Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase) {
    QList<GridPosition> valves = pipeLine.getValvePositions();
    grid = pipeLine.getGrid();
    for (int i = 0; i < (1 << valves.size()); i++) {
        positions.clear();
        int flowSize = makeFlow(pipeLine, phase, i);
        if (flowSize >= 0) {
            return flowSize;
        }
    }
    return -1;
}

QString Flow::toQString() const {
    QString result;
    for (int i = 0; i < grid.getHeight(); ++i) {
        for (int j = 0; j < grid.getWidth(); ++j) {
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            GridPosition currentPositionOther = GridPosition(grid, 1, j, i);

            if (positions.contains(currentPosition) || positions.contains(currentPositionOther)) {
                TileColor color = positions.contains(currentPosition) ? positions[currentPosition]
                                                                      : positions[currentPositionOther];
                switch (color) {
                    case NONE:
                        result += "     ";
                        break;
                    case RED:
                        result += "RRRRR";
                        break;
                    case CIAN:
                        result += "CCCCC";
                        break;
                    case BLUE:
                        result += "BBBBB";
                        break;
                    case GREEN:
                        result += "GGGGG";
                        break;
                    case YELLOW:
                        result += "YYYYY";
                        break;
                }
            } else {
                result += "     ";
            }

        }
        result += "\n";
    }
    return result;
}

TileColor Flow::getTileColorAt(GridPosition &position) const {
    if (positions.contains(position)) {
        return positions[position];
    }
    return NONE;
}

