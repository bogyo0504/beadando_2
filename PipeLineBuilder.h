//
// Created by gyberni15 on 12/29/24.
//

#ifndef BEADANDO_II_PIPELINEBUILDER_H
#define BEADANDO_II_PIPELINEBUILDER_H
#include "PipeLineValidator.h"

/**
 * A PipeLineBuilder osztály építi fel a csővezetéket
 * Illetve ennek a segítségével működik a Progress Bar, ami maximum 10^6-ig számol (kb. 87%)
 */
class PipeLineBuilder {

    /**
     * A validator ellenőrzi, hogy az adott csőrendszer helyes-e, tehát végig tud-e menni a folyadék
     * a csőrendszeren a forrástól a fogyasztóig a megadott fázisokban, illetve nem folyik-e ki a folyadék
     */
    const PipeLineValidator &validator;

    /**
     * A pipeline a csővezeték, amit építünk
     * Benne van a játéktér és a készlet
     */
    PipeLine &pipeline;

    /**
     * Nehéz megbecsülni a várható lépésszámot, illetve túllóg az "int" típus méretén, ezért csak egy folyamatso haladást próbálunk reprezentálni
     * úgy, hogy a százalékos szám folyamatosan növekedjen, tehát megtartjuk a jelenlegi lépésszám és az ábrázolt
     * tartomány közötti arányt, mikor átskáklázzuk a tartományt
     * A progressIntervalMin a progressIntervalMax a progressValue a progressLimit változók a Progress Barhoz kellenek
     * A progressIntervalMin a tartomány minimuma
     */
    int progressIntervalMin = 0;

    /**
     * A progressIntervalMax a tartomány maximuma
     */
    int progressIntervalMax = 1024;

    /**
     * A progressValue a jelenlegi érték százalékosan
     */
    int progressValue = 0;

    /**
     * A progressLimit értékénél történik az átskálázás
     */
    int progressLimit = 512;

    /**
     * Az inProgress változó azt mondja meg, hogy éppen folyamatban van-e az építés
     * A teszteléshez szükséges, a debugolás esetén
     */
    bool inProgress = false;

    /**
     * A debug változó azt mondja meg, hogy éppen debugolunk-e
     * A teszteléshez szükséges, a debugolás esetén
     */
    bool debugging = false;

    /**
     * A debugPosition változó azt mondja meg, hogy éppen melyik pozícióban vagyunk
     * A teszteléshez szükséges, a debugolás esetén
     * Segítségével kiírhatjuk az adott pozíciót és az elemet, amelyet rá akaruunk tenni
     */
    GridPosition debugPosition = INVALID_POSITION;

    /**
     *A buildStateStack a BuildState-eket tartalmazza, amelyek az építési fát representálja
     */
    QStack<std::shared_ptr<BuildState>> buildStateStack;

    /**
     * Az adott állapotból építi fel a csőrendszert
     * Addig folytatja, amíg a csőrendszer építhető
     * Például: OUT_OF_STOCK és READY értékeknél leáll
     */
    BuildState buildPipeLine(BuildState state);

    /**
     *Ezzel lépünk vissza az építési fában
     */
    QPair<bool, BuildState> stepBack();
public:
    PipeLineBuilder(const PipeLineValidator &validator, PipeLine &pipeline);

    /**
     * Ez a függvény építi a csöveket
     */
    ValidationResult build(const Stock &stock);

    void debug();

    /**
     * Kiírja a pozíciót, a debugoláshoz szükséges
     */
    void printPosition(GridPosition position);

    /**
     * Visszaállítja az építést, a felhasználói felületen a visszaállítás menüpontnál használjuk
     */
    void resetBuild();

};


#endif //BEADANDO_II_PIPELINEBUILDER_H
