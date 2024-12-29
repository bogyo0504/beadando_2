//
// Created by gyberni15 on 10/30/24.
//

#include "Tile.h"


Tile::Tile(int connections, TileType type) : connections(connections), type(type) {}

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

bool Tile::operator==(const Tile &rhs) const {
    return connections == rhs.connections &&
           type == rhs.type;
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

Tile::Tile() : Tile(0, NORMAL){}
