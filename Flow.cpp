//
// Created by gyberni15 on 12/29/24.
//

#include <QQueue>
#include <QList>
#include "Flow.h"

int Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase, int valveConfig) {
    QList<GridPosition> valves = pipeLine.getValvePositions();
    QMap<GridPosition, bool> closedValves;
    grid = pipeLine.getGrid();
    for (const GridPosition &valve: valves) {
        if ((valveConfig & 1) != 0) {
            closedValves[valve] = true;
        }
        valveConfig >>= 1;
    }
    for (const TileColor color: phase.getActiveColors()) {
        QList<GridPosition> sources = pipeLine.getSourcePositions(color);
        QQueue<GridPosition> flow;
        for (const GridPosition &sourcePosition: sources) {
            if (positions.contains(sourcePosition) && positions[sourcePosition] != NONE) {
                if (positions[sourcePosition] != color) {
                    return -1;
                }
                continue;
            }
            if (closedValves.contains(sourcePosition)) {
                continue;
            }
            flow.enqueue(sourcePosition);
            positions[sourcePosition] = color;
        }
        while (!flow.isEmpty()) {
            GridPosition current = flow.dequeue();
            for (GridPositionStep step: {UP, DOWN, LEFT, RIGHT}) {
                bool hasNoConnection = true;
                if (pipeLine.checkConnection(current, step, false)) {
                    hasNoConnection = false;
                    const GridPosition &newPosition = current.step(step);
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
        const QList<GridPosition> &sinks = pipeLine.getSinkPositions(color);
        for (const GridPosition &sinkPosition: sinks) {
            if (!positions.contains(sinkPosition) || positions[sinkPosition] != color) {
                return -1;
            }
        }
    }
    return positions.size(); //annyi helyen van flow, ahány position-ön végigfolyik a folyadék
}
//ezt a feladatot annyira bonyolult, hogy meglátta a halál és inkább megölte magát.


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

