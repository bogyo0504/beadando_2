//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEBUILDER_H
#define BEADANDO_II_PIPELINEBUILDER_H
#include "PipeLineValidator.h"

class PipeLineBuilder {
    const PipeLineValidator &validator;
    PipeLine &pipeline;
    int progressIntervalMin = 0;
    int progressIntervalMax = 1024;
    int progressValue = 0;
    int progressLimit = 512;
    bool inProgress = false;
    bool debugging = false;
    GridPosition debugPosition = INVALID_POSITION;
    QStack<std::shared_ptr<BuildState>> buildStateStack;
    BuildState buildPipeLine(BuildState state);
    QPair<bool, BuildState> stepBack();
public:
    PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline);

    ValidationResult build(const Stock &stock);

    void debug();

    void printPosition(GridPosition position);

    void resetBuild();

};


#endif //BEADANDO_II_PIPELINEBUILDER_H
