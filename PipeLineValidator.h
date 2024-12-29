//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEVALIDATOR_H
#define BEADANDO_II_PIPELINEVALIDATOR_H
#include "PipeLine.h"



class PipeLineValidator {
public:
    virtual bool validate(const PipeLine &pipeLine);

};


#endif //BEADANDO_II_PIPELINEVALIDATOR_H
