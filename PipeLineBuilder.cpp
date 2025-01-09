//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include "PipeLineBuilder.h"

PipeLineBuilder::PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline) : validator(validator),
                                                                                           pipeline(pipeline) {}

ValidationResult PipeLineBuilder::build(const Stock &stock) {
    //A true azt jelenti, hogy rögtön visszalépéssel kell kezdeni, nem kellf  felépíteni
    bool skipCurrentStep = false;
    if (!inProgress) {
        //Beállítjuk a progress bar értékeit
        progressIntervalMin = 0;
        progressIntervalMax = 65536;
        progressValue = 0;
        progressLimit = 32768;

    } else {
        skipCurrentStep = true;
    }
    inProgress = true;

    //Az adott állapot, amelyből indul az építkezés
    BuildState currentState = BuildState(GridPosition(pipeline.getGrid(), 0, 0, 0), stock, IN_PROGRESS, PostIt, 0);
    while (true) {
        if (!skipCurrentStep) {
            currentState = buildPipeLine(currentState);
            if (currentState.getStatus() == ERROR) {
                //Ha valami hiba történt, akkor INVALID-et adunk vissza
                return INVALID;
            }
            //A másik eset az, hogy nem sikerül felépíteni, mert elfogyott a készlet, tehát vissza kell lépni,
            //ebben az esetben nem teljes a csővezeték
            if (currentState.getStatus() == READY) {
                //Ha sikerült felépíteni egy csővezetéket, akkor léptetünk egyet a progress bar-on
                //A lent feltételek azért kellenek azért kellenek, hogy ne legyen túl nagy a progress bar értéke
                //különben túlcsordulás lehet (az int típus méretét meghaladná)
                if (progressValue < 1000000 && progressIntervalMax < 0x10000000 &&
                    progressIntervalMin > -(0x10000000)) {
                    progressValue++;
                    //Ez az átskálázás, az intervallum mérete a kétszeresére nő, a százalék megmarad
                    if (progressValue > progressLimit) {
                        progressValue = progressLimit;
                        double progressRate =
                                (double) (progressValue - progressIntervalMin) /
                                (progressIntervalMax - progressIntervalMin);
                        progressIntervalMax = progressIntervalMax * 2;

                        // (M-m)*r = p-m
                        // M*r-m*r = p-m
                        // M*r -p = m*r -m
                        // M*r -p = m*(r-1)
                        // m = (M*r -p) / (r-1)
                        //Itt kiszámoljuk az új minimum értékét az intervallumnak
                        progressIntervalMin = (int) (
                                (((double) progressIntervalMax) * progressRate - (double) progressValue) /
                                (progressRate - 1));
                        progressLimit = (progressIntervalMax - progressValue) / 2;
                    }
                }
                //Megnézzük, hogy a csővezeték megfelel-e a feltételeknek (headerben megfogalmazva)
                ValidationResult isValid = validator.validate(pipeline, ((progressValue - progressIntervalMin) * 100) /
                                                                        (progressIntervalMax - progressIntervalMin));
                if (isValid.getType() == VR_VALID) {
                    return isValid;
                }
                //Ez ahhoz kell, hogy a felhaszálói felületen a Dialog ablaknál működjön a Mégse gomb
                if (isValid == BREAK) {
                    return BREAK;
                }
            }
        }
        skipCurrentStep = false;

        //Ez a visszalépés része az algoritmusnak, a ciklus azért kell mert többször is vissza lehet lépni
        while (true) {
            //A stepBack kiveszi a stckből az állapotot és a pipLineb-ból is törli a Tile-t
            QPair<bool, BuildState> successAndCurrentState = stepBack();
            if (!successAndCurrentState.first) {
                //Ha nem tdjuk tovább építeni a vezetéket, de a folyam nem megfelelő, akkor INVALID-et adunk vissza
                return INVALID;
            }
            //Ez az eredeti lépés
            currentState = successAndCurrentState.second;
            //Ezzel az állapottal az építőt "rávesszük", hogy válassza a következő lehetőséget
            currentState = BuildState(currentState.getPosition(), currentState.getStock(), TRY_NEXT,
                                      currentState.getCurrentTile(), currentState.getRotation());
            //Megpróbáljuk a következő lehetőséget
            currentState = pipeline.applyBuildState(currentState, buildStateStack);
            //Ha sikerült, akkor kilépünk a ciklusból, ha szükséges akkor kiírjuk a debuging állapotot
            if (currentState.getStatus() != OUT_OF_STOCK) {
                if (debugging || debugPosition.covers(successAndCurrentState.second.getPosition())) {
                    if (currentState.getStatus() == IN_PROGRESS) {
                        std::cout << "Step back to "
                                  << successAndCurrentState.second.getPosition().toQString().toStdString() << std::endl;
                        std::cout << "Putting: " << currentState.getPosition().toQString().toStdString() << " "
                                  << currentState.getCurrentTile().getConnections()
                                  << " " << currentState.getCurrentTile().getType() << " " << currentState.getRotation()
                                  << std::endl;
                    } else {
                        std::cout << "Putting: " << currentState.getStatus();
                    }
                }
                break;
            }
            if (debugging) {
                std::cout << "Out of stock at " << successAndCurrentState.second.getPosition().toQString().toStdString()
                          << std::endl;
            }
        }
    }
    //Ha nem sikerül további visszalépések kellenek
    return INVALID;
}

BuildState PipeLineBuilder::buildPipeLine(BuildState state) {
    while (state.getStatus() == IN_PROGRESS || state.getStatus() == TRY_NEXT) {
        state = pipeline.applyBuildState(state, buildStateStack);
    }
    return state;
}

void PipeLineBuilder::debug() {
    debugging = true;
}

void PipeLineBuilder::printPosition(GridPosition position) {
    debugPosition = position;
}

void PipeLineBuilder::resetBuild() {
    inProgress = false;
    buildStateStack.clear();
    pipeline.removePostIts();
}

QPair<bool, BuildState> PipeLineBuilder::stepBack() {
    //Ha a stack teljesen üres és még mindig vissza kell lépni a programban
    if (buildStateStack.empty()) {
        return {false, BuildState(INVALID_POSITION, Stock(), ERROR, PostIt, 0)};
    }
    //Kiveszi a stackből az utolsó állapotot
    BuildState state = *buildStateStack.pop();
    //Törli a csempét a PipeLine-ból
    pipeline.clear(state.getPosition());
    return {true, state};
}
