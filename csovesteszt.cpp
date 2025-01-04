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
    CHECK(t3 < t1);
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
    CHECK(gp3.getStack() == 1);
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
        CHECK(stock2.getNextTile(t1) == PostIt);
        CHECK(stock2.getNextTile(t2) == t1);
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
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    CHECK(validator.validate(pipeline, 0));

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
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    CHECK(validator.validate(pipeline, 0));

}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 3x3-as tesztpéldán, egyesített cellákkal") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG-.        \n"
                                             "  -cG     -  oB\n"
                                             "      '----  cB\n");
    Stock stock = Stock::fromString(""
                                    "2  .-  \n");
    Phase phaseGreen = Phase({GREEN, BLUE});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    validator.debug();
    pipeLineBuilder.debug();
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    Phase phaseGreen = Phase({GREEN});
    Phase phaseBlue = Phase({BLUE});
    FlowValidator validator({phaseGreen, phaseBlue});
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    FlowValidator validator({phaseGreen, phaseBlue
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
    // pipeLineBuilder.debug();
    // validator.debug();
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    FlowValidator validator({phaseGreen, phaseBlue
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
    // pipeLineBuilder.debug();
    // validator.debug();
    CHECK(pipeLineBuilder.build(stock)==VALID);
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
    FlowValidator validator({phaseGreen, phaseBlue
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    // pipeLineBuilder.printPosition(GridPosition(pipeline.getGrid(), -1, 0, 1));
    // pipeLineBuilder.debug();
    // validator.debug();
    CHECK(pipeLineBuilder.build(stock)==VALID);
    std::cout << pipeline.toQString(true).toStdString();
}


TEST_CASE("Teszteljük a PipeLineBuilder funkciót 9x5-as tesztpéldán, csak átfedés") {
    PipeLine pipeline = PipeLine::fromString(""
                                             "  -oG-.----------------------------     -  cG\n"
                                             "            -oB     -----------  cB          \n"
                                             "  -oY                                   -  cY\n"
                                             "                                             \n"
                                             "  -oR     -------------------------     -  cR\n");
    Stock stock = Stock::fromString(""
                                    "5 -'-  \n"
                                    "6 ---  \n"
                                    "4 ---0 \n");
    Phase phaseGreen = Phase({GREEN, YELLOW, BLUE});
    Phase phaseBlue = Phase({BLUE, RED, YELLOW});
    Phase phaseYellow = Phase({GREEN, RED});
    FlowValidator validator({phaseGreen, phaseBlue, phaseYellow
                            });
    PipeLineBuilder pipeLineBuilder(validator, pipeline);
    CHECK(pipeLineBuilder.build(stock)==VALID);
    std::cout << pipeline.toQString(true).toStdString();
}

TEST_CASE("Teszteljük a Phase fromString és toString funkcióit"){
    Phase phase = Phase({RED, BLUE, YELLOW});
    CHECK(phase.toQString() == "RBY");
    Phase phase2 = Phase::fromString("RBY");
    CHECK(phase2.getActiveColors() == phase.getActiveColors());
}


