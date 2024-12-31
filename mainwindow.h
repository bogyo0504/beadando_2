#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    TileType stringtoTileType(std::string s);

    TileColor stringtoTileColor(std::string s);

    std::string TileColorToString(TileColor color);

    std::string TileTypeToString(TileType type);

    QString tilefilename(Tile t);


    Tile currenttile;
    Stock currentstock;

    void updatecurrentpipe();

    void updatestock();

    void updategrid(PipeLine &p);

private slots:

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_2_currentTextChanged(const QString &arg1);

    void on_comboBox_3_currentTextChanged(const QString &arg1);

    void on_rotate_clicked();

    void on_addtostock_clicked();
};

#endif // MAINWINDOW_H
