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
        : QMainWindow(parent), ui(new Ui::MainWindow), currentPipes(new PipeLine(Grid(3, 3))) {
    ui->setupUi(this);
    fillComboBoxes();
    updateGrid();
    updatePhases();
}

//Ha megnyitjuk a MainWidow-t, akkor kitörlünk mindent
MainWindow::~MainWindow() {
    delete ui;
    delete currentPipes;
    if (flow != nullptr) {
        delete flow;
    }
    clearRollBackPipeLine();
}

//A színt tudja string-gé alakítani
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

//A fázis táblájának feltöltéséhez szükséges, a TileColor enumot (magyar) string-gé alakítja
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

//A fájlok betöltésénél használjuk e szerint nyitjuk meg a megfelelő típusú képet
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

//Ennek a függvénynek a segítségével tudjuk a csempének a fájlnevét meghatározni
QString MainWindow::tilefilename(Tile t) {
    TileType type = t.getType();
    TileColor color = t.getColor();
    int connections = t.getConnections();
    //Ha a csempe egy PostIt, akkor a pics mappából a 0.png-t fogja visszaadni
    if (t.isPostIt()) {
        return "pics/0.png";
    }

    //Ha a csempe egy forrás vagy fogyasztó, akkor a pics mappából a megfelelő csatlakozással és színnel ellátott csempét fogja visszaadni
    if (type == SOURCE || type == SINK) {
        return QString::fromStdString(
                "pics/" + std::to_string(connections) + TileTypeToString(type) + TileColorToString(color) + ".png");
    }
    //Ez a sima és a csap csempe esetén fogja visszaadni a fájlnevet
    return QString::fromStdString("pics/" + std::to_string(connections) + TileTypeToString(type) + ".png");

}

//Ez a függvény az éppen szerkesztett csempe megjelenítéséhez szükséges
void MainWindow::updateCurrentPipe() {
    //A csempe képének fájlját alakítja pixmap-é.
    QPixmap p(tilefilename(currenttile));
    QSize targetSize(100, 100);
    //az éppen szerkesztett csempét jeleníti meg
    p = p.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->preview->setPixmap(p);
    //A csempe színét, típusát és csatlakozásait beállítja a megfelelő comboboxokban
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
    //Ha a csempe forrás vagy fogyasztó, akkor a színét beállítja a megfelelő comboboxban,
    //ha csap vagy sima típusú csempe, akkor a szín combobox nem szerkeszthető
    if (currenttile.getType() == SINK || currenttile.getType() == SOURCE) {
        ui->tileColorBox->setEnabled(true);
    } else {
        ui->tileColorBox->setEnabled(false);
    }
}

//Ez a függvény frissíti a készletet
void MainWindow::updateStock() {
    //az előző készletet törli
    ui->stock->clearContents();
    //az új készletet beállítja
    QMap<Tile, int> m = currentstock.enumStock();
    //az oszlopok száma a készletben lévő csempék száma
    // a sorok száma 2, a csempe típusa, a másik a száma
    ui->stock->setColumnCount(m.size());
    ui->stock->setRowCount(2);
    int column = 0;
    //végigmegy a készleten és megjeleníti a csempék képét és számát
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

//A fázisok tábláját frissíti
void MainWindow::updatePhases() {
    ui->phasesWidget->clearContents();
    //A sorok a fázisok száma
    ui->phasesWidget->setRowCount(phases.size());
    //Az oszlopok száma egy maximum keresés, a színek száma szerint a fázisban
    int max = 0;
    for (const Phase &phase: phases) {
        if (phase.getActiveColors().size() > max) {
            max = phase.getActiveColors().size();
        }
    }
    ui->phasesWidget->setColumnCount(max);
    for (int i = 0; i < phases.size(); ++i) {
        Phase phase = phases[i];
        //kivesszük egy fázis színeit a colors változóba
        const QSet<TileColor> &colors = phase.getActiveColors();
        auto it = colors.begin();
        //bejárjuk a színeket
        for (int j = 0; j < colors.size(); ++j) {
            const TileColor &color = *it; //ez az adott szín
            it++;
            //A színt magyar névvé alakítja
            QTableWidgetItem *item = new QTableWidgetItem(tileColorToName(color));
            //Beállítja a cella háttérszínét a színnek megfelelően
            item->setBackground(QBrush(tileColorToColor(color), Qt::SolidPattern));
            item->setData(Qt::UserRole, color);
            ui->phasesWidget->setItem(i, j, item);
        }
    }
}

//A csőrendszer frissítése
void MainWindow::updateGrid() {
    //A csőrendszer méretét beállítja a gird méretének megfelelően a SpinBoxokba
    ui->width->setValue(currentPipes->getGrid().getWidth());
    ui->height->setValue(currentPipes->getGrid().getHeight());
    //A méretet állítja az x és y pozíciók maximum értékét a grid méretének megfelelően
    ui->xpos->setMaximum(currentPipes->getGrid().getWidth());
    ui->ypos->setMaximum(currentPipes->getGrid().getHeight());
    //A csőrendszer méretét beállítja a grid méretének megfelelően
    ui->pipeline->setRowCount(currentPipes->getGrid().getHeight());
    ui->pipeline->setColumnCount(currentPipes->getGrid().getWidth());

    ui->pipeline->setShowGrid(false);
    const Grid &grid = currentPipes->getGrid();

    //bejárjuk a csőrendszert srofolytonosan
    for (int i = 0; i < grid.getHeight(); ++i) {
        for (int j = 0; j < grid.getWidth(); ++j) {
            //baállítjuk az aktuális és a másik szinten lévő pozíciókat és a csempéket
            GridPosition currentPosition = GridPosition(grid, 0, j, i);
            GridPosition currentPositionOther = GridPosition(grid, 1, j, i);
            Tile current = (*currentPipes)[currentPosition];
            Tile other = (*currentPipes)[currentPositionOther];
            QString filename;
            //Ha a csempe kanyar és a másik is egy kanyar akkor a megfelelő duplakanyar képet fogja visszaadni
            if (current.isStackableCorner() && other.isStackableCorner()) {
                if (current.getConnections() == 3 || current.getConnections() == 12) {
                    filename = "pics/DOUBLE1.png";
                }
                if (current.getConnections() == 6 || current.getConnections() == 9) {
                    filename = "pics/DOUBLE2.png";
                }
                //Ha mindkettő PostIt akkor a 0.png-t fogja visszaadni
            } else if (current.isPostIt() && other.isPostIt()) {
                filename = "pics/0.png";
                //Ha csak az egyik csempe PostIt akkor a másik csempe képét fogja visszaadni
            } else if (current.isPostIt()) {
                filename = tilefilename(other);
            } else {
                filename = tilefilename(current);
            }

            //A képet beállítjuk a megfelelő méretre és a megfelelő pozícióba
            QPixmap pixmap(filename);
            pixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DecorationRole, pixmap);
            QColor color = QColor(200, 200, 200);

            //Ha elindítjuk a folyamot és létezik akkor a PipeLine táblázat celláinak a háttérszínét beállítja a folyam színének megfelelően
            if (flow != nullptr) {
                TileColor currentColor = flow->getTileColorAt(currentPosition);
                if (currentColor == NONE) {
                    currentColor = flow->getTileColorAt(currentPositionOther);
                }
                if (currentColor != NONE) {
                    color = tileColorToColor(currentColor);
                }
            }
            item->setBackground(QBrush(color, Qt::SolidPattern));
            ui->pipeline->setColumnWidth(j, 90);
            ui->pipeline->setRowHeight(i, 90);
            ui->pipeline->setItem(i, j, item);

        }
    }

}

//A forgatásért felelős gomb, a Tile osztály rotate függvényét hívja meg
//ami az óramutató járásával megegyező irányba forgatja a csempét 1-szer
void MainWindow::on_rotate_clicked() {
    currenttile = currenttile.rotate(1);
    updateCurrentPipe();
}

//Ez a hozzáadás a készlethez gomb
void MainWindow::on_addtostock_clicked() {
    //PostIt nem adható a készlethez
    if (currenttile.getConnections() == 0) {
        return;
    }
    //Ha nem adunk a forrásnak vagy fogyasztónak színt, akkor nem adható a készlethez
    //A defaultként beállított színnel fogja hozzáadni a csempét (jelen esetben a cián)
    if (currenttile.getType() == SOURCE || currenttile.getType() == SINK) {
        if (currenttile.getColor() == NONE) {
            return;
        }
    }
    //A szelep nem lehet zsákutca, tehát a következő connectionokkal nem adható a készlethez
    if (currenttile.getType() == VALVE &&
        (currenttile.getConnections() == 0 || currenttile.getConnections() == 1 || currenttile.getConnections() == 2 ||
         currenttile.getConnections() == 4 || currenttile.getConnections() == 8)) {
        return;
    }
    //Különben hozzáadja a csempét a készlethez
    currentstock = currentstock.addTile(currenttile, 1);
    updateStock();

}

//Kitölti a felhasznélói felületen használt comboboxokat, egy szöveggel és hozzákapcsolja az adott szöveghez tartozó adatokat
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

//Az alak comboboxban kiválasztott csempe lesz az éppen szerkesztethető csempe
void MainWindow::on_tileConnectionBox_currentIndexChanged(int index) {
    //az comboboxban kiválasztott adat alapján beállítható a csempe connectionje
    //ugyani a fillComboBoxes függvényben hozzákapcsoltuk az adatokat a szövegekhez
    currenttile.setConnections(ui->tileConnectionBox->itemData(index).toInt());

    updateCurrentPipe();
}

//A típus comboboxban kiválasztott csempe lesz az aktuális csempe típusa
void MainWindow::on_tileTypeBox_currentIndexChanged(int index) {
    //az comboboxban kiválasztott adat alapján beállítható a csempe típusa
    currenttile.setType((TileType) ui->tileTypeBox->itemData(index).toInt());

    updateCurrentPipe();
}

//A szín comboboxban kiválasztott szín lesz az aktuális csempe színe
void MainWindow::on_tileColorBox_currentIndexChanged(int index) {
    //az comboboxban kiválasztott adat alapján beállítható a csempe színe
    currenttile.setColor((TileColor) ui->tileColorBox->itemData(index).toInt());
    updateCurrentPipe();
}

//Ezzel a gombbal tudjuk hozzáadni a csempét a játéktáblához
void MainWindow::on_addtogrid_clicked() {
    //Az x és y pozíciók alapján beállítja a csempe pozícióját
    int x_pos = ui->xpos->value() - 1;
    int y_pos = ui->ypos->value() - 1;
    //Ha nem "értelmes" pozíciót adunk meg, akkor nem tudjuk hozzáadni a csempét a játéktérhez
    if (x_pos < 0 || y_pos < 0 || x_pos >= currentPipes->getGrid().getWidth() ||
        y_pos >= currentPipes->getGrid().getHeight()) {
        return;
    }
    //Kitöröljük a módosítások előtti állapotot
    clearRollBackPipeLine();

    //Itt tudunk két kanyart egymásra rakni
    GridPosition pos = GridPosition(currentPipes->getGrid(), 0, x_pos, y_pos);
    //Ha az éppen lerakandó csempe egy kanyar és a 0. szinten már van egy kanyar, akkor...
    if (currenttile.isCorner() && (*currentPipes)[pos].isCorner()) {
        GridPosition pos_other = GridPosition(currentPipes->getGrid(), 1, x_pos, y_pos);
        //Ha a csatlakozások is engedik (ezt mondja meg a canPut függvény, az 1. szintre le tudjuk tenni a komplementer kanyart
        if (currentPipes->canPut(pos_other, currenttile)) {
            currentPipes->put(pos_other, currenttile);
            updateGrid();
            return;
        }
    }
    //Ha a csempe lerakható a megadott pozícióra, akkor lerakjuk és töröljük a másik szinten lévő csempét
    if (currentPipes->canPut(pos, currenttile)) {
        currentPipes->put(pos, currenttile);
        currentPipes->clear(pos.step(OTHER_STACK));
        updateGrid();
    }

}

//Ezzel a gombbal tudjuk kitörölni az adott csempét a készletből
void MainWindow::on_delfromstock_clicked() {

    currentstock = currentstock - currenttile;
    updateStock();
}

//Ezzel a gombbal tudjuk törölni a csempét a játéktérről
void MainWindow::on_delfromgrid_clicked() {
    int x_pos = ui->xpos->value() - 1;
    int y_pos = ui->ypos->value() - 1;
    if (x_pos < 0 || y_pos < 0 || x_pos >= currentPipes->getGrid().getWidth() ||
        y_pos >= currentPipes->getGrid().getHeight()) {
        return;
    }
    clearRollBackPipeLine();
    GridPosition pos = GridPosition(currentPipes->getGrid(), 0, x_pos, y_pos);
    GridPosition pos_other = GridPosition(currentPipes->getGrid(), 1, x_pos, y_pos);

    //Elpször az 1. szintről töröljük a csempét, ha az nem PostIt
    if ((*currentPipes)[pos_other] != PostIt) {
        //currenttile = currentPipes[pos_other];
        //updateCurrentPipe(); //beálltíthatü vele, hogy a törölt elem legyen a currenttile
        currentPipes->clear(pos_other);
        updateGrid();
        return;
    }
    //Aztán az 0. szintről töröljük a csempét, ha az nem PostIt
    if ((*currentPipes)[pos] != PostIt) {
        //currenttile = currentPipes[pos];
        //updateCurrentPipe(); //beálltíthatü vele, hogy a törölt elem legyen a currenttile
        currentPipes->clear(pos);
    }
    updateGrid();

}

//Fájlba való kiírás
void MainWindow::on_actionMent_s_triggered() {
    //A fájl kiterjesztése ".cst" lesz
    QString mentes = "CsatornaFajl (*.cst)";
    //A dialog ablak a working directory-t fogja default megnyitni
    QFileDialog dialog(this, "Mentés", "./",
                       mentes);
    //ha nem állítunk be kiterjesztést, akkor a ".cst" lesz a default, igazából csak kényelmi funkció
    dialog.setDefaultSuffix(".cst");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    //Ha cancel-t nyomunk, akkor nem történik semmi
    if (!(dialog.exec())) {
        return;
    }
    //Felül tudjuk írni a kiválasztott nevű fájlt.
    QString fileName = dialog.selectedFiles().front();
    QFile file(fileName);
    //Ha a fájlt nem tudjuk megnyitni, akkor nem történik semmi
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    //A toString függvények segítségével (lásd.: Stock.h, Phase.h, PipeLine.h) a jelenlegi állapotot kiírjuk a fájlba
    QTextStream out(&file);
    out << currentstock.toQString();
    out << "[Phases]\n";
    for (const Phase &phase: phases) {
        out << phase.toQString() << "\n";
    }
    out << "[Pipes]\n";
    QStringList pipel = currentPipes->toQString(false).split('\n');
    for (const QString &line: pipel) {
        //Az olvashatóság érdekében a csőrendszerv sorainak végére egy '`' karaktert teszünk
        out << line << '`' << "\n";
    }
}

//A fájlból való betöltés függvénye
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
        //Beolvasunk egy sort
        line = in.readLine();
        //A [Phases] sorig tartalmazza a fájl a készletet (fálj minta: signs.md)
        if (line == "[Phases]") {
            break;
        }
        //A sorokat a stockList változóba tesszük
        stocksList.push_back(line);
    }
    //a Stock::fromString "értelmezi" a készletet
    currentstock = Stock::fromString(stocksList.join("\n"));
    phases.clear();
    while (!in.atEnd()) {
        line = in.readLine();
        //A [Pipes] sorig tartalmazza a fájl a fázisokat
        if (line == "[Pipes]") {
            break;
        }
        Phase phase = Phase::fromString(line);
        phases.push_back(phase);
    }
    QStringList pipesList;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.endsWith('`')) {
            //Levesszük a sor végéről a '`' karaktert, ugyanis az nem szerves része a csőrendszernek
            //csak a fájl olvashatósága miatt szerepel
            line = line.mid(0, line.size() - 1);
        }
        if (line.size() % 5 != 0) {
            //Egy csempét mindig 5 karakter reprezentál, ha az adott sor nem osztható 5-tel, tehát valamelyik csempe hiányos
            //akkor a hiányzó karaktereket szóközzel pótoljuk, ugyanis a readLine a sor végről levágja a szóközöket
            line += QString(5 - line.size() % 5, ' ');
        }
        pipesList.push_back(line);
    }
    clearRollBackPipeLine();
    deletePipelineElements();
    //A PipeLine::fromString értelmezi a csőrendszert
    currentPipes = new PipeLine(PipeLine::fromString(pipesList.join("\n")));
    updateStock();
    updateGrid();
    updatePhases();
}

//A csőrendszer visszaállítja az üres állapotra
void MainWindow::deletePipelineElements() {
    delete currentPipes;
    if (flow != nullptr) {
        delete flow;
        flow = nullptr;
    }
}

//A játéktér szélességének változását kezeli
void MainWindow::on_width_valueChanged(int arg1) {
    PipeLine copy = currentPipes->resizeGrid(arg1, currentPipes->getGrid().getHeight());
    revertToRollBackPipeLine();
    currentPipes = new PipeLine(copy);
    updateGrid();
}


void MainWindow::on_height_valueChanged(int arg1) {
    PipeLine copy = currentPipes->resizeGrid(currentPipes->getGrid().getWidth(), arg1);
    //A méret megváltozik, új csőrendszert jön létre, amibe visszatöltjük az előzőnek a tartalmát
    revertToRollBackPipeLine();
    currentPipes = new PipeLine(copy);
    updateGrid();
}

//Ha rákattintunk a csőrendszer egy cellájára, akkor az x és y pozíciók a cella pozíciójára változnak,
//így nem csak a spinBoxokkal módosítható a játéktér
void MainWindow::on_pipeline_cellClicked(int row, int column) {
    ui->xpos->setValue(column + 1);
    ui->ypos->setValue(row + 1);
}

//Egy szín QColor-ját tudja beállítani, ezt használjuk a fázisok háttérszíneinek beállítására
//illetve a csőrendszeren a cellák háttérszínének beállítására a fázisnak megfelelően
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

//A fázisban törölhető vele egy szín
void MainWindow::on_deleteColor_clicked() {
    //Kiválasztjuk azt a cellát, amelyikben a amelyikre rákatintottunk
    int column = ui->phasesWidget->currentColumn();
    int row = ui->phasesWidget->currentRow();

    //Ha egyiken sem állunk nem történik semmi
    if (column == -1 || row == -1) {
        return;
    }

    //Kitöröljük a kiválasztott színt a táblázatból és az adatok közül is
    Phase phase = phases[row];
    TileColor currentColor = (TileColor) ui->phasesWidget->item(row, column)->data(Qt::UserRole).toInt();
    QSet<TileColor> colors = phase.getActiveColors();
    colors.remove(currentColor);
    phases[row] = Phase(colors);
    updatePhases();
}

//Hozzáad egy színt a fázishoz
void MainWindow::on_addColor_clicked() {
    //Mivel a fázisban a színek sorrendje mindegy, ezért elég csak kiválasztani, hogy
    //melyik fázishoz akarunk hozzáadni a színt

    int row = ui->phasesWidget->currentRow();
    //HA egyiken sem állunk, akkor a 1. fázishoz adja hozzá a színt,
    // ha nincs fázis, akkor létrehoz egyet és ahhoz adja hozzá a színt
    if (row == -1) {
        row = 0;
    }

    //A fázis adattagjához is hozzáadjuk a kiválasztott színt
    TileColor color = (TileColor) ui->chooseColor->itemData(ui->chooseColor->currentIndex()).toInt();
    if (phases.isEmpty()) {
        phases.push_back(Phase(QSet<TileColor>{color}));
        updatePhases();
        return;
    }
    QSet<TileColor> colors = phases[row].getActiveColors();
    colors.insert(color);
    phases[row] = Phase(colors);
    updatePhases();
}

//Kitörölhetjük vele azt a fázist, amelyikre sorában állunk
void MainWindow::on_deletePhase_clicked() {

    int row = ui->phasesWidget->currentRow();

    if (row == -1) {
        return;
    }
    phases.removeAt(row);
    updatePhases();
}

//Gomb a fázis hozzáadásához, mindig abba a sorba kerül a fázis, amelyikre rákattintottunk, ha nem állunk semmin, akkor a végére
void MainWindow::on_addPhase_clicked() {
    int row = ui->phasesWidget->currentRow();

    if (row == -1) {
        phases.push_back(Phase());
    } else {
        phases.insert(row, Phase());
    }
    updatePhases();
}

//Új játékot hozunk létre, a memóriából is kitörli az előző játékban megjegyezett adatokat
void MainWindow::on_action_j_2_triggered() {
    phases.clear();
    currentstock = Stock();
    deletePipelineElements();
    currentPipes = new PipeLine(Grid(3, 3));
    updateStock();
    updateGrid();
    updatePhases();

}

//Kilépés a programból
void MainWindow::on_actionKil_p_s_triggered() {
    exit(0);
}

//A kirakásért felelős gomb
void MainWindow::on_start_clicked() {

    if (solverIsRunning) {
        return;
    }
    solverIsRunning = true;
    //A kirakás előtt elmentjük a jelenlegi állapotot, hogy vissza tudjunk térni rá
    revertToRollBackPipeLine();
    originalPipes = new PipeLine(*currentPipes);
    //Amíg folyik a kirakás megjelenik egy progress bar
    QProgressDialog progressBar("Csővezeték építése", "Mégse", 0, 100, this);
    progressBar.setWindowModality(Qt::WindowModal);
    progressBar.show();

    //Létrehozunk a Buildernek egy Validator-t, amely a WindowedFlowValidator lesz (mert az tud ProgressBar-t csinálni)
    const PipeLineValidator &validator = WindowedFlowValidator(phases, progressBar);
    PipeLineBuilder builder = PipeLineBuilder(validator, *currentPipes);
    builder.resetBuild();
    QCoreApplication::processEvents();
    if (builder.build(currentstock) != INVALID) {
        //Ha kirakás befejeződött vagy megszakítódott, akkor a progress bar bezáródik
        //és bekerül az adott állapot a játéktérbe
        updateGrid();
    } else {
        //Ha nem lehet kirakni a csőrendszert akkor hibaüzenetet kapunk
        QMessageBox::warning(this, "Hiba", "A csővezeték kirakása nem sikerült");
    }
    progressBar.close();
    solverIsRunning = false;
}

//A stock táblázatban kattintásra az adott csempe lesz az éppen szerkesztett csempe
void MainWindow::on_stock_cellClicked(int row, int column) {
    if (column == -1) {
        return;
    }
    Tile key = currentstock.enumStock().keys().at(column);
    currenttile = key;
    updateCurrentPipe();
}

//A fázis táblázatban dupla kattintással az abban a fázisban lévő színekkel megjelenik
//a folyam a játékteren
void MainWindow::on_phasesWidget_cellDoubleClicked(int row, int column) {
    if (row == -1 || column == -1) {
        return;
    }
    if (flow != nullptr) {
        delete flow;
    }
    flow = new Flow();
    flow->makeFlow(*currentPipes, phases[row]);
    updateGrid();
}

//Az indítás gomb előtti csővezetéket állítja vissza
void MainWindow::on_actionVissza_ll_t_s_triggered() {
    revertToRollBackPipeLine();
}

//A feladat megfogalmazása szerinti optimális megoldást keresi meg
void MainWindow::on_optimal_clicked() {
    if (solverIsRunning) {
        return;
    }
    solverIsRunning = true;
    revertToRollBackPipeLine();
    originalPipes = new PipeLine(*currentPipes);
    QList<std::shared_ptr<QPair<PipeLine, int>>> bestPipes;

    QProgressDialog progressBar("Csővezeték építése", "Mégse", 0, 100, this);
    progressBar.setWindowModality(Qt::WindowModal);
    progressBar.show();

    const PipeLineValidator &validator = WindowedFlowValidator(phases, progressBar);
    PipeLineBuilder builder = PipeLineBuilder(validator, *currentPipes);
    builder.resetBuild();
    QCoreApplication::processEvents();
    //Eddig kb. ugyanaz mint az indítás
    while (true) {
        const ValidationResult &vrWithCoolness = builder.build(currentstock);
        if (vrWithCoolness.getType() == VR_VALID) {
            //A jó csőrendszereket elmentjük egy listába, hogy később megkeressük a legnagyobb coolness értékűt
            bestPipes.push_back(std::make_shared<QPair<PipeLine, int>>(
                    QPair<PipeLine, int>(PipeLine(*currentPipes), vrWithCoolness.getRateCoolness())));
        } else {
            //Itt is ki lehet lépni a Mégs gombbal, megjelenik a próbálokzás állapota a játéktéren
            if (vrWithCoolness == BREAK) {
                solverIsRunning = false;
                progressBar.close();
                updateGrid();
                return;
            }
            break;
        }
    }
    if (bestPipes.empty()) {
        QMessageBox::warning(this, "Hiba", "A csővezeték kirakása nem sikerült");
    }

    //Maximum kereséssel megkeressük a legnagyobb coolness értékű csőrendszert
    int bestCoolness = -1;
    int bestPipe = -1;
    for (int i = 0; i < bestPipes.size(); ++i) {
        if (bestPipes[i]->second > bestCoolness) {
            bestCoolness = bestPipes[i]->second;
            bestPipe = i;
        }
    }
    //Ha talál ilyen csőrendszert, akkor azt megjeleníti a játéktéren
    if (bestPipe != -1) {
        deletePipelineElements();
        currentPipes = new PipeLine(bestPipes[bestPipe]->first);
        updateGrid();
    }
    progressBar.close();
    solverIsRunning = false;
}

void MainWindow::clearRollBackPipeLine() {
    if (originalPipes != nullptr) {
        delete originalPipes;
        originalPipes = nullptr;
    }
}

void MainWindow::revertToRollBackPipeLine() {
    if (originalPipes == nullptr) {
        return;
    }
    deletePipelineElements();
    currentPipes = new PipeLine(*originalPipes);
    updateGrid();
}

