//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include <QCoreApplication>
#include "PipeLineValidator.h"

ValidationResult PipeLineValidator::validate(const PipeLine &pipeLine, long alternativeCount) const {
    return INVALID;
}

FlowValidator::FlowValidator(const QList<Phase> &phases) : phases(phases) {}

ValidationResult FlowValidator::validate(const PipeLine &pipeLine, long alternativeCount) const {
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

ValidationResult WindowedFlowValidator::validate(const PipeLine &pipeLine, long alternativeCount) const {
    ValidationResult result = FlowValidator::validate(pipeLine, 0);
    QCoreApplication::processEvents();
    if (progressDialog.maximum() == 0 && alternativeCount < 0x7FFFFFFF) {
        progressDialog.setMaximum((int) alternativeCount);
    }
    if (alternativeCount < 0x7FFFFFFF) {
        if (progressDialog.maximum() >= (int) alternativeCount) {
            progressDialog.setValue(progressDialog.maximum() - (int) alternativeCount);
        }
    }
    if(progressDialog.wasCanceled()){
        return BREAK;
    }
    return result;
}
