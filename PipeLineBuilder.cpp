//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include "PipeLineBuilder.h"

PipeLineBuilder::PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline) : validator(validator),
                                                                                     pipeline(pipeline) {}

ValidationResult PipeLineBuilder::build(const Stock &stock) {
    BuildState currentState = BuildState(GridPosition(pipeline.getGrid(), 0, 0, 0), stock, IN_PROGRESS, PostIt, 0, 0);
    while (true) {
        currentState = buildPipeLine(currentState);
        if (currentState.getStatus() == ERROR) {
            return INVALID;
        }
        ValidationResult isValid = validator.validate(pipeline, pipeline.getAlternativeCount());
        if (isValid == VALID) {
            return VALID;
        }
        if(isValid == BREAK){
            return BREAK;
        }
        while (true) {
            QPair<bool, BuildState> successAndCurrentState = pipeline.stepBack();
            if (!successAndCurrentState.first) {
                return INVALID;
            }
            currentState = successAndCurrentState.second;
            currentState = BuildState(currentState.getPosition(), currentState.getStock(), TRY_NEXT,
                                      currentState.getCurrentTile(), currentState.getRotation(), currentState.getAlternatives());
            currentState = pipeline.addElementFromStock(currentState);
            if (currentState.getStatus() != OUT_OF_STOCK) {
                if (debugging || debugPosition.covers(successAndCurrentState.second.getPosition())) {
                    if (currentState.getStatus() == IN_PROGRESS) {
                        std::cout << "Step back to " << successAndCurrentState.second.getPosition().toQString().toStdString() << std::endl;
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
        state = pipeline.addElementFromStock(state);
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
    pipeline.resetBuildStates();
    pipeline.removePostIts();
}
