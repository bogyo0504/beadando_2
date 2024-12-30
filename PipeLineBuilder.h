//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEBUILDER_H
#define BEADANDO_II_PIPELINEBUILDER_H
#include "PipeLineValidator.h"

class PipeLineBuilder {
    PipeLineValidator &validator;
    PipeLine &pipeline;

public:
    PipeLineBuilder(PipeLineValidator &validator, PipeLine &pipeline);
    bool build(const Stock &stock);

    BuildState buildPipeLine(BuildState state);
};


#endif //BEADANDO_II_PIPELINEBUILDER_H
