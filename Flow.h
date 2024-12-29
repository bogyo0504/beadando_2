//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_FLOW_H
#define BEADANDO_II_FLOW_H
#include "QMap"
#include "GridPosition.h"
#include "PipeLine.h"
#include "Phase.h"

class Flow {
    QMap<GridPosition,TileColor> positions;
    Grid grid = Grid(0,0);
public:
    bool makeFlow(const PipeLine &pipeLine, const Phase &phase, int valveConfig);
    bool makeFlow(const PipeLine &pipeLine, const Phase &phase);
    QString toQString() const;

};


#endif //BEADANDO_II_FLOW_H
