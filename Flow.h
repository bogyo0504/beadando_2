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
public:
    bool makeFlow(const PipeLine &pipeLine, const Phase &phase);
};


#endif //BEADANDO_II_FLOW_H
