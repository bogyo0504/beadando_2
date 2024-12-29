//
// Created by gyberni15 on 10/30/24.
//

#include "Stock.h"


Stock::Stock(const QMap<Tile, int> &stock) : stock(stock) {}


Stock Stock::operator-(const Tile &tile) const {
    if (tile == PostIt) {
        return *this;
    }
    if (!stock.contains(tile)) {
        return *this;
    }
    QMap<Tile, int> resultStock = stock;
    resultStock[tile] = resultStock[tile] - 1;
    return Stock(resultStock);
}

Stock Stock::addTile(const Tile &tile, int howMany) const {
    if (tile == PostIt) {
        return *this;
    }
    QMap<Tile, int> resultStock = stock;
    resultStock[tile] = resultStock[tile] + howMany;
    return Stock(resultStock);
}

Tile Stock::getNextTile(const Tile &tile) const {

    auto it = stock.lowerBound(tile);
    if (it.key() == tile) {
        it++;
    }
    if (it == stock.end()) {
        return PostIt;
    }
    return it.key();
}

bool Stock::contains(const Tile &tile) const {
    if (tile == PostIt) {
        return true;
    }
    if (!stock.contains(tile)) {
        return false;
    }
    return stock[tile] > 0;
}

Stock::Stock() : stock(){}
