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

bool PipeLine::checkConnection(GridPositionStep step, const GridPosition position, const Tile &tile) const {
    const Tile other1 = (*this)[position.step(step)];
    const Tile other2 = (*this)[position.step(step).step(OTHER_STACK)];
    int connections = other1.getConnections() | other2.getConnections();

    int following = connections;        //ha jobbre vagy lefele megyünk
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

//Megpróbálni a stockból a currenttilet kiszedni, ha a  nincs készlet a stockban az adott tileból, akkor
//meg kell keresni a következő stockból a következő tile-t és az így kapott tile-t be kell rakni a gridbe és vissza kell adni a
//következő pozíciót a maradék stockkal


BuildState PipeLine::addElementFromStock(const BuildState &state) {
    if (state.getIsReady()) {
        return state;
    }
    //ha az állapot befejezett, szimplán a befejezett állapotot fogja visszaadni
    if (state.getPosition() == INVALID_POSITION) {
        return {INVALID_POSITION, state.getStock(), true, PostIt};
    }
    //ha a pozíció invalid, tehát olyan helyre akarunk Tile-t tenni, amelyre nem lehet, akkor egy olyan
    //állapottal tér vissza, amely az invalid pozíciót, az adott állapotban lévő készletet adja vissza,
    //illetve késznek állítja az állapotot

    GridPosition currentPosition = state.getPosition();

    if (!isEmpty(currentPosition)) {
        while (!isEmpty(currentPosition)) {
            ++currentPosition;
            if (currentPosition == INVALID_POSITION) {
                return {INVALID_POSITION, state.getStock(), true, PostIt};
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
        if (!canPut(currentPosition, state.getCurrentTile())) {
            Tile newTile = state.getStock().getNextTile(state.getCurrentTile());
            if (newTile == PostIt) {
                return {INVALID_POSITION, state.getStock(), true, PostIt};
            } else {
                return {currentPosition, state.getStock(), false, newTile};
            }
        } else {
            put(currentPosition, state.getCurrentTile());
            states.push(std::make_shared<BuildState>(state));
            return {++currentPosition, newStock, false, PostIt};
        }
    } else {
        Tile newTile = state.getStock().getNextTile(state.getCurrentTile());
        if (newTile == PostIt) {
            return {INVALID_POSITION, state.getStock(), true, PostIt};
        } else {
            return {currentPosition, state.getStock(), false, newTile};
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
