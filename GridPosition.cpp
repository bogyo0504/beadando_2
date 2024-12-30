//
// Created by gyberni15 on 10/30/24.
//

#include "GridPosition.h"

GridPosition::GridPosition(const Grid &grid, const int stack, const int horizontal, const int vertical) : grid(grid),
                                                                                                          stack(stack),
                                                                                                          horizontal(
                                                                                                                  horizontal),
                                                                                                          vertical(
                                                                                                                  vertical) {}


const int GridPosition::getStack() const {
    return stack;
}

const int GridPosition::getHorizontal() const {
    return horizontal;
}

const int GridPosition::getVertical() const {
    return vertical;
}

bool GridPosition::operator==(const GridPosition &rhs) const {
    return stack == rhs.stack &&
           horizontal == rhs.horizontal &&
           vertical == rhs.vertical;
}

bool GridPosition::operator!=(const GridPosition &rhs) const {
    return !(rhs == *this);
}

bool GridPosition::operator<(const GridPosition &rhs) const {
    if (vertical < rhs.vertical)
        return true;
    if (rhs.vertical < vertical)
        return false;
    if (horizontal < rhs.horizontal)
        return true;
    if (rhs.horizontal < horizontal)
        return false;
    return stack < rhs.stack;
}

bool GridPosition::operator>(const GridPosition &rhs) const {
    return rhs < *this;
}

bool GridPosition::operator<=(const GridPosition &rhs) const {
    return !(rhs < *this);
}

bool GridPosition::operator>=(const GridPosition &rhs) const {
    return !(*this < rhs);
}

GridPosition GridPosition::operator++() const {
    if (stack < 0 || horizontal < 0 || vertical < 0 || stack > 1) {
        return INVALID_POSITION;
    }
    if (stack == 0) {
        return {grid, 1, horizontal, vertical}; // GridPosition(grid, 1, horizontal, vertical);
    }
    if (horizontal < grid.getWidth() - 1) {
        return {grid, 0, horizontal + 1, vertical};
    }
    if (vertical < grid.getHeight() - 1) {
        return {grid, 0, 0, vertical + 1};
    }
    return INVALID_POSITION;
}

GridPosition GridPosition::operator--() const {
    if (stack < 0 || horizontal < 0 || vertical < 0 || stack > 1 || horizontal >= grid.getWidth() || vertical >= grid.getHeight()) {
        return INVALID_POSITION;
    }
    if (stack == 1) {
        return {grid, 0, horizontal, vertical};
    }
    if (horizontal > 0) {
        return {grid, 1, horizontal - 1, vertical};
    }
    if (vertical > 0) {
        return {grid, 1, grid.getWidth() - 1, vertical - 1};
    }
    return INVALID_POSITION;
}

GridPosition GridPosition::step(const GridPositionStep step) const {
    switch (step) {
        case UP:
            if (vertical > 0) {
                return {grid, stack, horizontal, vertical - 1};
            }
            break;
        case DOWN:
            if (vertical < grid.getHeight() - 1) {
                return {grid, stack, horizontal, vertical + 1};
            }
            break;
        case LEFT:
            if (horizontal > 0) {
                return {grid, stack, horizontal - 1, vertical};
            }
            break;
        case RIGHT:
            if (horizontal < grid.getWidth() - 1) {
                return {grid, stack, horizontal + 1, vertical};
            }
            break;
        case OTHER_STACK:
            return {grid, stack == 0 ? 1 : 0, horizontal, vertical}; // stack == 0 ? 1 : 0, ha stack 0 akkor 1, ha 1 akkor 0
    }
    return INVALID_POSITION;
}

bool GridPosition::isLast() const {
    return stack == 1 && horizontal == grid.getWidth() - 1 && vertical == grid.getHeight() - 1;
}

QString GridPosition::toQString() const {
    return QString::number(stack) + " " + QString::number(horizontal) + " " + QString::number(vertical);
}



