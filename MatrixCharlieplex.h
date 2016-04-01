/*
 * Created & Collected by Tamal Patra <patra.tamal@gmail.com>. Copyright 2016
 * Released for the sake of knowledge for mankind under The MIT License.
 */

/* 
 * File:        MatrixCharlieplex.h
 * Description: Drive (N * (N-1)) number of "Charlieplxed" diode matrices
 *              (e.g. LED Matrix) with N number of outputs capable of tri-state.
 *              It 
 *
 *                   C A T H O D E
 *                   1 2 3 4 5 . N
 *              A  1 X 0 0 0 0
 *              N  2 0 X 0 0 0
 *              O  3 0 0 X 0 0
 *              D  4 0 0 0 X 0
 *              E  5 0 0 0 0 X
 *                 .           X
 *                 N             X
 *
 * Author:      Tamal Patra <patra.tamal@gmail.com>
 *
 * Created on March 29, 2016, 12:33 PM
 */

#ifndef MATRIXCHARLIEPLEX_H
#define MATRIXCHARLIEPLEX_H

#define MXCHARLIE_ACTIVE 0x01
#define MXCHARLIE_INACTIVE 0x00

#include<Arduino.h>

namespace MatrixCharlieplex {
    typedef struct{
        uint8_t vcc;
        uint8_t gnd;
    } DiodeNode;

    class MatrixCharlieplex{
    public:
        MatrixCharlieplex(uint8_t pins[]);
        boolean TurnOn(uint8_t row, uint8_t col);
        boolean TurnOff(uint8_t row, uint8_t col);
        boolean Clear();
        boolean Reset();

    private:
        uint8_t* _pins;
        uint8_t _noOfPins;
        uint8_t _maxNode;
        uint8_t _state;
        DiodeNode* _activeNode;
        void _init(boolean isReset);
        boolean _upPin(uint8_t pin);
        boolean _downPin(uint8_t pin);
        boolean _sinkPin(uint8_t pin);
        boolean _setNode(DiodeNode* pin, uint8_t state);
        DiodeNode* _getNode(uint8_t row, uint8_t col);
    };
}
#endif /* MATRIXCHARLIEPLEX_H */

