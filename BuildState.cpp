//
// Created by gyberni15 on 10/30/24.
//

#include "BuildState.h"


BuildState::BuildState(const GridPosition &position, const Stock &stock, const bool isReady, const Tile currentTile,
                       Rotation rotation)
        : position(position), stock(stock), isReady(isReady), currentTile(currentTile), rotation(rotation) {}


const GridPosition &BuildState::getPosition() const {
    return position;
}

const Stock &BuildState::getStock() const {
    return stock;
}

const bool BuildState::getIsReady() const {
    return isReady;
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
                                                       isReady(buildState.getIsReady()),
                                                       currentTile(buildState.getCurrentTile()),
                                                       rotation(buildState.getRotation()){}



