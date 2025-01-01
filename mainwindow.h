#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "Tile.h"
#include "Stock.h"
#include "PipeLine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;

    TileType stringToTileType(std::string s);

    TileColor stringtoTileColor(std::string s);

    std::string TileColorToString(TileColor color);

    std::string TileTypeToString(TileType type);

    QString tilefilename(Tile t);

    QGraphicsScene *scene;
    Tile currenttile;
    Stock currentstock;
    PipeLine* currentPipes;

    void updateCurrentPipe();

    void updateStock();

    void updateGrid();

private slots:

    void on_rotate_clicked();

    void on_addtostock_clicked();

    void fillComboBoxes();
    void on_tileConnectionBox_currentIndexChanged(int index);
    void on_tileTypeBox_currentIndexChanged(int index);
    void on_tileColorBox_currentIndexChanged(int index);
    void on_addtogrid_clicked();
    void on_delfromstock_clicked();
    void on_delfromgrid_clicked();
    void on_actionMent_s_triggered();
    void on_actionBet_lt_s_triggered();
    void on_width_valueChanged(int arg1);
    void on_height_valueChanged(int arg1);
    void on_pipeline_cellClicked(int row, int column);
};

#endif // MAINWINDOW_H
