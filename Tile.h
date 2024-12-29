//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_TILE_H
#define BEADANDO_II_TILE_H

#define ROTATION_MASK 3
typedef short int Rotation;

#define CSP_TOP 8
#define CSP_RIGHT 4
#define CSP_BOTTOM 2
#define CSP_LEFT 1

enum TileType {
    NORMAL, SOURCE, SINK, VALVE
};
/**
 * A Tile class reprezentálja a játéktábla egy mezőjét.
 */

enum TileColor {
    NONE, RED, CIAN, BLUE, GREEN, YELLOW
};


class Tile {
private:
    /**
     * A mezőn lévő csatlakozási pontok bitmezője (0-15).
     *
     * A CSP_TOP, CSP_RIGHT, CSP_BOTTOM, CSP_LEFT konstansokat használja (megadja, hogy melyik irányba van csatlakozási pont)
     */
    int connections;
    TileType type;
    TileColor color;
public:
    Tile(int connections, TileType type);

    Tile(int connections, TileType type, TileColor color);

    Tile();

    TileColor getColor() const;

    void setColor(TileColor color);

    int getConnections() const;

    void setConnections(int connections);

    TileType getType() const;

    void setType(TileType type);

    bool operator==(const Tile &rhs) const;

    bool operator!=(const Tile &rhs) const;

    /*A QMap-ben való rendezéshez szükséges operátorok.
     */

    bool operator<(const Tile &rhs) const;

    bool operator>(const Tile &rhs) const;

    bool operator<=(const Tile &rhs) const;

    bool operator>=(const Tile &rhs) const;

    bool isCorner() const;

    bool isPostIt() const;

    Tile rotate(Rotation rotation) const;
};


/**
 * A PostIt egy konstans Tile, ami a játéktábla üres mezőjét reprezentálja.
 */

const Tile PostIt(0, NORMAL);


#endif //BEADANDO_II_TILE_H
