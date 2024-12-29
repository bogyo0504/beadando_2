//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PHASE_H
#define BEADANDO_II_PHASE_H
#include "Tile.h"
#include <QSet>

class Phase {
    QSet<TileColor> activeColors;
public:
    QSet<TileColor> getActiveColors() const;
};


#endif //BEADANDO_II_PHASE_H
