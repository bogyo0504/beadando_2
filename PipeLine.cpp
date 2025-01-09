//
// Created by gyberni15 on 10/30/24.
//
#include "PipeLine.h"

bool PipeLine::canPut(const GridPosition position, const Tile &tile) const {
    //Ha nincs overlaping, akkor csak az első stack-re lehet rakni
    if (!overlappingEnabled && position.getStack() == 1 && tile != PostIt) {
        return false;
    }
    //Az 1-s szintre csak PostIt vagy sarok rakható
    if(!tile.isPostIt() && !tile.isStackableCorner() && position.getStack() == 1){
        return false;
    }
    const GridPosition &otherPosition = position.step(OTHER_STACK);
    if (tile.isPostIt()) {
        //A PostIt esetén, ha a másik pozícióban van valami, akkor csak megfelelő szomszédokkal lehet rátenni, tehát, ha szomszédjában nincs nyitott cső
        //Ha másik stack is üres, a pozícióra PostIt csak akkor rakható, ha ez a 0-s stack
        //összességében egymásra két PostIt-et ne rakjunk (ez egy vágás az algoritmusban)
        if (tiles.contains(otherPosition)) {
            return hasValidNeighbourghs(position, tile, 0);
        }
        return position.getStack() == 0;
    }
    //Az egyes stack-re csak akkor rakhatunk, ha az alsó nem üres
    if (!tiles.contains(otherPosition) && position.getStack() == 1) {
        return false;
    }
    //Ha az otherPosition üres akkor az other egy PostIt
    const Tile other = (*this)[otherPosition];
    //Ha a csempe sarok, akkor csak komplementer sarokra rakható vagy PostIt-re, és csak Normal típusú csempére
    if (tile.isStackableCorner()) {
        if (!other.isPostIt()) {
            if (!other.isStackableCorner() || other == tile) {
                return false;
            }
            if (other.getConnections() !=
                (tile.getConnections() ^
                 15)) { //^15 a XOR, tehát visszaadja a komplementert pl. 1100-nak 0011-et, tehát a 12-re a 3
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
        //Minden más csak PostIt-re rakható rá
        if (!other.isPostIt()) {
            return false;
        }
    }
    //Csak akkor rakható rá a mezőre, ha me, hagyunk a szomszédokban nyitott csöveket
    return hasValidNeighbourghs(position, tile, 0);

}

Tile PipeLine::operator[](const GridPosition &position) const {
    if (tiles.contains(position)) {
        return tiles[position];
    } else {
        return PostIt;
    }
}

bool PipeLine::checkConnection(const GridPosition &position, const GridPositionStep step, bool otherLevel) const {
    const Tile tile = (*this)[position];
    const Tile other = otherLevel ? (*this)[position.step(step).step(OTHER_STACK)] : (*this)[position.step(step)];
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
}

BuildState PipeLine::applyBuildState(const BuildState &state, QStack<std::shared_ptr<BuildState>> &states) {
    //ha az állapot befejezett, szimplán a befejezett állapotot fogja visszaadni
    if (state.getStatus() == READY) {
        return state;
    }

    //ha a pozíció invalid, tehát olyan helyre akarunk Tile-t tenni, amelyre nem lehet, akkor egy olyan
    //állapottal tér vissza, amely az invalid pozíciót, az adott állapotban lévő készletet adja vissza,
    //illetve késznek állítja az állapotot
    // Ezzel stabilizáljuk az algoritmust, hogy ne akarjon lejebb menni az építési fájlban
    if (state.getPosition() == INVALID_POSITION) {
        return {INVALID_POSITION, state.getStock(), READY, PostIt, 0};
    }


//Inicializálja a currentPosition változót az aktuális állapot pozíciójával.
//Ha az aktuális pozíció nem üres, iterálva keresi a következő üres pozíciót, növelve a currentPosition értékét.
//A ++ a GridPositionben van megírva, ez már valójában úgy megy végig, hogy először a sorokon, majd az oszlopokon, miközben a
//második dimenziót is bejárja (lásd. Gridposition: GridPosition operator++() const
//Ha érvénytelen pozícióhoz ér, egy új, kész állapotra (BuildState) tér vissza INVALID_POSITION-nel, az aktuális készlettel és egy PostIt csempével.
    GridPosition currentPosition = state.getPosition();

    if (!isEmpty(currentPosition)) {
        while (!isEmptyAndAvailable(currentPosition)) {
            currentPosition = ++currentPosition;
            //Ha nincs további csempehely
            if (currentPosition == INVALID_POSITION) {
                return {INVALID_POSITION, state.getStock(), READY, PostIt, 0};
            }
        }
        return {currentPosition, state.getStock(), IN_PROGRESS, PostIt, 0};
    }
    //Fontos, hogy a stockban legyen olyan csempe, amit le akarunk rakni
    //Megjegyzés PostIt mindig van, bár soha sem lehet valódi készlete
    if (state.getStock().contains(state.getCurrentTile())) {
        //Kivesszük a készletből a csempét
        Stock newStock = state.getStock() - state.getCurrentTile();
        //A csempét elforgatjuk a BuildState-ben tárolt forgatási állapotnak megfelelően
        const Tile &rotatedTile = state.getCurrentTile().rotate(state.getRotation());
        //Ha a csempét nem lehet az adott pozícióra rakni vagy muszáj váltani csempét az adott pozícióra, akkor
        //megpróbálja a következő csempét kiválasztani
        if (state.getStatus() == TRY_NEXT || !canPut(currentPosition, rotatedTile)) {
            //Ha már nem tudjuk forgatni a csempét, akkor a következő lerakható csempét választjuk ki
            //A lerakhatóság helyben vizsgálata egy optimalizásiós lépés

            Rotation nextRotation = findRotation(currentPosition, state.getCurrentTile(), state.getRotation() + 1);
            if (nextRotation != INVALID_ROTATION) {
                return {currentPosition, state.getStock(), IN_PROGRESS, state.getCurrentTile(), nextRotation};
            }
            Tile currentTile = state.getCurrentTile();
            //Ez a ciklus addig megy, amíg a készletben talál egy olyan csempét, amelyet rá lehet tenni a pozícióra vagy a készletben nincs több csempe
            while (true) {
                //Ez a készlet következő elemet adja vissza, ha a készletben nincs több elem, akkor PostIt-et ad vissza
                Tile newTile = state.getStock().getNextTile(currentTile);
                //Ha nincs több elem a készleten akkor ezt jelezzük és nem haladunk tovább az építési fában, ezért adunk INVALID_POSITION-t
                if (newTile == PostIt) {
                    return {INVALID_POSITION, state.getStock(), OUT_OF_STOCK, PostIt, 0};
                } else {
                    //Ha talál egy olyan csempét a készleten, akkor megkeresi a megfelelő forgatást és megvizsgálja, hogy az adott pozícióra rakható-e
                    //INVALID_ROTATION-t ad vissza, ha nem lehet rátenni a csempét
                    //Ha a csempe rátehető, akkor visszatérünk az új állapottal
                    const Rotation rotation = findRotation(currentPosition, newTile, 0);
                    if (rotation != INVALID_ROTATION) {
                        return {currentPosition, state.getStock(), IN_PROGRESS, newTile, rotation};
                    }
                }
                currentTile = newTile;
            }

        } else {
            //Ha lerakható ide a csempe akkor le is rakjuk a pozícióra
            //Fontos: az elforgatást is figyelembe kell venni
            //Belerakjuk a stackbe az adott állapotot, ezzel lejebb lépve az építési fában
            put(currentPosition, rotatedTile);
            states.push(std::make_shared<BuildState>(state));

            //Az új állapot (lelépünk a fában egyet) a következő pozíció, a csökkentett készlet, az IN_PROGRESS állapot, és a PostIt, 0-s forgatással
            return {++currentPosition, newStock, IN_PROGRESS, PostIt, 0};
        }

    } else {
        //Ha valamiért nincs a készletben a BuildState-ben lévő csempe, akkor megpróbáljuk a következő csempét kiválasztani
        //Ez az ág az algoritmusban elvileg nem kap szerepet, csak stabilizációs szerepe van
        const Tile &newTile = state.getStock().getNextTile(state.getCurrentTile());
        if (newTile == PostIt) {
            return {INVALID_POSITION, state.getStock(), OUT_OF_STOCK, PostIt, 0};
        } else {
            return {currentPosition, state.getStock(), IN_PROGRESS, newTile, 0};
        }
    }
}

PipeLine &PipeLine::put(const GridPosition position, const Tile &tile) {
    tiles[position] = tile;
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
        currentPosition = ++currentPosition;
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
        currentPosition = ++currentPosition;
    }
    return result;
}

QString PipeLine::toQString(bool prefix) const {
    QString result = prefix ? "|" : "";
    for (int i = 0; i < grid.getHeight(); i++) {
        for (int j = 0; j < grid.getWidth(); j++) {
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            Tile currentTile = (*this)[currentPosition];
            const GridPosition &otherPosition = currentPosition.step(OTHER_STACK);
            Tile otherTile = (*this)[otherPosition];
            if (currentTile.isPostIt()) {
                Tile changedTile = otherTile;
                otherTile = currentTile;
                currentTile = changedTile;
            }
            if (currentTile.isPostIt()) {
                if (tiles.contains(currentPosition) && tiles[currentPosition] == PostIt) {
                    result += "*****";
                } else if (tiles.contains(otherPosition) && tiles[otherPosition] == PostIt) {
                    result += "#####";
                } else {
                    result += "     ";
                }

            } else {
                if (currentTile.isStackableCorner() && otherTile.isStackableCorner()) {
                    result += "-:-" + currentTile.typeAndColorToChar(true);
                } else {
                    result += currentTile.toQString();
                }
            }
        }
        result += QString("\n") + (prefix ? "|" : "");
    }
    return result;
}


PipeLine PipeLine::fromString(const QString &string) {
    QHash<GridPosition, Tile> tiles;
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
            if (tileStr.mid(0, 3) == "-:-") {
                Tile tileUp = Tile::fromString("-'   ");
                Tile tileDown = Tile::fromString(" .---");
                GridPosition currentPosition = GridPosition(grid, 0, j / 5, i);
                GridPosition currentPositionOther = GridPosition(grid, 1, j / 5, i);
                tiles[currentPosition] = tileUp;
                tiles[currentPositionOther] = tileDown;
                continue;
            }
            if (tileStr.mid(0, 3) == "-;-") {
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

QList<GridPosition> PipeLine::getValvePositions() const {
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

bool PipeLine::isEmptyAndAvailable(GridPosition position) {
    if (!isEmpty(position)) {
        return false;
    }
    const GridPosition &otherPosition = position.step(OTHER_STACK);
    if (!tiles.contains(otherPosition)) {
        return true;
    }
    const Tile &otherTile = tiles[otherPosition];
    if (otherTile.isStackableCorner() && otherTile.getType() == NORMAL) {
        return true;
    }
    return false;
}

Rotation PipeLine::findRotation(GridPosition position, Tile tile, Rotation startingRotation) const {
    tile = tile.rotate(startingRotation);
    for (int i = startingRotation; i < tile.getMaxPossibleRotation(); ++i) {
        if (canPut(position, tile)) {
            return (Rotation) i;
        }
        tile = tile.rotate(1);
    }
    return INVALID_ROTATION;
}

void PipeLine::disableOverlap() {
    overlappingEnabled = false;
}

bool PipeLine::hasValidNeighbourghs(const GridPosition &position, const Tile &tile, int checkLevel) const {
    return hasValidNeighbourgh(position, tile, UP, CSP_TOP, CSP_BOTTOM, checkLevel) &&
           hasValidNeighbourgh(position, tile, DOWN, CSP_BOTTOM, CSP_TOP, checkLevel) &&
           hasValidNeighbourgh(position, tile, LEFT, CSP_LEFT, CSP_RIGHT, checkLevel) &&
           hasValidNeighbourgh(position, tile, RIGHT, CSP_RIGHT, CSP_LEFT, checkLevel);
}

bool PipeLine::hasValidNeighbourgh(const GridPosition &position, const Tile &tile, GridPositionStep step,
                                   int selfConnectionMask, int otherConnectionMask, int checkLevel) const {
    //Ez a szomszéd, illetve a szomszéd feletti/alatti csempe pozíciója
    const GridPosition &otherPosition = position.step(step);
    const GridPosition &otherStackOtherPosition = otherPosition.step(OTHER_STACK);
    //Az adott csempéből az adott irányba van-e nyitott cső
    bool myConnection = tile.hasConnectionInDirection(selfConnectionMask);
    //Ha a "lelógunk" a gridről, azaz a megfelelő irányba lévő szomszéd nincs a griden akkor abba az irányba nem lehet nyitott cső
    if (otherPosition == INVALID_POSITION || otherStackOtherPosition == INVALID_POSITION) {
        return !myConnection;
    }
    //Ha mindkét szinten üres a szomszéd mező, akkor a csempe letehető ebből az irányból nézve
    if (!tiles.contains(otherPosition) && !tiles.contains(otherStackOtherPosition)) {
        return true;
    }
    //A szomszédos csempék és a megfelelő irányban vett csőkapcsolatok, az adott csempe irányába
    const Tile &otherTile = (*this)[otherPosition];
    const Tile &otherStackOtherTile = (*this)[otherStackOtherPosition];
    bool otherConnection = otherTile.hasConnectionInDirection(otherConnectionMask);
    bool otherStackOtherConnection = otherStackOtherTile.hasConnectionInDirection(otherConnectionMask);
    //Ha az adott csempe PostIt
    if (tile.isPostIt()) {
        const GridPosition &pairPosition = position.step(OTHER_STACK);
        const Tile &pairTile = (*this)[pairPosition];
        // Ha a párja nem PostIt
        if (!pairTile.isPostIt()) {
            // Ha a párja kanyar és nincs kapcsolat azon a szomszéddal, akkor ez nem lehet postIt
            if (pairTile.isStackableCorner()) {
                const bool cornerHasConnection = pairTile.hasConnectionInDirection(selfConnectionMask);
                if (!cornerHasConnection && (otherConnection || otherStackOtherConnection)) {
                    return false;
                }
            }
            // PostIt letehető
            return true;
        }
    }

    //Ha van nyitott cső a csempéből és valamelyik szomszédnak is van erre a csempére nyitott csöve, akkor le lehet rakni a csempét
    //Ha nincs egyiknek sem nyitott csöve akkor biztos nem rakható le
    if (myConnection) {
        if (otherConnection || otherStackOtherConnection) {
            return true;
        }
        //Ha már a szomszédoknál mindkét szinten van csempe és nincs nyitott cső, akkor nem lehet rátenni a csempét
        if (tiles.contains(otherPosition) && tiles.contains(otherStackOtherPosition)) {
            return false;
        }
        // Ha csak az egyik szinten van csempe, de az nem kanyar, akkor nem lehet rátenni a csempét, tehát már biztos
        // hogy nem lesz kapcsolat.
        if (tiles.contains(otherPosition) && !otherTile.isStackableCorner()) {
            return false;
        }
        if (tiles.contains(otherStackOtherPosition) && !otherStackOtherTile.isStackableCorner()) {
            return false;
        }
        // Ezen ponton tudjuk, hogy a szomszédon csak egy csempe van, és az egy kanyar, ami "rossz" irányban van.
        if (checkLevel > 2) {
            return true;
        }
        // Ez a "rossz" kanyar kapcsolatai:
        int otherConnections = tiles.contains(otherPosition) ? otherTile.getConnections() : otherStackOtherTile.getConnections();
        // A rossz kanyar "feletti" pozíció:
        GridPosition potentialPosition =tiles.contains(otherPosition) ? otherStackOtherPosition : otherPosition;
        // Csempe, ami a rossz kanyar fölé tudna kerülni (átellenes kanyar)
        Tile potentialNeighbour = Tile(otherConnections ^ 15, NORMAL, NONE);
        // Megnézzük, hogy arra a helyre a "rossz" kanyar párja rákerülhet-e
        // Rekurzív hívás, ezért a checkLevel-el korlátozzuk, milyen mélyen nézzük
        return hasValidNeighbourghs(potentialPosition, potentialNeighbour, checkLevel + 1);

    }
    //Ha a csempe kanyar és a másik vagy kanyar vagy PostIt, akkor le lehet rakni
    //Bár a csempéből nincs a szomszéd felé kapcsolat, de később a rárakott csempe miatt még lehet
    if (tile.isStackableCorner()) {
        const GridPosition &pairPosition = position.step(OTHER_STACK);
        const Tile &pairTile = (*this)[pairPosition];
        if (pairTile.isPostIt()) {
            return true;
        }
        if (pairTile.isStackableCorner()) {
            if (otherConnection || otherStackOtherConnection) {
                bool pairHasConnection = pairTile.hasConnectionInDirection(selfConnectionMask);
                return pairHasConnection;
            }
            // Ha ez egy kanyar kombináció, és viszont a szomszédoktól nincs ide kapcsolat, az nem jó
            // mert innen van  kapcsolat
            return false;
        }
    }
    //Ha nincs abban az irányban kapcsolat akkor a szomszédban sem lehet errefelé kapcsolat
    return !otherConnection && !otherStackOtherConnection;
}

PipeLine PipeLine::resizeGrid(int width, int height) const {
    PipeLine copy(Grid(width, height));
    for (auto it = tiles.begin(); it != tiles.end(); ++it) {
        const GridPosition &position = it.key();
        const Tile &tile = it.value();
        if (position.getHorizontal() >= width || position.getVertical() >= height) {
            continue;
        }
        copy.put(GridPosition(copy.grid, position.getStack(), position.getHorizontal(), position.getVertical()), tile);
    }
    return copy;
}

void PipeLine::removePostIts() {
    QList<GridPosition> postItPositions;
    for (auto it = tiles.begin(); it != tiles.end(); ++it) {
        if (it.value().isPostIt()) {
            postItPositions.push_back(it.key());
        }
    }
    for (const GridPosition &position: postItPositions) {
        tiles.remove(position);
    }
}




