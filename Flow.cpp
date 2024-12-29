//
// Created by gyberni15 on 12/29/24.
//

#include <QQueue>
#include <QList>
#include "Flow.h"

bool Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase, int valveConfig) {
    QList<GridPosition> valves = pipeLine.getValvePositions();
    QMap<GridPosition, bool> closedValves;
    grid = pipeLine.getGrid();
    for (const GridPosition &valve: valves) {
        if ((valveConfig & 1) == 0) {
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
                    return false;
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
                if (pipeLine.checkConnection(current, step, false)) {
                    const GridPosition &newPosition = current.step(step);
                    if (positions.contains(newPosition) && positions[newPosition] != NONE) {
                        if (positions[newPosition] == color) {
                            continue;
                        }
                        if (pipeLine[newPosition].getColor() != color) {
                            return false;
                        }
                    }
                    if (closedValves.contains(newPosition)) {
                        continue;
                    }
                    positions[newPosition] = color;
                    flow.enqueue(newPosition);
                }
                if (pipeLine.checkConnection(current, step, true)) {
                    const GridPosition &newPosition = current.step(step).step(OTHER_STACK);
                    if (positions.contains(newPosition) && positions[newPosition] != NONE) {
                        if (positions[newPosition] == color) {
                            continue;
                        }
                        if (pipeLine[newPosition].getColor() != color) {
                            return false;
                        }
                    }
                    if (closedValves.contains(newPosition)) {
                        continue;
                    }
                    positions[newPosition] = color;
                    flow.enqueue(newPosition);
                }
            }
        }
        const QList<GridPosition> &sinks = pipeLine.getSinkPositions(color);
        for (const GridPosition &sinkPosition: sinks) {
            if (!positions.contains(sinkPosition) || positions[sinkPosition] != color) {
                return false;
            }
        }
    }
    return true;
}


bool Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase) {
    QList<GridPosition> valves = pipeLine.getValvePositions();
    grid = pipeLine.getGrid();
    for (int i = 0; i < (1 << valves.size()); i++) {
        positions.clear();
        // TODO: Itt lehet, hogy meg kéne jegyezni, hogy melyik szelep volt nyitva....
        if (makeFlow(pipeLine, phase, i)) {
            return true;
        }
    }
    return false;
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

