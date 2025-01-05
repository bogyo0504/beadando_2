#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressDialog>
#include "Tile.h"
#include "PipeLine.h"
#include "QBrush"
#include "PipeLineBuilder.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), currentPipes(new PipeLine(Grid(3, 3))){
    ui->setupUi(this);
    fillComboBoxes();
    updateGrid();
    updatePhases();
}

MainWindow::~MainWindow() {
    delete ui;
    delete currentPipes;
}

TileType MainWindow::stringToTileType(std::string s) {
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

TileColor MainWindow::stringToTileColor(std::string s) {
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


QString MainWindow::tileColorToName(TileColor color) {
    switch (color) {
        case RED:
            return "Piros";
        case CIAN:
            return "Cián";
        case BLUE:
            return "Kék";
        case GREEN:
            return "Zöld";
        case YELLOW:
            return "Sárga";
        default:
            return "";
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

void MainWindow::updateCurrentPipe() {
    QPixmap p(tilefilename(currenttile));
    QSize targetSize(100, 100);
    p = p.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->preview->setPixmap(p);
    int indx = ui->tileColorBox->findData(currenttile.getColor());
    if (indx != -1) {
        ui->tileColorBox->setCurrentIndex(indx);
    }
    indx = ui->tileTypeBox->findData(currenttile.getType());
    if (indx != -1) {
        ui->tileTypeBox->setCurrentIndex(indx);
    }
    indx = ui->tileConnectionBox->findData(currenttile.getConnections());
    if (indx != -1) {
        ui->tileConnectionBox->setCurrentIndex(indx);
    }
    if (currenttile.getType() == SINK || currenttile.getType() == SOURCE) {
        ui->tileColorBox->setEnabled(true);
    } else {
        ui->tileColorBox->setEnabled(false);
    }
}

void MainWindow::updateStock() {
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

void MainWindow::updatePhases() {
    ui->phasesWidget->clearContents();
    ui->phasesWidget->setRowCount(phases.size());
    int max = 0;
    for (const Phase &phase: phases) {
        if (phase.getActiveColors().size() > max) {
            max = phase.getActiveColors().size();
        }
    }
    ui->phasesWidget->setColumnCount(max);
    for (int i = 0; i < phases.size(); ++i) {
        Phase phase = phases[i];
        const QSet<TileColor> &colors = phase.getActiveColors();
        auto it = colors.begin();
        for (int j = 0; j < colors.size(); ++j) {
            const TileColor &color = *it;
            it++;
            QTableWidgetItem *item = new QTableWidgetItem(tileColorToName(color));
            item->setBackground(QBrush(tileColorToColor(color), Qt::SolidPattern));
            item->setData(Qt::UserRole, color);
            ui->phasesWidget->setItem(i, j, item);
        }
    }
}

void MainWindow::updateGrid() {
    ui->width->setValue(currentPipes->getGrid().getWidth());
    ui->height->setValue(currentPipes->getGrid().getHeight());
    ui->xpos->setMaximum(currentPipes->getGrid().getWidth());
    ui->ypos->setMaximum(currentPipes->getGrid().getHeight());
    ui->pipeline->setRowCount(currentPipes->getGrid().getHeight());
    ui->pipeline->setColumnCount(currentPipes->getGrid().getWidth());

    ui->pipeline->setShowGrid(false);
    const Grid &grid = currentPipes->getGrid();

    for (int i = 0; i < grid.getHeight(); ++i) {
        for (int j = 0; j < grid.getWidth(); ++j) {
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            GridPosition currentPositionOther = GridPosition(grid, 1, j, i);
            Tile current = (*currentPipes)[currentPosition];
            Tile other = (*currentPipes)[currentPositionOther];
            QString filename;
            if (current.isCorner() && other.isCorner()) {
                if (current.getConnections() == 3 || current.getConnections() == 12) {
                    filename = "pics/DOUBLE1.png";
                }
                if (current.getConnections() == 6 || current.getConnections() == 9) {
                    filename = "pics/DOUBLE2.png";
                }
            } else if (current.isPostIt() && other.isPostIt()) {
                filename = "pics/0.png";
            } else if (current.isPostIt()) {
                filename = tilefilename(other);
            } else {
                filename = tilefilename(current);
            }

            QPixmap pixmap(filename);
            pixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DecorationRole, pixmap);
            item->setBackground(QBrush(QColor(200, 200, 200), Qt::SolidPattern));
            ui->pipeline->setColumnWidth(j, 80);
            ui->pipeline->setRowHeight(i, 80);
            ui->pipeline->setItem(i, j, item);

        }
    }

}


void MainWindow::on_rotate_clicked() {
    currenttile = currenttile.rotate(1);
    updateCurrentPipe();
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
    if (currenttile.getType() == VALVE &&
        (currenttile.getConnections() == 0 || currenttile.getConnections() == 1 || currenttile.getConnections() == 2 ||
         currenttile.getConnections() == 4 || currenttile.getConnections() == 8)) {
        return;
    }
    currentstock = currentstock.addTile(currenttile, 1);
    updateStock();

}

void MainWindow::fillComboBoxes() {
    ui->tileConnectionBox->addItem("", 0);
    ui->tileConnectionBox->addItem("Zsákutca", 1);
    ui->tileConnectionBox->addItem("Egyenes", 5);
    ui->tileConnectionBox->addItem("Kanyar", 3);
    ui->tileConnectionBox->addItem("T-kanyar", 7);
    ui->tileConnectionBox->addItem("Kereszt", 15);

    ui->tileTypeBox->addItem("Sima", NORMAL);
    ui->tileTypeBox->addItem("Forrás", SOURCE);
    ui->tileTypeBox->addItem("Csap", VALVE);
    ui->tileTypeBox->addItem("Fogyasztó", SINK);

    ui->tileColorBox->addItem("Nincs", NONE);
    ui->tileColorBox->addItem("Piros", RED);
    ui->tileColorBox->addItem("Cián", CIAN);
    ui->tileColorBox->addItem("Kék", BLUE);
    ui->tileColorBox->addItem("Zöld", GREEN);
    ui->tileColorBox->addItem("Sárga", YELLOW);

    ui->chooseColor->addItem("Piros", RED);
    ui->chooseColor->addItem("Cián", CIAN);
    ui->chooseColor->addItem("Kék", BLUE);
    ui->chooseColor->addItem("Zöld", GREEN);
    ui->chooseColor->addItem("Sárga", YELLOW);

    updateCurrentPipe();

}


void MainWindow::on_tileConnectionBox_currentIndexChanged(int index) {

    currenttile.setConnections(ui->tileConnectionBox->itemData(index).toInt());

    updateCurrentPipe();
}


void MainWindow::on_tileTypeBox_currentIndexChanged(int index) {
    currenttile.setType((TileType) ui->tileTypeBox->itemData(index).toInt());

    updateCurrentPipe();
}


void MainWindow::on_tileColorBox_currentIndexChanged(int index) {
    currenttile.setColor((TileColor) ui->tileColorBox->itemData(index).toInt());
    updateCurrentPipe();
}


void MainWindow::on_addtogrid_clicked() {
    int x_pos = ui->xpos->value() - 1;
    int y_pos = ui->ypos->value() - 1;
    if (x_pos < 0 || y_pos < 0 || x_pos >= currentPipes->getGrid().getWidth() ||
        y_pos >= currentPipes->getGrid().getHeight()) {
        return;
    }
    GridPosition pos = GridPosition(currentPipes->getGrid(), 0, x_pos, y_pos);
    if (currenttile.isCorner() && (*currentPipes)[pos].isCorner()) {
        GridPosition pos_other = GridPosition(currentPipes->getGrid(), 1, x_pos, y_pos);
        if (currentPipes->canPut(pos_other, currenttile)) {
            currentPipes->put(pos_other, currenttile);
            updateGrid();
            return;
        }
    }
    if (currentPipes->canPut(pos, currenttile)) {
        currentPipes->put(pos, currenttile);
        currentPipes->clear(pos.step(OTHER_STACK));
        updateGrid();
    }

}


void MainWindow::on_delfromstock_clicked() {

    currentstock = currentstock - currenttile;
    updateStock();
}


void MainWindow::on_delfromgrid_clicked() {
    int x_pos = ui->xpos->value() - 1;
    int y_pos = ui->ypos->value() - 1;
    if (x_pos < 0 || y_pos < 0 || x_pos >= currentPipes->getGrid().getWidth() ||
        y_pos >= currentPipes->getGrid().getHeight()) {
        return;
    }
    GridPosition pos = GridPosition(currentPipes->getGrid(), 0, x_pos, y_pos);
    GridPosition pos_other = GridPosition(currentPipes->getGrid(), 1, x_pos, y_pos);

    if ((*currentPipes)[pos_other] != PostIt) {
        //currenttile = currentPipes[pos_other];
        //updateCurrentPipe(); //beálltíthatü vele, hogy a törölt elem legyen a currenttile
        currentPipes->clear(pos_other);
        updateGrid();
        return;
    }
    if ((*currentPipes)[pos] != PostIt) {
        //currenttile = currentPipes[pos];
        //updateCurrentPipe(); //beálltíthatü vele, hogy a törölt elem legyen a currenttile
        currentPipes->clear(pos);
    }
    updateGrid();

}


void MainWindow::on_actionMent_s_triggered() {
    QString mentes = "CsatornaFajl (*.cst)";
    QFileDialog dialog(this, "Mentés", "./",
                       mentes);
    dialog.setDefaultSuffix(".cst");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (!(dialog.exec())) {
        return;
    }
    QString fileName = dialog.selectedFiles().front();
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << currentstock.toQString();
    out << "[Phases]\n";
    for (const Phase &phase: phases) {
        out << phase.toQString() << "\n";
    }
    out << "[Pipes]\n";
    QStringList pipel =  currentPipes->toQString(false).split('\n');
    for (const QString &line: pipel) {
        out << line << '`' << "\n";
    }
}


void MainWindow::on_actionBet_lt_s_triggered() {
    QString betoltes = "CsatornaFajl (*.cst)";
    QFileDialog dialog(this, "Betöltés", "./",
                       betoltes);
    dialog.setDefaultSuffix(".cst");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (!(dialog.exec())) {
        return;
    }
    QString fileName = dialog.selectedFiles().front();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream in(&file);
    QString line;
    QStringList stocksList;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line == "[Phases]") {
            break;
        }

        stocksList.push_back(line);
    }

    currentstock = Stock::fromString(stocksList.join("\n"));
    phases.clear();
    while (!in.atEnd()) {
        line = in.readLine();
        if (line == "[Pipes]") {
            break;
        }
        Phase phase = Phase::fromString(line);
        phases.push_back(phase);
    }
    QStringList pipesList;
    while (!in.atEnd()) {
        line = in.readLine();
        if(line.endsWith('`')){
            line = line.mid(0,line.size()-1);
        }
        if (line.size() % 5 != 0) {
            line += QString(5 - line.size() % 5, ' ');
        }
        pipesList.push_back(line);
    }
    delete currentPipes;
    currentPipes = new PipeLine(PipeLine::fromString(pipesList.join("\n")));
    updateStock();
    updateGrid();
    updatePhases();
}


void MainWindow::on_width_valueChanged(int arg1) {
    PipeLine copy = currentPipes->resizeGrid(arg1, currentPipes->getGrid().getHeight());
    delete currentPipes;
    currentPipes = new PipeLine(copy);
    updateGrid();
}


void MainWindow::on_height_valueChanged(int arg1) {
    PipeLine copy = currentPipes->resizeGrid(currentPipes->getGrid().getWidth(), arg1);
    delete currentPipes;
    currentPipes = new PipeLine(copy);
    updateGrid();
}


void MainWindow::on_pipeline_cellClicked(int row, int column) {
    ui->xpos->setValue(column + 1);
    ui->ypos->setValue(row + 1);
}

QColor MainWindow::tileColorToColor(TileColor color) {
    switch (color) {
        case RED:
            return QColor(200, 0, 0);
        case CIAN:
            return QColor(0, 200, 200);
        case BLUE:
            return QColor(0, 0, 200);
        case GREEN:
            return QColor(0, 200, 0);
        case YELLOW:
            return QColor(200, 200, 0);
        default:
            return QColor(0, 0, 0);
    }

}


void MainWindow::on_deleteColor_clicked() {
    int column = ui->phasesWidget->currentColumn();
    int row = ui->phasesWidget->currentRow();

    if (column == -1 || row == -1) {
        return;
    }

    Phase phase = phases[row];
    TileColor currentColor = (TileColor) ui->phasesWidget->item(row, column)->data(Qt::UserRole).toInt();
    QSet<TileColor> colors = phase.getActiveColors();
    colors.remove(currentColor);
    phases[row] = Phase(colors);
    updatePhases();
}


void MainWindow::on_addColor_clicked() {
    int row = ui->phasesWidget->currentRow();
    int column = ui->phasesWidget->currentColumn();
    if (row == -1) {
        row = 0;
    }


    TileColor color = (TileColor) ui->chooseColor->itemData(ui->chooseColor->currentIndex()).toInt();
    QSet<TileColor> colors = phases[row].getActiveColors();
    colors.insert(color);
    phases[row] = Phase(colors);
    updatePhases();
}


void MainWindow::on_deletePhase_clicked() {

    int row = ui->phasesWidget->currentRow();

    if (row == -1) {
        return;
    }
    phases.removeAt(row);
    updatePhases();
}


void MainWindow::on_addPhase_clicked()
{
    int row = ui->phasesWidget->currentRow();

    if (row == -1) {
        phases.push_back(Phase());
    }
    else {
        phases.insert(row, Phase());
    }
    updatePhases();
}


void MainWindow::on_action_j_2_triggered()
{  phases.clear();
    currentstock = Stock();
    delete currentPipes;
    currentPipes = new PipeLine(Grid(3, 3));
    updateStock();
    updateGrid();
    updatePhases();

}



void MainWindow::on_actionKil_p_s_triggered()
{
    exit(0);
}


void MainWindow::on_start_clicked(){
    if (solverIsRunning) {
        return;
    }
    solverIsRunning = true;
    QProgressDialog progressBar("Csővezeték építése", "Mégse", 0, 100, this);
    progressBar.setWindowModality(Qt::WindowModal);

    const PipeLineValidator &validator = WindowedFlowValidator(phases, progressBar);
    PipeLineBuilder builder = PipeLineBuilder(validator, *currentPipes);
    builder.resetBuild();
    QCoreApplication::processEvents();
    if (builder.build(currentstock) != INVALID) {
        updateGrid();
    }  else {
        QMessageBox::warning(this, "Hiba", "A csővezeték kirakása nem sikerült");
    }
    progressBar.close();
    solverIsRunning = false;
}


void MainWindow::on_stock_cellClicked(int row, int column)
{   if (column == -1) {
        return;
    }
    Tile key = currentstock.enumStock().keys().at(column);
    currenttile = key;
    updateCurrentPipe();
}

