//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include <QCoreApplication>
#include "PipeLineValidator.h"

ValidationResult PipeLineValidator::validate(const PipeLine &pipeLine, int progress) const {
    return INVALID;
}

FlowValidator::FlowValidator(const QList<Phase> &phases) : phases(phases) {}

ValidationResult FlowValidator::validate(const PipeLine &pipeLine, int progress) const {
    if (debugging) {
        std::cout << "Validating pipeline\n" << pipeLine.toQString(true).toStdString() << std::endl;
    }
    int coolnessRate = 0;
    for (const Phase &phase: phases) {
        Flow flow;
        int coolness = flow.makeFlow(pipeLine, phase);
        if (coolness == -1) {

            return INVALID;
        }
        coolnessRate += coolness;
    }
    return ValidationResult(VR_VALID, coolnessRate);
}

void FlowValidator::debug() {
    debugging = true;
}

WindowedFlowValidator::WindowedFlowValidator(const QList<Phase> &phases, QProgressDialog &progressDialog)
        : FlowValidator(phases), progressDialog(progressDialog) {

}

ValidationResult WindowedFlowValidator::validate(const PipeLine &pipeLine, int progress) const {
    ValidationResult result = FlowValidator::validate(pipeLine, 0);
    QCoreApplication::processEvents();
            progressDialog.setValue( progress);
    if(progressDialog.wasCanceled()){
        return BREAK;
    }
    return result;
}

bool ValidationResult::operator==(const ValidationResult &rhs) const {
    return type == rhs.type &&
           rateCoolness == rhs.rateCoolness;
}

bool ValidationResult::operator!=(const ValidationResult &rhs) const {
    return !(rhs == *this);
}

ValidationResult::ValidationResult(ValidationResultType type, int rateCoolness) : type(type),
                                                                                  rateCoolness(rateCoolness) {}

ValidationResultType ValidationResult::getType() const {
    return type;
}

int ValidationResult::getRateCoolness() const {
    return rateCoolness;
}
