//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEVALIDATOR_H
#define BEADANDO_II_PIPELINEVALIDATOR_H
#include "PipeLine.h"
#include "Phase.h"
#include "Flow.h"

class PipeLineValidator {
public:
    virtual bool validate(const PipeLine &pipeLine);

};


class FlowValidator : public PipeLineValidator {
    QList<Phase> phases;
public:
    explicit FlowValidator(const QList<Phase> &phases);

    bool validate(const PipeLine &pipeLine) override;
};
#endif //BEADANDO_II_PIPELINEVALIDATOR_H
