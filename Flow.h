//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_FLOW_H
#define BEADANDO_II_FLOW_H
#include "QSet"
#include "GridPosition.h"
#include "PipeLine.h"

class Flow {
    QSet<GridPosition> positions;

public:

    void makeFlow(const PipeLine &pipeLine);
};


#endif //BEADANDO_II_FLOW_H
