//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEVALIDATOR_H
#define BEADANDO_II_PIPELINEVALIDATOR_H

#include <QProgressDialog>
#include "PipeLine.h"
#include "Phase.h"
#include "Flow.h"

/**
 * A ValidationResultType az egy ValidationResult típusokat tartalmazó enum, megadja,
 * hogy milyen eredményt adott vissza a validátor.
 */
enum ValidationResultType{
    VR_INVALID, VR_VALID, VR_BREAK
};


class ValidationResult{

    /**
     * Megadja a validáció eredményét, a ValidationResultType enum értékei segítségével.
     * Az egyenlőség operátor a különböző esetek elkülönítéséhez kell a PipeLineBuilder-ben.
     */
    const ValidationResultType type;

    /**
     * Az adott folyam jóságát adja meg
     */
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

/**
 * A PipeLineValidator egy csőrendszert validáló osztály.
 * Ennek a leszármazottja a FlowValidator, amely a folyamot validálja (ez csinálja a valódi munkát).
 */
class PipeLineValidator {
public:
    virtual ValidationResult validate(const PipeLine &pipeLine, int progress) const;

};


/**
 * Fázisok és folyamok alpján validál.
 */
class FlowValidator : public PipeLineValidator {
    QList<Phase> phases;
    bool debugging = false;
public:
    explicit FlowValidator(const QList<Phase> &phases);

    ValidationResult validate(const PipeLine &pipeLine, int progress) const override ;
    void debug();
};

/**
 * A WindowedFlowValidator egy olyan FlowValidator leszármazott, amelyhez még kapcsolódik egy progress bar.
 */
class WindowedFlowValidator : public FlowValidator {
    QProgressDialog &progressDialog;
public:
    explicit WindowedFlowValidator(const QList<Phase> &phases, QProgressDialog &progressDialog);

    ValidationResult validate(const PipeLine &pipeLine, int progress) const override;
};

#endif //BEADANDO_II_PIPELINEVALIDATOR_H

