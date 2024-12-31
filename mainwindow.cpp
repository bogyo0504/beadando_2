#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include "Tile.h"
#include "PipeLine.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

TileType MainWindow::stringtoTileType(std::string s) {
    if (s == "NORMAL") {
        return NORMAL;
    } else if (s == "SOURCE") {
        return SOURCE;
    } else if (s == "SINK") {
        return SINK;
    } else {
        return VALVE;
    }
}

TileColor MainWindow::stringtoTileColor(std::string s) {
    if (s == "RED") {
        return RED;
    } else if (s == "CIAN") {
        return CIAN;
    } else if (s == "BLUE") {
        return BLUE;
    } else if (s == "GREEN") {
        return GREEN;
    } else {
        return YELLOW;
    }
}

std::string MainWindow::TileColorToString(TileColor color) {
    switch (color) {
        case RED:
            return "RED";
        case CIAN:
            return "CIAN";
        case BLUE:
            return "BLUE";
        case GREEN:
            return "GREEN";
        case YELLOW:
            return "YELLOW";
        default:
            return "RED";
    }
}

std::string MainWindow::TileTypeToString(TileType type) {
    switch (type) {
        case NORMAL:
            return "NORMAL";
        case SOURCE:
            return "SOURCE";
        case SINK:
            return "SINK";
        case VALVE:
            return "VALVE";
        default:
            return "NORMAL";
    }
}


QString MainWindow::tilefilename(Tile t) {
    TileType type = t.getType();
    TileColor color = t.getColor();
    int connections = t.getConnections();
    if (t.isPostIt()) {
        return "pics/0.png";
    }

    if (type == SOURCE || type == SINK) {
        return QString::fromStdString(
                "pics/" + std::to_string(connections) + TileTypeToString(type) + TileColorToString(color) + ".png");
    }

    return QString::fromStdString("pics/" + std::to_string(connections) + TileTypeToString(type) + ".png");

}

void MainWindow::updatecurrentpipe() {
    QPixmap p(tilefilename(currenttile));
    QSize targetSize(100, 100);
    p = p.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->preview->setPixmap(p);
}

void MainWindow::updatestock() {
    ui->stock->clearContents();
    QMap<Tile, int> m = currentstock.enumStock();
    ui->stock->setColumnCount(m.size());
    ui->stock->setRowCount(2);
    int column = 0;
    for (auto it = m.begin(); it != m.end(); ++it) {
        Tile key = it.key();
        int value = it.value();
        QPixmap pixmap(tilefilename(key));
        pixmap = pixmap.scaled(86, 86, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->stock->setColumnWidth(column, 86);
        QTableWidgetItem *pixmapItem = new QTableWidgetItem();
        pixmapItem->setData(Qt::DecorationRole, pixmap);

        ui->stock->setItem(0, column, pixmapItem);
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString::number(value));
        ui->stock->setItem(1, column, valueItem);
        column += 1;
    }
}


void MainWindow::updategrid(PipeLine &p) {
    QGraphicsScene *scene = new QGraphicsScene();
    ui->pipeline->setScene(scene);
    const Grid &grid = p.getGrid();
    for (int i = 0; i < grid.getHeight(); ++i) {
        for (int j = 0; j < grid.getWidth(); ++j) {
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            GridPosition currentPositionOther = GridPosition(grid, 1, j, i);
            Tile current = p[currentPosition];
            Tile other = p[currentPositionOther];
            QString filename;
            if (current.isCorner() && other.isCorner()) {
                if (current.getConnections() == 3 || current.getConnections() == 12) {
                    filename = "pics//DOUBLE1.png";
                }
                if (current.getConnections() == 6 || current.getConnections() == 9) {
                    filename = "pics//DOUBLE2.png";
                }
            } else if (current.isPostIt() && other.isPostIt()) {
                filename = "pics/0.png";
            } else if (current.isPostIt()) {
                filename = tilefilename(other);
            } else {
                filename = tilefilename(current);
            }
            QPixmap pixmap(filename);
            pixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatio);
            int x = j * pixmap.width();
            int y = i * pixmap.height();
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
            item->setPos(x, y);
            scene->addItem(item);
        }
    }

}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) {
    if (arg1 == "Egyenes") {
        currenttile.setConnections(5);
    }
    if (arg1 == "Kanyar") {
        currenttile.setConnections(3);
    }
    if (arg1 == "T-kanyar") {
        currenttile.setConnections(7);
    }
    if (arg1 == "Kereszt") {
        currenttile.setConnections(15);
    }
    if (arg1 == "Zsákutca") {
        currenttile.setConnections(1);
    }
    updatecurrentpipe();
}


void MainWindow::on_comboBox_2_currentTextChanged(const QString &arg1) {
    if (arg1 == "Sima") {
        currenttile.setType(NORMAL);
    }
    if (arg1 == "Forrás") {
        currenttile.setType(SOURCE);
    }
    if (arg1 == "Csap") {
        currenttile.setType(VALVE);
    }
    if (arg1 == "Fogyasztó") {
        currenttile.setType(SINK);
    }
    updatecurrentpipe();
}


void MainWindow::on_comboBox_3_currentTextChanged(const QString &arg1) {
    if (arg1 == "Piros") {
        currenttile.setColor(RED);
    }
    if (arg1 == "Cián") {
        currenttile.setColor(CIAN);
    }
    if (arg1 == "Kék") {
        currenttile.setColor(BLUE);
    }
    if (arg1 == "Zöld") {
        currenttile.setColor(GREEN);
    }
    if (arg1 == "Sárga") {
        currenttile.setColor(YELLOW);
    }
    updatecurrentpipe();
}


void MainWindow::on_rotate_clicked() {
    currenttile = currenttile.rotate(1);
    updatecurrentpipe();
}


void MainWindow::on_addtostock_clicked() {
    if (currenttile.getConnections() == 0) {
        return;
    }
    if (currenttile.getType() == SOURCE || currenttile.getType() == SINK) {
        if (currenttile.getColor() == NONE) {
            return;
        }
    }
    if (currenttile.getType() == VALVE && (currenttile.getConnections() == 0 || currenttile.getConnections() == 1 || currenttile.getConnections() == 2 || currenttile.getConnections() == 4 || currenttile.getConnections() == 8)) {
        return;
    }
    currentstock = currentstock.addTile(currenttile, 1);
    updatestock();

}


