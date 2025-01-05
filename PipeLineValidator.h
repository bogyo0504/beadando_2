//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEVALIDATOR_H
#define BEADANDO_II_PIPELINEVALIDATOR_H

#include <QProgressDialog>
#include "PipeLine.h"
#include "Phase.h"
#include "Flow.h"

enum ValidationResult{
    INVALID, VALID, BREAK
};

class PipeLineValidator {
public:
    virtual ValidationResult validate(const PipeLine &pipeLine, int progress) const;

};


class FlowValidator : public PipeLineValidator {
    QList<Phase> phases;
    bool debugging = false;
public:
    explicit FlowValidator(const QList<Phase> &phases);

    ValidationResult validate(const PipeLine &pipeLine, int progress) const override ;

    void debug();
};

class WindowedFlowValidator : public FlowValidator {
    QProgressDialog &progressDialog;
public:
    explicit WindowedFlowValidator(const QList<Phase> &phases, QProgressDialog &progressDialog);

    ValidationResult validate(const PipeLine &pipeLine, int progress) const override;
};

#endif //BEADANDO_II_PIPELINEVALIDATOR_H

