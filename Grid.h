//
// Created by gyberni15 on 10/30/24.
//

#ifndef BEADANDO_II_GRID_H
#define BEADANDO_II_GRID_H

/**
 * A Grid megadja a játéktér méretét.
 */

class Grid {
private:
    int width;
    int height;
public:
    Grid(int width, int height);

    const int getWidth() const;

    const int getHeight() const;

};


#endif //BEADANDO_II_GRID_H
