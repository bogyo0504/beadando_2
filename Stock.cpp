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
    if (resultStock[tile] <= 0) {
        return *this;
    }
    int tileCount = resultStock[tile] - 1;
    if (tileCount > 0) {
        resultStock[tile] = resultStock[tile] - 1;
    } else {
        resultStock.remove(tile);
    }
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

Stock::Stock() : stock() {}


/*
 * Stock definition is:
 * -every line is a stock element
 * -line contains a number and a tile with a space separator
 */
Stock Stock::fromString(const QString &string) {
    QMap<Tile, int> stock;
    QStringList lines = string.split("\n");
    for (const QString &line: lines) {
        if (line == "") {
            continue;
        }
        QStringList parts = line.split(" ");
        if (parts.size() < 2) {
            throw std::invalid_argument("Invalid string: " + string.toStdString());
        }
        bool ok;
        int number = parts[0].toInt(&ok);
        if (!ok) {
            throw std::invalid_argument("Invalid string: " + string.toStdString());
        }
        QString tileString = "";
        for (int i = 1; i < parts.size(); i++) {
            tileString += parts[i];
            if (i != parts.size() - 1) {
                tileString += " ";
            }
        }
        if (tileString.size() % 5 != 0) {
           tileString += QString(5 - tileString.size() % 5, ' ');
        }
        Tile tile = Tile::fromString(tileString);
        stock[tile] = number;
    }
    return Stock(stock);

}

QMap<Tile, int> Stock::enumStock() const {
    return stock;
}

QString Stock::toQString() {
    QString result;
    for (auto it = stock.begin(); it != stock.end(); ++it) {
        Tile key = it.key();
        int value = it.value();
        result += QString::number(value) + " " + key.toQString() + "\n";
    }
    return result;
}
