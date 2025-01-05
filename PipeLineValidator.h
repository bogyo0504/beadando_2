//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEVALIDATOR_H
#define BEADANDO_II_PIPELINEVALIDATOR_H

#include <QProgressDialog>
#include "PipeLine.h"
#include "Phase.h"
#include "Flow.h"


enum ValidationResultType{
    VR_INVALID, VR_VALID, VR_BREAK
};

class ValidationResult{
    const ValidationResultType type;
    const int rateCoolness;
public:
    bool operator==(const ValidationResult &rhs) const;

    bool operator!=(const ValidationResult &rhs) const;

    ValidationResult(ValidationResultType type, int rateCoolness);

    int getRateCoolness() const;

    ValidationResultType getType() const;
};

#define INVALID ValidationResult(VR_INVALID, 0)
#define BREAK ValidationResult(VR_BREAK, 0)

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

