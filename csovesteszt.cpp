#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "Tile.h"
#include "Grid.h"
#include "GridPosition.h"
#include "Stock.h"
#include "PipeLine.h"
#include "Flow.h"
#include "PipeLineValidator.h"
#include "PipeLineBuilder.h"

TEST_CASE("Teszteljük a Tile funkcióit") {
    Tile t1 = Tile(3, TileType::NORMAL);
    Tile t2 = Tile(3, TileType::NORMAL);
    Tile t3 = Tile(5, TileType::NORMAL);
    CHECK(t1 != PostIt);
    CHECK(t1 == t2);
    CHECK(t1.isCorner());
    CHECK(!t1.isPostIt());
    CHECK(t1.getConnections() == 3);
    CHECK(t1.getType() == TileType::NORMAL);
    CHECK(!t3.isCorner());
    CHECK(t3 > t1);
    CHECK(t1.rotate(1).getConnections() == 9);
    CHECK(t1.rotate(2).getConnections() == 12);
    CHECK(t1.rotate(3).getConnections() == 6);
    CHECK(t3.rotate(1).getConnections() == 10);
    CHECK(t3.rotate(2).getConnections() == 5);
    CHECK(t3.rotate(3).getConnections() == 10);
}

TEST_CASE("Teszteljük a GridPosition funkcióit") {
    Grid grid = Grid(3, 2);
    GridPosition gp1 = GridPosition(grid, 1, 1, 1);
    GridPosition gp2 = GridPosition(grid, 1, 1, 1);
    CHECK(gp1 == gp2);
    GridPosition gp3 = ++gp1;
    CHECK(gp3 != gp2);
    CHECK(gp3 > gp2);
    CHECK(gp3.getHorizontal() == 2);
    CHECK(gp3.getStack() == 0);
    CHECK(gp3.getVertical() == 1);
    GridPosition gp4 = gp1.step(GridPositionStep::RIGHT);
    CHECK(gp4.getHorizontal() == 2);
    CHECK((gp4.getStack() == 1));
    CHECK(gp4.getVertical() == 1);
    GridPosition gp5 = gp1.step(GridPositionStep::UP);
    CHECK(gp5.getHorizontal() == 1);
    CHECK(gp5.getStack() == 1);
    CHECK(gp5.getVertical() == 0);
    GridPosition gp6 = gp1.step(GridPositionStep::DOWN);
    CHECK(gp6 == INVALID_POSITION);
    GridPosition gp7 = gp1.step(GridPositionStep::OTHER_STACK);
    CHECK(gp7.getHorizontal() == 1);
    CHECK(gp7.getStack() == 0);
    CHECK(gp7.getVertical() == 1);
    CHECK(!gp1.isLast());
    CHECK(gp4.isLast());
}

TEST_CASE("Teszteljük a Stock funkcióit") {
    SUBCASE("Üres készletben nincs semmi, és nincs következő sem.") {
        Tile t1 = Tile(3, TileType::NORMAL);
        Stock stock = Stock();
        CHECK(!stock.contains(t1));
        CHECK(stock.getNextTile(t1) == PostIt);
    }
    SUBCASE("Egy elemű készletben az elem benne van, és nincs következő sem.") {
        Tile t1 = Tile(3, TileType::NORMAL);
        Stock stock = Stock();
        Stock stock1 = stock.addTile(t1, 2);
        CHECK(stock1.contains(t1));
        CHECK(stock1.getNextTile(t1) == PostIt);
    }
    SUBCASE("Két elemű készletben az elemek benne vannak, és a következő elem a másik elem.") {
        Tile t1 = Tile(3, TileType::NORMAL);
        Tile t2 = Tile(5, TileType::NORMAL);
        Stock stock = Stock();
        Stock stock1 = stock.addTile(t1, 2);
        Stock stock2 = stock1.addTile(t2, 2);
        CHECK(stock2.contains(t1));
        CHECK(stock2.contains(t2));
        CHECK(stock2.getNextTile(t1) == t2);
        CHECK(stock2.getNextTile(t2) == PostIt);
        Stock stock3 = stock2 - t1;
        CHECK(stock3.contains(t1));
        Stock stock4 = stock3 - t1;
        CHECK(!stock4.contains(t1));
    }
}

TEST_CASE("Teszteljük a PipeLine funkcióit") {
    Grid grid = Grid(3, 2);
    GridPosition gp1 = GridPosition(grid, 0, 1, 1);
    GridPosition gp2 = GridPosition(grid, 1, 1, 1);
    Tile t1 = Tile(3, TileType::NORMAL);
    Tile t2 = Tile(5, TileType::NORMAL);
    Tile t3 = Tile(6, TileType::NORMAL);
    Tile t4 = Tile(12, TileType::NORMAL);
    PipeLine pipeLine = PipeLine(grid);
    CHECK(pipeLine.isEmpty(gp1));
    CHECK(pipeLine[gp1] == PostIt);
    pipeLine.put(gp1, t1);
    CHECK(pipeLine[gp1] == t1);
    CHECK(!pipeLine.isEmpty(gp1));
    CHECK(!pipeLine.canPut(gp2, t2));
    CHECK(!pipeLine.canPut(gp2, t3));
    CHECK(pipeLine.canPut(gp2, t4));
    pipeLine.clear(gp1);
    CHECK(pipeLine.isEmpty(gp1));
}

TEST_CASE("Teszteljük a PipeLine addElementFromStock funkcióját") {
    Grid grid = Grid(2, 1);
    Stock stock = Stock();
    Tile t1 = Tile(3, TileType::NORMAL);
    Tile t2 = Tile(5, TileType::NORMAL);
    Tile t3 = Tile(12, TileType::NORMAL);
    stock = stock.addTile(t1, 1);
    stock = stock.addTile(t2, 1);
    stock = stock.addTile(t3, 1);
    GridPosition gp1 = GridPosition(grid, 0, 0, 0);
    PipeLine pipeLine = PipeLine(grid);
    BuildState state = BuildState(gp1, stock, IN_PROGRESS, t1, 0);
    BuildState state1 = pipeLine.addElementFromStock(state);
    CHECK(state1.getPosition().getStack() == 1);
    CHECK(state1.getCurrentTile() == PostIt);
    CHECK(state1.getStatus() != READY);
    // Kihagyjuk a PostIt behelyezést
    BuildState state2 = BuildState(state1.getPosition(), state1.getStock(), IN_PROGRESS, t1, 0);
    BuildState state3 = pipeLine.addElementFromStock(state2);
    CHECK(state3.getPosition().getStack() == 1);
    CHECK(state3.getCurrentTile() == t2);
    CHECK(state3.getRotation() == 0);
    CHECK(state3.getStatus() != READY);
    BuildState state4 = pipeLine.addElementFromStock(state3);
    CHECK(state4.getPosition().getStack() == 1);
    CHECK(state4.getCurrentTile() == t2);
    CHECK(state4.getRotation() == 1);
    CHECK(state4.getStatus() != READY);/*
    BuildState state4a = BuildState(state4.getPosition(), state4.getStock(), IN_PROGRESS, t3,3);
    BuildState state5 = pipeLine.addElementFromStock(state4a);
    CHECK(state5.getPosition().getStack() == 0);
    CHECK(state5.getCurrentTile() == PostIt);
    CHECK(state5.getStatus() != READY);
    CHECK(state5.getPosition().getHorizontal()==1);
    CHECK(state5.getPosition().getVertical()==0);
    // Kihagyjuk a PostIt behelyezést
    BuildState state6 = BuildState(state5.getPosition(), state5.getStock(), IN_PROGRESS, t1,0);
    BuildState state7 = pipeLine.addElementFromStock(state6);
    CHECK(state7.getPosition().getStack() == 0);
    CHECK(state7.getCurrentTile() == t2);
    CHECK(state7.getStatus() != READY);
    BuildState state7a = BuildState(state7.getPosition(), state7.getStock(), IN_PROGRESS, t2,3);
    BuildState state8 = pipeLine.addElementFromStock(state7);
    CHECK(state8.getPosition().getStack() == 1);
    CHECK(state8.getCurrentTile() == PostIt);
    CHECK(state8.getStatus() != READY);
    BuildState state9 = pipeLine.addElementFromStock(state8);
    CHECK(state9.getPosition() == INVALID_POSITION);
    CHECK(state9.getCurrentTile() == PostIt);
    CHECK(state9.getStatus() != READY);
    BuildState state10 = pipeLine.addElementFromStock(state9);
    CHECK(state10.getPosition() == INVALID_POSITION);
    CHECK(state10.getCurrentTile() == PostIt);
    CHECK(state10.getStatus() == READY);

    CHECK(pipeLine[gp1] == t1);
    CHECK(pipeLine[GridPosition(grid, 1, 0, 0)] == t3);
    CHECK(pipeLine[GridPosition(grid, 0, 1, 0)] == t2);
    CHECK(pipeLine[GridPosition(grid, 1, 1, 0)] == PostIt);
*/

// 1. **Objektumok inicializálása**
//  - *Rács*: Egy 2x1 méretű rács jön létre.
//  - *Készlet*: Egy készlet inicializálódik, amelybe három csempe (t1, t2 és t3) kerül.
//  - *Rács pozíciója*: A kezdeti rácspozíció (0, 0, 0) értékre van állítva.
//  - *PipeLine*: Egy pipeline inicializálódik a rács segítségével.
//  - *BuildState*: A kezdeti állapot beállításra kerül a kezdeti rácspozícióval, készlettel, nem kész állapottal és az első csempével, t1-gyel.

//2. **Első hozzáadás**
// - Meghívásra kerül az `addElementFromStock` metódus, hogy hozzáadja az első csempét, t1-et.
// - El kell helyeznie t1-et a (0, 0, 0) pozícióban.
// - A pozíció stack értéke 1 lesz, és az aktuális csempe PostIt-re vált.

//3. **PostIt átugrása és a következő csempe hozzáadása**
//  - Egy új állapot jön létre, amely átugorja a PostIt-et, és az aktuális csempét visszaállítja t1-re.
//  - Megkapja a következő csempét, t2-t, és elhelyezi a (0, 0, 1) pozícióban.
//
//4. **t3 hozzáadása**
// - A következő csempe, t3 hozzáadása, amely a (0, 0, 1) pozícióba kerül.

// 5. **PostIt átugrása és a következő csempe hozzáadása**
// - Egy újabb PostIt kerül átugrásra, az aktuális csempe visszaállításával t1-re.

//6. **Csempe hozzáadása a következő pozícióba**
// - Megpróbálja a következő csempét a következő elérhető pozícióba helyezni.

//7. **Érvénytelen pozíciók és kész állapot**
// - A rács megtelik, így a pozíciók érvénytelenné válnak, és a BuildState készenléti állapotba kerül.

//8. **Végső pozíciók ellenőrzése**
// - Végül ellenőrzi a csempék pozícióit a pipeline-ban:
// - (0, 0, 0) pozícióban t1 található.
// - (1, 0, 0) pozícióban t3 található.
// - (0, 1, 0) pozícióban t2 található.
// - (1, 1, 0) pozícióban PostIt található.


}


TEST_CASE("Teszteljük a Pipeline to/from QString funkcióit") {
    PipeLine p1 = PipeLine::fromString(" .---------.   \n"
                                       " |         |   \n"
                                       " |         |   \n"
                                       " |         |   \n"
                                       " '---------'   \n");
    std::cout << p1.toQString().toStdString();
    CHECK(p1.toQString() == " .---------.   \n"
                            " |         |   \n"
                            " |         |   \n"
                            " |         |   \n"
                            " '---------'   \n");
    PipeLine p2 = PipeLine::fromString(
            " .-oB------.   \n"
            " |         |   \n"
            " |------0--|   \n"
            " |         |   \n"
            " '---------' cG\n");
    std::cout << p2.toQString().toStdString();

    CHECK(p2.toQString() == " .-oB------.   \n"
                            " |         |   \n"
                            " |------0--|   \n"
                            " |         |   \n"
                            " '---------' cG\n");

    PipeLine p3 = PipeLine::fromString(
            ""
            " .-oB---0--. oG\n"
            " | 0       |   \n"
            " |------0--|   \n"
            " |         |   \n"
            " '-cB---0--' cG\n");
    Flow f1;
    Phase ph1 = Phase({BLUE, GREEN});

    CHECK(f1.makeFlow(p3, ph1));

    std::cout << f1.toQString().toStdString();

}

TEST_CASE("Teszteljük a PipeLineBuilder funkciót 2x3-as tesztpéldán") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG          \n"
                                             "          -  cG\n");
    Stock stock = Stock::fromString("2 -'   \n");
    Phase phase = Phase({GREEN});
    FlowValidator validator({phase});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 3x3-as tesztpéldán 2") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG          \n"
                                             "               \n"
                                             "          -  cG\n");
    Stock stock = Stock::fromString(""
                                    "2 -'   \n"
                                    "1 ---  \n");
    Phase phase = Phase({GREEN});
    FlowValidator validator({phase});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 3x3-as tesztpéldán, szeleppel") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG          \n"
                                             "               \n"
                                             "          -  cG\n");
    std::cout << pipeline.toQString(true).toStdString();
    Stock stock = Stock::fromString(""
                                    "2 -'   \n"
                                    "1 ---0 \n");
    Phase phase = Phase(GREEN);
    FlowValidator validator({phase});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}

TEST_CASE("Teszteljük a PipeLineBuilder funkciót 4x2-as tesztpéldán, felesleges anyaggal") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG          -  cG\n"
                                             "                    \n");
    Stock stock = Stock::fromString(""
                                    "4 -'   \n"
                                    "1 ---  \n");
    Phase phase = Phase({GREEN});
    FlowValidator validator({phase});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Validáljuk 3x3-as tesztpéldát") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG-.----  cB\n"
                                             "      |        \n"
                                             "  -oB-'----  cG\n");
    Phase phaseGreen = Phase({GREEN});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    CHECK(validator.validate(pipeline));

}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 3x3-as tesztpéldán 1") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG     -  cB\n"
                                             "               \n"
                                             "  -oB     -  cG\n");
    Stock stock = Stock::fromString(""
                                    "2 -'   \n"
                                    "2 -'-  \n"
                                    "1 ---  \n");
    Phase phaseGreen = Phase({GREEN});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Validáljuk 3x3-as tesztpéldát, egyesített cellával") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG-.        \n"
                                             "  -cG-:----  oB\n"
                                             "      '----  cB\n");
    Phase phaseGreen = Phase({GREEN, BLUE});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    CHECK(validator.validate(pipeline));

}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 3x3-as tesztpéldán, egyesített cellákkal") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG-.        \n"
                                             "  -cG     -  oB\n"
                                             "      '----  cB\n");
    Stock stock = Stock::fromString(""
                                    "2  .-  \n");
    Phase phaseGreen = Phase({GREEN,BLUE});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    validator.debug();
    pipeLineBuilder.debug();
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 6x3-as tesztpéldán") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG                    -  cG\n"
                                             "                              \n"
                                             "  -oB                    -  cB\n");
    Stock stock = Stock::fromString(""
                                    "4 -'   \n"
                                    "2 -'-  \n"
                                    "4 ---0 \n");
    Phase phaseGreen = Phase({GREEN, BLUE});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}

TEST_CASE("Teszteljük a PipeLineBuilder funkciót 4x3-as tesztpéldán") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG          -  cB\n"
                                             "                    \n"
                                             "  -oB          -  cG\n");
    pipeline.disableOverlap();
    Stock stock = Stock::fromString(""
                                    "4 -'   \n"
                                    "2 -'-  \n");
    Phase phaseGreen = Phase({GREEN});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen
                             , phaseBlue
                             });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
   // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
  // pipeLineBuilder.debug();
  // validator.debug();
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 5x3-as tesztpéldán, csappal") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG               -  cG\n"
                                             "                         \n"
                                             "  -oB               -  cB\n");
    pipeline.disableOverlap();
    Stock stock = Stock::fromString(""
                                    "4 ---  \n"
                                    "3 -'-  \n"
                                    "1 -'   \n"
                                    "1 -'-0 \n");
    Phase phaseGreen = Phase({GREEN, BLUE});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen
                                    , phaseBlue
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
    // pipeLineBuilder.debug();
    // validator.debug();
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 5x5-as tesztpéldán, csappal") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "      . oB               \n"
                                             "  -oG          -  cB     \n"
                                             "                         \n"
                                             "     -.-oY          -  cG\n"
                                             "                ' cY     \n");
    pipeline.disableOverlap();
    Stock stock = Stock::fromString(""
                                    "1 ---0 \n"
                                    "12 -'   \n"
                                    "5 -'-  \n");
    Phase phaseGreen = Phase({GREEN});
    Phase phaseBlue = Phase({YELLOW, BLUE});
    FlowValidator validator({phaseGreen
                                    , phaseBlue
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
    // pipeLineBuilder.debug();
    // validator.debug();
    CHECK(pipeLineBuilder.build(stock));
    std::cout << pipeline.toQString(true).toStdString();
}
