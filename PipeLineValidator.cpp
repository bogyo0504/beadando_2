//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include <QCoreApplication>
#include "PipeLineValidator.h"

ValidationResult PipeLineValidator::validate(const PipeLine &pipeLine) const {
    return INVALID;
}

FlowValidator::FlowValidator(const QList<Phase> &phases) : phases(phases) {}

ValidationResult FlowValidator::validate(const PipeLine &pipeLine) const {
    if (debugging) {
        std::cout << "Validating pipeline\n" << pipeLine.toQString(true).toStdString() << std::endl;
    }
    for (const Phase &phase: phases) {
        Flow flow;
        if (!flow.makeFlow(pipeLine, phase)) {
            return INVALID;
        }
    }
    return VALID;
}

void FlowValidator::debug() {
    debugging = true;
}

WindowedFlowValidator::WindowedFlowValidator(const QList<Phase> &phases, QProgressDialog &progressDialog)
        : FlowValidator(phases), progressDialog(progressDialog) {
    
}

ValidationResult WindowedFlowValidator::validate(const PipeLine &pipeLine) const {
    ValidationResult result = FlowValidator::validate(pipeLine);
    QCoreApplication::processEvents();
    if(progressDialog.wasCanceled()){
        return BREAK;
    }
    return result;
}
