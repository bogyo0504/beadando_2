//
// Created by gyberni15 on 10/30/24.
//

#include "Grid.h"

Grid::Grid(int width, int height) : width(width), height(height) {}

const int Grid::getWidth() const {
    return width;
}

const int Grid::getHeight() const {
    return height;
}
