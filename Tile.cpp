//
// Created by gyberni15 on 10/30/24.
//

#include "Tile.h"


Tile::Tile(int connections, TileType type, TileColor color) : connections(connections), type(type), color(color) {}

Tile::Tile() : Tile(0, NORMAL, NONE) {}

Tile::Tile(int connections, TileType type) : Tile(connections, type, NONE) {}


int Tile::getConnections() const {
    return connections;
}

void Tile::setConnections(int connections) {
    Tile::connections = connections;
}

TileType Tile::getType() const {
    return type;
}

void Tile::setType(TileType type) {
    Tile::type = type;
}

TileColor Tile::getColor() const {
    return color;
}

void Tile::setColor(TileColor color) {
    Tile::color = color;
}


bool Tile::operator==(const Tile &rhs) const {
    return connections == rhs.connections &&
           type == rhs.type && color == rhs.color;
}

bool Tile::operator!=(const Tile &rhs) const {
    return !(rhs == *this);
}

bool Tile::operator<(const Tile &rhs) const {
    if (isCorner() && !rhs.isCorner()) {
        return false;
    }
    if (!isCorner() && rhs.isCorner()) {
        return true;
    }
    if (connections < rhs.connections)
        return true;
    if (rhs.connections < connections)
        return false;
    return type < rhs.type;
}

bool Tile::operator>(const Tile &rhs) const {
    return rhs < *this;
}

bool Tile::operator<=(const Tile &rhs) const {
    return !(rhs < *this);
}

bool Tile::operator>=(const Tile &rhs) const {
    return !(*this < rhs);
}

bool Tile::isCorner() const {
    return connections == 3 || connections == 6 || connections == 9 || connections == 12;
}

bool Tile::isPostIt() const {
    return connections == 0;
}

Tile Tile::rotate(Rotation rotation) const {
    int newConnections = connections;
    for (int i = 0; i < rotation; ++i) {
        int wasLeft = newConnections & 1;
        newConnections >>= 1;
        newConnections |= wasLeft << 3;
    }
    return Tile(newConnections, type, color);
}

Tile Tile::fromString(const QString &string) {
    TileType type = NORMAL;
    TileColor color = NONE;
    if (string.size() < 5) {
        throw std::invalid_argument("Invalid string: " + string.toStdString());
    }
    int connections = 0;
    if (string[0] == '-') {
        connections |= CSP_LEFT;
    }
    if (string[1] == '|') {
        connections |= CSP_TOP;
        connections |= CSP_BOTTOM;
    }
    if (string[1] == '.') {
        connections |= CSP_BOTTOM;
    }
    if (string[1] == '\'') {
        connections |= CSP_TOP;
    }
    if (string[2] == '-') {
        connections |= CSP_RIGHT;
    }
    if (string[3] == 'o') {
        type = SOURCE;
    }
    if (string[3] == 'c') {
        type = SINK;
    }
    if (string[3] == '0') {
        type = VALVE;
    }
    if (string[4] == 'R') {
        color = RED;
    }
    if (string[4] == 'C') {
        color = CIAN;
    }
    if (string[4] == 'B') {
        color = BLUE;
    }
    if (string[4] == 'G') {
        color = GREEN;
    }
    if (string[4] == 'Y') {
        color = YELLOW;
    }
    Tile tile = Tile(connections, type, color);
    return tile;
}

Rotation Tile::getMaxPossibleRotation() const {
    if(connections == 5 || connections == 10){
        return 2;
    }
    if(connections == 0){
        return 1;
    }
    return 4; // a forgatások száma a használatánál levonunk blőle 1-et, mert 0-tól indexelünk :)
}

bool Tile::hasConnectionInDirection(int direction) const {
    return (connections & direction) != 0;
}

bool Tile::hasConnentionInStep(GridPositionStep step) const {
    switch (step) {
        case UP:
            return hasConnectionInDirection(CSP_TOP);
        case DOWN:
            return hasConnectionInDirection(CSP_BOTTOM);
        case LEFT:
            return hasConnectionInDirection(CSP_LEFT);
        case RIGHT:
            return hasConnectionInDirection(CSP_RIGHT);
        default:
            return false;

    }
}



