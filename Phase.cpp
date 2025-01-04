//
// Created by gyberni15 on 12/29/24.
//

#include "Phase.h"

QSet<TileColor> Phase::getActiveColors() const {
    return activeColors;
}

Phase::Phase(const QSet<TileColor> &activeColors) : activeColors(activeColors) {
}

Phase::Phase() {}

Phase::Phase(TileColor color) : activeColors({color}) {}

Phase Phase::fromString(const QString &string) {
    QSet<TileColor> colors;
    for (QChar c: string) {
        switch (c.toLatin1()) {
            case 'R':
                colors.insert(RED);
                continue;
            case 'B':
                colors.insert(BLUE);
                continue;
            case 'C':
                colors.insert(CIAN);
                continue;
            case 'G':
                colors.insert(GREEN);
                continue;
            case 'Y':
                colors.insert(YELLOW);
                continue;
            default:
                continue;
        }
    }
    return Phase(colors);
}

QString Phase::toQString() const {
    QString result;
    for(TileColor color: activeColors){
        switch (color) {
            case RED:
                result += "R";
                break;
            case CIAN:
                result += "C";
                break;
            case BLUE:
                result += "B";
                break;
            case GREEN:
                result += "G";
                break;
            case YELLOW:
                result += "Y";
                break;
            default:
                result += " ";
        }
    }
    return result;
}
