//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PHASE_H
#define BEADANDO_II_PHASE_H
#include "Tile.h"
#include <QSet>
/**
 * A Phase osztály egy fázist reprezentál a játékban. benne vannak az aktuálisan
 * "indított" színek.
 */
class Phase {
    QSet<TileColor> activeColors;
public:
    explicit Phase(const QSet<TileColor> &activeColors);

    Phase();

    Phase(TileColor color);

    /**
     * A setActiveColors függvény visszaadja az aktív színeket.
     */
    QSet<TileColor> getActiveColors() const;

    /**
     * A fázist kii tudjuk olvasni QStringből
     */
    static Phase fromString(const QString &string);

    /**
     * A fázist ki tudjuk írni QStringbe
     */
    QString toQString() const;
};


#endif //BEADANDO_II_PHASE_H
