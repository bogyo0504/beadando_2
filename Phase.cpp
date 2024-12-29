//
// Created by gyberni15 on 12/29/24.
//

#include "Phase.h"

QSet<TileColor> Phase::getActiveColors() const {
    return activeColors;
}

Phase::Phase(const QSet<TileColor> &activeColors) : activeColors(activeColors) {
}

Phase::Phase() {}

Phase::Phase(TileColor color): activeColors({color}) {}
