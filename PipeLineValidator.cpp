//
// Created by gyberni15 on 12/29/24.
//

#include <iostream>
#include "PipeLineValidator.h"

bool PipeLineValidator::validate(const PipeLine &pipeLine) {
    return false;
}

FlowValidator::FlowValidator(const QList<Phase> &phases) : phases(phases) {}

bool FlowValidator::validate(const PipeLine &pipeLine) {
    if (debugging) {
        std::cout << "Validating pipeline\n" << pipeLine.toQString(true).toStdString() << std::endl;
    }
    for (const Phase &phase: phases) {
        Flow flow;
        if (!flow.makeFlow(pipeLine, phase)) {
            return false;
        }
    }
    return true;
}

void FlowValidator::debug() {
    debugging = true;
}
