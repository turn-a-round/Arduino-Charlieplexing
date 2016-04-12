/*
 * Created & Collected by Tamal Patra <patra.tamal@gmail.com>. Copyright 2016
 * Released for the sake of knowledge for mankind under The MIT License.
 */

/* 
 * File:   MatrixCharlieplex.cpp
 * Description: Drive (N * (N-1)) number of "Charlieplexed" diode (node) matrices
 *              (e.g. LED Matrix) with N number of outputs capable of tri-state.
 *              The grid consists of X-axis (horizontal) as Anodes & Y-axis
 *              (vertical) as Cathodes. Here the grid is displayed below.
 *              Inspired from http://wealoneonearth.blogspot.com/2013/03/design-note-charlieplexing-led-matrices.html
 *
 *                (-)C A T H O D E
 *             (+)   1 2 3 4 5 . N
 *              A  1 X 0 0 0 0
 *              N  2 0 X 0 0 0
 *              O  3 0 0 X 0 0
 *              D  4 0 0 0 X 0
 *              E  5 0 0 0 0 X
 *                 .           X
 *                 N             X
 * 
 *              There will be N number of rows and (N-1) number of columns. The 
 *              same line cannot be connected to both Anode & Cathode of the 
 *              same node. Hence, the "X" is marked, where they cannot be both
 *              at same time.
 * 
 *              It's best practice to connect the Anodes in same row & then 
 *              connect the Cathodes.
 * 
 *              e.g. Cathode Connection for a 5 pin setup
 *                   (-)C A T H O D E
 *              R  1 -> 2 3 4 5
 *              O  2 -> 1 3 4 5
 *              W  3 -> 1 2 4 5
 *              S  4 -> 1 2 3 5
 *                 5 -> 1 2 3 4
 * 
 * Author: Tamal Patra <patra.tamal@gmail.com> 
 * 
 * Created on April 1, 2016, 12:00 PM
 */

#include "Arduino.h"
#include "MatrixCharlieplex.h"

namespace ArduinoMatrixCharlieplex {
#pragma region Constructor

    //MatrixCharlieplex::MatrixCharlieplex() {
    //}

    MatrixCharlieplex::MatrixCharlieplex(uint8_t pins[], uint8_t noOfPins, uint8_t commonType) {
        _pins = pins;
        _commonType = commonType;
        _noOfPins = noOfPins; //sizeof (pins) / sizeof (uint8_t);
        _maxNode = _noOfPins * (_noOfPins - 1);
        _activeNode = new DiodeNode();
        _init(true);
    }
#pragma endregion Constructor

#pragma region Private Functions

    void MatrixCharlieplex::_init(boolean isStart) {
        // Sink all output pins
        _activeNode->vcc = 0;
        _activeNode->gnd = 0;
        _state = MXCHARLIE_INACTIVE;
        if (isStart) {
            //_exeDDRDn = {0b11111110, 0b00111111};
            //_exeDDRUp = {MXCHARLIE_UPMASK, MXCHARLIE_UPMASK};
            //_exePORTDn = {0b11111110, 0b00111111};
            //_exePORTUp = {MXCHARLIE_UPMASK, MXCHARLIE_UPMASK};
            _exeDDRDn[0] = 0b11111111;
            _exeDDRDn[1] = 0b00111111;
            _exeDDRUp[0] = MXCHARLIE_UPMASK;
            _exeDDRUp[1] = MXCHARLIE_UPMASK;
            _exePORTDn[0] = 0b11111111;
            _exePORTDn[1] = 0b00111111;
            _exePORTUp[0] = MXCHARLIE_UPMASK;
            _exePORTUp[1] = MXCHARLIE_UPMASK;
            //for (uint8_t i = 0; i < 5; i++) {
            for (uint8_t i = 0; i < _noOfPins; i++) {
                _sinkPin(*(_pins + i));
            }
            _ioDDR[0] = _exeDDRDn[0];
            _ioDDR[1] = _exeDDRDn[1];
            _ioPORT[0] = _exePORTDn[0];
            _ioPORT[1] = _exePORTDn[1];
        }
        _reset();
    }

//    void MatrixCharlieplex::_print() {
//        Serial.println("Status:-");
//        Serial.print("ioDDR[0]: ");
//        Serial.println(_ioDDR[0], BIN);
//        Serial.print("ioDDR[1]: ");
//        Serial.println(_ioDDR[1], BIN);
//        Serial.print("_ioPORT[0]: ");
//        Serial.println(_ioPORT[0], BIN);
//        Serial.print("_ioPORT[1]: ");
//        Serial.println(_ioPORT[1], BIN);
//        Serial.print("_exeDDRDn[0]: ");
//        Serial.println(_exeDDRDn[0], BIN);
//        Serial.print("_exeDDRDn[1]: ");
//        Serial.println(_exeDDRDn[1], BIN);
//        Serial.print("_exeDDRUp[0]: ");
//        Serial.println(_exeDDRUp[0], BIN);
//        Serial.print("_exeDDRUp[1]: ");
//        Serial.println(_exeDDRUp[1], BIN);
//        Serial.print("_exePORTDn[0]: ");
//        Serial.println(_exePORTDn[0], BIN);
//        Serial.print("_exePORTDn[1]: ");
//        Serial.println(_exePORTDn[1], BIN);
//        Serial.print("_exePORTUp[0]: ");
//        Serial.println(_exePORTUp[0], BIN);
//        Serial.print("_exePORTUp[1]: ");
//        Serial.println(_exePORTUp[1], BIN);
//        Serial.println("----------");
//    }

    boolean MatrixCharlieplex::_reset() {
        _exeDDRDn[0] = _ioDDR[0];
        _exeDDRDn[1] = _ioDDR[1];
        _exeDDRUp[0] = MXCHARLIE_UPMASK;
        _exeDDRUp[1] = MXCHARLIE_UPMASK;
        _exePORTDn[0] = _ioPORT[0];
        _exePORTDn[1] = _ioPORT[1];
        _exePORTUp[0] = MXCHARLIE_UPMASK;
        _exePORTUp[1] = MXCHARLIE_UPMASK;
        return _execute();
    }

    boolean MatrixCharlieplex::_execute() { //(boolean needClearance) {
        //_print();
//        if (needClearance) {
//            // Clear all previous setup
//            DDRD = DDRD & _ioDDR[0];
//            DDRB = DDRB & _ioDDR[1];
//            PORTD = PORTD & _ioPORT[0];
//            PORTB = PORTB & _ioPORT[1];
//        }
        DDRD = DDRD & _exeDDRDn[0] | _exeDDRUp[0];
        DDRB = DDRB & _exeDDRDn[1] | _exeDDRUp[1];
        PORTD = PORTD & _exePORTDn[0] | _exePORTUp[0];
        PORTB = PORTB & _exePORTDn[1] | _exePORTUp[1];
        return true;
    }

    // Help from http://www.instructables.com/id/Charlieplexing-the-Arduino/

    boolean MatrixCharlieplex::_upPin(uint8_t pin) {
        //        pinMode(pin, OUTPUT);
        //        digitalWrite(pin, HIGH);
        BitMan* bm = _getBitMan(pin);
        //_exeDDRUp[bm->y] |= 0b1 << bm->x;
        //_exePORTUp[bm->y] |= 0b1 << bm->x;
        //bitWrite(_exeDDRUp[bm->y], bm->x, 1);
        //bitWrite(_exePORTUp[bm->y], bm->x, 1);
        bitSet(_exeDDRUp[bm->y], bm->x);
        bitSet(_exePORTUp[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going up");
        return true;
    }

    boolean MatrixCharlieplex::_downPin(uint8_t pin) {
        //        pinMode(pin, OUTPUT);
        //        digitalWrite(pin, LOW);
        BitMan* bm = _getBitMan(pin);
        //_exeDDRUp[bm->y] |= 0b1 << bm->x;
        //bitWrite(_exePORTUp[bm->y], bm->x, 0);
        //bitWrite(_exeDDRUp[bm->y], bm->x, 1);
        //bitWrite(_exePORTDn[bm->y], bm->x, 0);
        bitClear(_exePORTUp[bm->y], bm->x);
        bitSet(_exeDDRUp[bm->y], bm->x);
        bitClear(_exePORTDn[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going down");
        return true;
    }

    boolean MatrixCharlieplex::_sinkPin(uint8_t pin) {
        //        pinMode(pin, INPUT);
        //        digitalWrite(pin, LOW);
        BitMan* bm = _getBitMan(pin);
        //bitWrite(_exeDDRUp[bm->y], bm->x, 0);
        //bitWrite(_exePORTUp[bm->y], bm->x, 0);
        //bitWrite(_exeDDRDn[bm->y], bm->x, 0);
        //bitWrite(_exePORTDn[bm->y], bm->x, 0);
        bitClear(_exeDDRUp[bm->y], bm->x);
        bitClear(_exePORTUp[bm->y], bm->x);
        bitClear(_exeDDRDn[bm->y], bm->x);
        bitClear(_exePORTDn[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going sink");
        return true;
    }

    boolean MatrixCharlieplex::_setNode(DiodeNode* pin, uint8_t state) {
        if (state) {
            uint8_t _chkMatch = 0;

            if (_activeNode->vcc == pin->vcc) {
                _chkMatch |= 0b10;
            } else if (_activeNode->vcc == pin->gnd) {
                _downPin(pin->gnd);
                _chkMatch |= 0b01;
            } else {
                if (_state)
                    _sinkPin(_activeNode->vcc);
            }

            if (_activeNode->gnd == pin->vcc) {
                _upPin(pin->vcc);
                _chkMatch |= 0b10;
            } else if (_activeNode->gnd == pin->gnd) {
                _chkMatch |= 0b01;
            } else {
                if (_state)
                    _sinkPin(_activeNode->gnd);
            }

            // Now Set the requested pin's state
            //_upPin(pin->vcc);
            //_downPin(pin->gnd);
            if ((0b10 & _chkMatch) != 0b10) {
                _upPin(pin->vcc);
            }
            if ((0b01 & _chkMatch) != 0b01) {
                _downPin(pin->gnd);
            }
            _activeNode->vcc = pin->vcc;
            _activeNode->gnd = pin->gnd;
            _state = MXCHARLIE_ACTIVE;
            //return true;
            return _execute();
        } else { //The objective is to check the given Node doesn't get conflict
            uint8_t _chkMatch = 0; // Whether the given node is the ActiveNode
            uint8_t _chkConflict = 0; // Whether it conflicts with ActiveNode
            uint8_t _chkClear = 0; // Whether it doesn't conflict with ActiveNode

            if (_activeNode->vcc == pin->vcc) {
                _chkMatch = (_chkMatch << 1) | 1;
            } else if (_activeNode->vcc == pin->gnd) {
                _chkConflict = (_chkConflict << 1) | 1;
            } else {
                _chkClear = (_chkClear << 1) | 1;
            }

            if (_activeNode->gnd == pin->vcc) {
                _chkConflict = (_chkConflict << 1) | 1;
            } else if (_activeNode->gnd == pin->gnd) {
                _chkMatch = (_chkMatch << 1) | 1;
            } else {
                _chkClear = (_chkClear << 1) | 1;
            }

            if (0b11 == _chkClear) { // No harm in changing
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                //return true;
                return _execute();
            } else if (0b11 & _chkConflict) { // If any conflict happens
                return false;
            } else if (0b11 == _chkMatch) { // Exact match to ActiveNode
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                _activeNode->vcc = 0;
                _activeNode->gnd = 0;
                _state = MXCHARLIE_INACTIVE;
                //return true;
                return _execute();
            }
            return false;
        }
        return false;
    }

    DiodeNode* MatrixCharlieplex::_getNode(uint8_t row, uint8_t col) {
        DiodeNode* node = new DiodeNode();
        /* The objective is to map the exact pin as based on the Matrix. The 
         * setup supports both Common-Cathode (CC) or Common-Anode (CA) grid.
         * 
         * Here Array are 0 based (i.e. starts the counts from 0 & then 1, 2, 3)
         * whereas rows and columns are count from 1. Need a conversion here.
         * 
         * e.g.
         *          Array: 0 | 1 | 2 | 3 | 4
         *                 ^ | ^ | ^ | ^ | ^
         *            Row: 1 | 2 | 3 | 4 | 5
         *         Column: 1 | 2 | 3 | 4 | 5
         */
        //node->vcc = *(_pins + (row - 1)); // same as row number
        //node->gnd = *(_pins + ((col < row) ? (col - 1) : col)); // complicated
        uint8_t x = *(_pins + (row - 1));
        uint8_t y = *(_pins + ((col < row) ? (col - 1) : col));
        if (_commonType) {
            node->vcc = x; // same as row number
            node->gnd = y; // complicated
        } else { // just reverse
            node->vcc = y;
            node->gnd = x;
        }
        return node;
    }

    DiodeNode* MatrixCharlieplex::_getNode(uint16_t index) {
        DiodeNode* node = new DiodeNode();
        /* This one is also calculated based on 0-based indexing like the above
         * one.
         */
        uint8_t row = (index - 1) / (_noOfPins - 1);
        uint8_t col = (index - 1) % (_noOfPins - 1);
        //node->vcc = *(_pins + row);
        //node->gnd = *(_pins + ((col < row) ? col : (col + 1)));
        uint8_t x = *(_pins + row);
        uint8_t y = *(_pins + ((col < row) ? col : (col + 1)));
        if (_commonType) {
            node->vcc = x; // same as row number
            node->gnd = y; // again complicated
        } else { // just reverse
            node->vcc = y;
            node->gnd = x;
        }
        return node;
    }

    BitMan* MatrixCharlieplex::_getBitMan(uint16_t pin) {
        BitMan* bm = new BitMan();
        bm->y = pin / 8;
        bm->x = pin % 8;
        return bm;
    }
#pragma endregion Private Functions

#pragma region Public Functions

//    DiodeNode* MatrixCharlieplex::getActiveNode() {
//        return _activeNode;
//    }
//
//    uint8_t* MatrixCharlieplex::getPins() {
//        return _pins;
//    }

    boolean MatrixCharlieplex::turnOn(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), HIGH);
    }

    boolean MatrixCharlieplex::turnOff(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), LOW);
    }

    boolean MatrixCharlieplex::turnOn(uint16_t index) {
        return _setNode(_getNode(index), HIGH);
    }

    boolean MatrixCharlieplex::turnOff(uint16_t index) {
        return _setNode(_getNode(index), LOW);
    }

    boolean MatrixCharlieplex::clear() {
        return ((_state == MXCHARLIE_INACTIVE) ? true : _setNode(_activeNode, LOW));
    }

    boolean MatrixCharlieplex::reset() {
        //if (_setNode(_activeNode, LOW))
        //    return _init(false);
        //return false;
        _init(false);
        return true;
    }
#pragma endregion Public Functions
}
