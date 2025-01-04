//
// Created by gyberni15 on 10/30/24.
//

#include "BuildState.h"


BuildState::BuildState(const GridPosition &position, const Stock &stock, BuildStatus status, const Tile currentTile,
                       Rotation rotation, long alternatives)
        : position(position), stock(stock), status(status), currentTile(currentTile), rotation(rotation),
          alternatives(alternatives) {}


const GridPosition &BuildState::getPosition() const {
    return position;
}

const Stock &BuildState::getStock() const {
    return stock;
}

const Tile &BuildState::getCurrentTile() const {
    return currentTile;
}

Rotation BuildState::getRotation() const {
    return rotation;
}

void BuildState::setRotation(Rotation rot) {
    BuildState::rotation = rot;
}

BuildState::BuildState(const BuildState &buildState) : position(buildState.getPosition()), stock(buildState.getStock()),
                                                       status(buildState.getStatus()),
                                                       currentTile(buildState.getCurrentTile()),
                                                       rotation(buildState.getRotation()),
                                                       alternatives(buildState.alternatives){}

 BuildStatus BuildState::getStatus() const {
    return status;
}

long BuildState::getAlternatives() const {
    return alternatives;
}



