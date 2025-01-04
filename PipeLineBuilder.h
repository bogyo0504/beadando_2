//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEBUILDER_H
#define BEADANDO_II_PIPELINEBUILDER_H
#include "PipeLineValidator.h"

class PipeLineBuilder {
    PipeLineValidator &validator;
    PipeLine &pipeline;
    bool debugging = false;
    GridPosition debugPosition = INVALID_POSITION;
public:
    PipeLineBuilder(PipeLineValidator &validator, PipeLine &pipeline);
    bool build(const Stock &stock);

    BuildState buildPipeLine(BuildState state);

    void debug();

    void printPosition(GridPosition position);

    void resetBuild();
};


#endif //BEADANDO_II_PIPELINEBUILDER_H
