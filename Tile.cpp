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



