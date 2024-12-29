//
// Created by gyberni15 on 12/29/24.
//

#include <QQueue>
#include "Flow.h"

bool Flow::makeFlow(const PipeLine &pipeLine, const Phase &phase) {
    for (const TileColor color : phase.getActiveColors()) {
        QList<GridPosition> sources = pipeLine.getSourcePositions(color);
        QQueue<GridPosition> flow;
        for (const GridPosition &sourcePosition : sources) {
            if (positions.contains(sourcePosition) && positions[sourcePosition] != NONE) {
                if (positions[sourcePosition] != color) {
                    return false;
                }
                continue;
            }
            flow.enqueue(sourcePosition);
            positions[sourcePosition] = color;
        }
        while (!flow.isEmpty()) {
            GridPosition current = flow.dequeue();
            for (GridPositionStep step : {UP, DOWN, LEFT, RIGHT}) {
                if (pipeLine.checkConnection(step, current)) {
                    const GridPosition &newPosition = current.step(step);
                    if (positions.contains(newPosition) && positions[newPosition] != NONE) {
                        if (positions[newPosition] == color) {
                            continue;
                        }
                        if (pipeLine[newPosition].getColor() != color) {
                            return false;
                        }
                    }
                    positions[newPosition] = color;
                    flow.enqueue(newPosition);
                }
            }
        }
        const QList<GridPosition> &sinks = pipeLine.getSinkPositions(color);
        for (const GridPosition &sinkPosition : sinks) {
            if (!positions.contains(sinkPosition) || positions[sinkPosition] != color) {
                return false;
            }
        }
    }
    return true;
}
