//
// Created by gyberni15 on 10/30/24.
//
#include "PipeLine.h"

bool PipeLine::canPut(const GridPosition position, const Tile &tile) const {
    if (tile.isPostIt()) {
        return true;
    }
    const Tile other = (*this)[position.step(OTHER_STACK)];

    if (tile.isCorner()) {
        if (!other.isPostIt()) {
            if (!other.isCorner() || other == tile) {
                return false;
            }
            if (other.getConnections() !=
                (tile.getConnections() ^ 15)) { //^15 a XOR, tehát visszaadja a komplementert pl. 1100-nak 0011-et
                return false;
            }
            if (other.getType() != tile.getType()) {
                return false;
            }
            if (tile.getType() != NORMAL) {
                return false;
            }
            //Ez a két kanyar egymásra rakása
        }
    } else {
        if (!other.isPostIt()) {
            return false;
        }
        //Minden más csak PostIt-re rakható rá
    }
    return true;

    //A többi esetben a Tile-t rá lehet rakni a megadott pozícióra
}

Tile PipeLine::operator[](const GridPosition &position) const {
    if (tiles.contains(position)) {
        return tiles[position];
    } else {
        return PostIt;
    }
}

//Todo: Tesztelni a függvényt

bool PipeLine::checkConnection(const GridPosition &position, const GridPositionStep step, bool otherLevel) const {
    const Tile tile = (*this)[position];
    const Tile other = otherLevel ?  (*this)[position.step(step).step(OTHER_STACK)] : (*this)[position.step(step)];
    //const Tile other2 = (*this)[position.step(step).step(OTHER_STACK)];
    int connections = other.getConnections(); // | other2.getConnections();

    int following = connections;        //ha jobbra vagy lefele megyünk
    int previous = tile.getConnections();

    if (step == LEFT || step == UP) {   // ha balra vagy felfele megyünk
        following = tile.getConnections();
        previous = connections;
    }
    int first = CSP_RIGHT; //amit vizsgálunk
    int second = CSP_LEFT; //amivel vizsgáljuk

    if (step == UP || step == DOWN) {
        first = CSP_BOTTOM; //amit vizsgálunk
        second = CSP_TOP; //amivel vizsgáljuk
    }
    if ((previous & first) != 0 && (following & second) != 0) {
        return true;
    } else {
        return false;
    }
    /**
     * A checkConnection függvény ellenőrzi, hogy a két Tile között van-e kapcsolat.
     * A kapcsolat akkor van, ha az egyik Tile-ban van egy csatlakozási pont, ami a másik Tile-ban van.
     * A csatlakozási pontokat a CSP_TOP, CSP_RIGHT, CSP_BOTTOM, CSP_LEFT konstansokkal lehet ellenőrizni.
     * A függvény visszatérési értéke true, ha van kapcsolat, false, ha nincs.
     */
}


/*
 * TODO: Optimalizációk:
 * - Csak Normálra lehet rakni bármit is.
 */

//Megpróbálni a stockból a currenttilet kiszedni, ha a  nincs készlet a stockban az adott tileból, akkor
//meg kell keresni a következő stockból a következő tile-t és az így kapott tile-t be kell rakni a gridbe és vissza kell adni a
//következő pozíciót a maradék stockkal


BuildState PipeLine::addElementFromStock(const BuildState &state) {
    if (state.getStatus() == READY) {
        return state;
    }
    //ha az állapot befejezett, szimplán a befejezett állapotot fogja visszaadni
    if (state.getPosition() == INVALID_POSITION) {
        return {INVALID_POSITION, state.getStock(), READY, PostIt, 0};
    }
    //ha a pozíció invalid, tehát olyan helyre akarunk Tile-t tenni, amelyre nem lehet, akkor egy olyan
    //állapottal tér vissza, amely az invalid pozíciót, az adott állapotban lévő készletet adja vissza,
    //illetve késznek állítja az állapotot

    GridPosition currentPosition = state.getPosition();

    if (!isEmpty(currentPosition)) {
        while (!isEmpty(currentPosition)) {
            currentPosition = ++currentPosition;
            if (currentPosition == INVALID_POSITION) {
                return {INVALID_POSITION, state.getStock(), READY, PostIt, 0};
            }
        }
    }
//Inicializálja a currentPosition változót az aktuális állapot pozíciójával.
//Ha az aktuális pozíció nem üres, iterálva keresi a következő üres pozíciót, növelve a currentPosition értékét.
//A ++ a GridPositionben van megírva, ez már valójában úgy megy végig, hogy először a sorokon, majd az oszlopokon, miközben a
//második dimenziót is bejárja (lásd. Gridposition: GridPosition operator++() const
//Ha érvénytelen pozícióhoz ér, egy új, kész állapotra (BuildState) tér vissza INVALID_POSITION-nel, az aktuális készlettel és egy PostIt csempével.

    if (state.getStock().contains(state.getCurrentTile())) {
        Stock newStock = state.getStock() - state.getCurrentTile();


        if (state.getStatus() == TRY_NEXT || !canPut(currentPosition, state.getCurrentTile().rotate(state.getRotation()))) {
            if (state.getRotation() == 3 || state.getCurrentTile() == PostIt) {
                Tile newTile = state.getStock().getNextTile(state.getCurrentTile());

                if (newTile == PostIt) {
                    return {INVALID_POSITION, state.getStock(), OUT_OF_STOCK, PostIt, 0};
                } else {
                    return {currentPosition, state.getStock(), IN_PROGRESS, newTile, 0};
                }
            } else {
                return {currentPosition, state.getStock(), IN_PROGRESS, state.getCurrentTile(), (Rotation) (state.getRotation() + 1)};
            }
        } else {
            put(currentPosition, state.getCurrentTile().rotate(state.getRotation()));
            states.push(std::make_shared<BuildState>(BuildState(currentPosition, state.getStock(), IN_PROGRESS, state.getCurrentTile(), state.getRotation())));
            return {++currentPosition, newStock, IN_PROGRESS, PostIt, 0};
        }

    } else {
        Tile newTile = state.getStock().getNextTile(state.getCurrentTile());
        if (newTile == PostIt) {
            return {INVALID_POSITION, state.getStock(), OUT_OF_STOCK, PostIt, 0};
        } else {
            return {currentPosition, state.getStock(), IN_PROGRESS, newTile, 0};
        }
    }
}
//Ellenőrzi, hogy a jelenlegi csempe megtalálható-e a készletben.
//Ha igen, frissíti a készletet azáltal, hogy eltávolítja a jelenlegi csempét, majd ellenőrzi, hogy a csempe elhelyezhető-e a currentPosition pozíciónál:
//Ha nem, lekéri és visszaadja a következő csempét a készletből:
//Ha a következő csempe PostIt, akkor egy kész, INVALID_POSITION-nel jelölt BuildState állapotot ad vissza.
//Ellenkező esetben visszaadja az új állapotot a következő csempével.
//Ha igen, elhelyezi a csempét, elmenti az előző állapotot, és visszaadja a frissített állapotot.
//Ha a jelenlegi csempe nincs a készletben:
//Lekéri a következő csempét a készletből:
//Ha a következő csempe PostIt, egy kész BuildState állapotot ad vissza INVALID_POSITION-nel.
//Egyébként az új állapotot adja vissza a következő csempével.

PipeLine &PipeLine::put(const GridPosition position, const Tile &tile) {
    if (canPut(position, tile)) {
        tiles[position] = tile;
    }
    return *this;
}


PipeLine::PipeLine(const Grid &grid) : grid(grid) {}

bool PipeLine::isEmpty(const GridPosition &position) const {
    return !tiles.contains(position);
}

bool PipeLine::clear(const GridPosition &position) {
    if (tiles.contains(position)) {
        tiles.remove(position);
        return true;
    }
    return false;
}

QList<GridPosition> PipeLine::getSourcePositions(TileColor color) const {
    GridPosition currentPosition = GridPosition(grid, 0, 0, 0);
    QList<GridPosition> result;
    while (currentPosition != INVALID_POSITION) {
        Tile currentTile = (*this)[currentPosition];
        if (currentTile.getColor() == color && currentTile.getType() == SOURCE) {
            result.push_back(currentPosition);
        }
        currentPosition=++currentPosition;
    }
    return result;
}

QList<GridPosition> PipeLine::getSinkPositions(TileColor color) const {
    GridPosition currentPosition = GridPosition(grid, 0, 0, 0);
    QList<GridPosition> result;
    while (currentPosition != INVALID_POSITION) {
        Tile currentTile = (*this)[currentPosition];
        if (currentTile.getColor() == color && currentTile.getType() == SINK) {
            result.push_back(currentPosition);
        }
        currentPosition=++currentPosition;
    }
    return result;
}

QString PipeLine::toQString(bool prefix) const {
    QString result = prefix ? "|" : "";
    for (int i = 0; i < grid.getHeight(); i++) {
        for (int j = 0; j < grid.getWidth(); j++) {
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            Tile currentTile =  (*this)[currentPosition];
            Tile otherTile =  (*this)[currentPosition.step(OTHER_STACK)];
            if (currentTile.isPostIt()) {
                Tile changedTile = otherTile;
                otherTile = currentTile;
                currentTile = changedTile;
            }
            if (currentTile.isPostIt()) {
                result += "     ";
            } else {
                if (currentTile.isCorner() && otherTile.isCorner()) {
                    result += "-:-" + typeAndColorToChar(currentTile, true);
                } else {
                    int leftSide = currentTile.getConnections() & CSP_LEFT;
                    int rightSide = currentTile.getConnections() & CSP_RIGHT;
                    int topSide = currentTile.getConnections() & CSP_TOP;
                    int bottomSide = currentTile.getConnections() & CSP_BOTTOM;
                    if (leftSide != 0) {
                        result += "-";
                    } else {
                        result += " ";
                    }
                    if (topSide != 0) {
                        if (bottomSide != 0) {
                            result += "|";
                        } else {
                            result += "'";
                        }
                    } else {
                        if (bottomSide != 0) {
                            result += ".";
                        } else {
                            if (leftSide != 0 && rightSide != 0) {
                                result += "-";
                            } else {
                                result += " ";
                            }
                        }
                    }
                    if (rightSide != 0) {
                        result += "-";
                    } else {
                        result += " ";
                    }
                    result += typeAndColorToChar(currentTile, rightSide != 0);
                }
            }
        }
        result += QString("\n") + (prefix ? "|" : "");
    }
    return result;
}

QString PipeLine::typeAndColorToChar(Tile tile, bool hasRightItem) {
    QString result;
    switch (tile.getType()) {
        case NORMAL:
            result += hasRightItem ? "-" : " ";
            break;
        case SOURCE:
            result += "o";
            break;
        case SINK:
            result += "c";
            break;
        case VALVE:
            result += "0";
            break;
    }
    switch (tile.getColor()) {
        case RED:
            result += "R";
            break;
        case CIAN:
            result += "C";
            break;
        case BLUE:
            result += "B";
            break;
        case GREEN:
            result += "G";
            break;
        case YELLOW:
            result += "Y";
            break;
        case NONE:
            result += hasRightItem ? "-" : " ";
            break;
    }
    return result;
}

PipeLine PipeLine::fromString(const QString &string) {
    QMap<GridPosition, Tile> tiles;
    QStringList AllLines = string.split("\n");
    QStringList lines;

    for (int i = 0; i < AllLines.size(); i++) {
        if (!AllLines[i].isEmpty()) {
            lines.push_back(AllLines[i]);
        }
    }

    if (lines.empty()) {
        throw std::invalid_argument("Invalid string: " + string.toStdString());
    }
    Grid grid = Grid(lines[0].size() / 5, lines.size());

    for (int i = 0; i < lines.size(); i++) {
        QString line = lines[i];

        if (line.size() / 5 != grid.getWidth() || line.size() % 5 != 0) {
            throw std::invalid_argument("Invalid string: " + line.toStdString());
        }

        for (int j = 0; j < line.size(); j += 5) {
            // Majd kell a -:- és a -;- kezelése
            const QString &tileStr = line.mid(j, 5);
            if (tileStr.mid(0,3) == "-:-") {
                Tile tileUp = Tile::fromString("-'   ");
                Tile tileDown = Tile::fromString(" .---");
                GridPosition currentPosition = GridPosition(grid, 0, j / 5, i);
                GridPosition currentPositionOther = GridPosition(grid, 1, j / 5, i);
                tiles[currentPosition] = tileUp;
                tiles[currentPositionOther] = tileDown;
                continue;
            }
            if (tileStr.mid(0,3) == "-;-") {
                Tile tileUp = Tile::fromString(" '---");
                Tile tileDown = Tile::fromString("-.   ");
                GridPosition currentPosition = GridPosition(grid, 0, j / 5, i);
                GridPosition currentPositionOther = GridPosition(grid, 1, j / 5, i);
                tiles[currentPosition] = tileUp;
                tiles[currentPositionOther] = tileDown;
                continue;
            }
            Tile tile = Tile::fromString(tileStr);
            if (tile != PostIt) {
                GridPosition currentPosition = GridPosition(grid, 0, j / 5, i);
                tiles[currentPosition] = tile;
            }
        }
    }
    PipeLine pipeLine = PipeLine(grid);
    pipeLine.tiles = tiles;
    return pipeLine;
}

QList<GridPosition> PipeLine::getValvePositions() const{
    QList<GridPosition> result;
    GridPosition currentPosition = GridPosition(grid, 0, 0, 0);
    while (currentPosition != INVALID_POSITION) {
        Tile currentTile = (*this)[currentPosition];
        if (currentTile.getType() == VALVE) {
            result.push_back(currentPosition);
        }
        currentPosition = ++currentPosition;
    }
    return result;
}

const Grid &PipeLine::getGrid() const {
    return grid;
}

QPair<bool, BuildState> PipeLine::stepBack() {
    if (states.empty()) {
        return {false, BuildState(INVALID_POSITION, Stock(), ERROR, PostIt, 0)};
    }
    BuildState state = *states.pop();
    clear(state.getPosition());
    return {true, state};
}

