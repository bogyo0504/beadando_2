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
    bool debugging = false;
    GridPosition debugPosition = INVALID_POSITION;
public:
    PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline);
    ValidationResult build(const Stock &stock);

    BuildState buildPipeLine(BuildState state);

    void debug();

    void printPosition(GridPosition position);

    void resetBuild();
};


#endif //BEADANDO_II_PIPELINEBUILDER_H
