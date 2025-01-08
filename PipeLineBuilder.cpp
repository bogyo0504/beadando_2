//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include "PipeLineBuilder.h"

PipeLineBuilder::PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline) : validator(validator),
                                                                                           pipeline(pipeline) {}

ValidationResult PipeLineBuilder::build(const Stock &stock) {
    bool skipCurrentStep = false;
    if (!inProgress) {
        progressIntervalMin = 0;
        progressIntervalMax = 65536;
        progressValue = 0;
        progressLimit = 32768;

    } else {
        skipCurrentStep = true;
    }
    inProgress = true;

    BuildState currentState = BuildState(GridPosition(pipeline.getGrid(), 0, 0, 0), stock, IN_PROGRESS, PostIt, 0);
    while (true) {
        if (!skipCurrentStep) {
            currentState = buildPipeLine(currentState);
            if (currentState.getStatus() == ERROR) {
                return INVALID;
            }
            if (progressValue < 1000000 && progressIntervalMax < 0x10000000 && progressIntervalMin > -(0x10000000)) {
                progressValue++;
                if (progressValue > progressLimit) {
                    progressValue = progressLimit;
                    double progressRate =
                            (double) (progressValue - progressIntervalMin) /
                            (progressIntervalMax - progressIntervalMin);
                    progressIntervalMax = progressIntervalMax * 2;

                    // (x-m)*r = p-m
                    // x*r-m*r = p-m
                    // x*r -p = m*r -m
                    // x*r -p = m*(r-1)
                    // m = (x*r -p) / (r-1)
                    progressIntervalMin = (int) (
                            (((double) progressIntervalMax) * progressRate - (double) progressValue) /
                            (progressRate - 1));
                    progressLimit = (progressIntervalMax - progressValue) / 2;
                }
            }

            ValidationResult isValid = validator.validate(pipeline, ((progressValue - progressIntervalMin) * 100) /
                                                                    (progressIntervalMax - progressIntervalMin));
            if (isValid.getType() == VR_VALID) {
                return isValid;
            }
            if (isValid == BREAK) {
                return BREAK;
            }
        }
        skipCurrentStep = false;
        while (true) {
            QPair<bool, BuildState> successAndCurrentState = stepBack();
            if (!successAndCurrentState.first) {
                return INVALID;
            }
            currentState = successAndCurrentState.second;
            currentState = BuildState(currentState.getPosition(), currentState.getStock(), TRY_NEXT,
                                      currentState.getCurrentTile(), currentState.getRotation());
            currentState = pipeline.applyBuildState(currentState, buildStateStack);
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
    if (buildStateStack.empty()) {
        return {false, BuildState(INVALID_POSITION, Stock(), ERROR, PostIt, 0)};
    }
    BuildState state = *buildStateStack.pop();
    pipeline.clear(state.getPosition());
    return {true, state};
}
