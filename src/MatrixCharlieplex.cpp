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
        this->_pins = pins;
        this->_commonType = commonType;
        this->_noOfPins = noOfPins; //sizeof (pins) / sizeof (uint8_t);
        this->_maxNode = this->_noOfPins * (this->_noOfPins - 1);
        this->_activeNode = new DiodeNode();
        this->_init(true);
    }
#pragma endregion Constructor

#pragma region Private Functions

    void MatrixCharlieplex::_init(boolean isStart) {
        // Sink all output pins
        this->_activeNode->vcc = 0;
        this->_activeNode->gnd = 0;
        this->_state = MXCHARLIE_INACTIVE;
        if (isStart) {
            //this->_exeDDRDn = {0b11111110, 0b00111111};
            //this->_exeDDRUp = {MXCHARLIE_UPMASK, MXCHARLIE_UPMASK};
            //this->_exePORTDn = {0b11111110, 0b00111111};
            //this->_exePORTUp = {MXCHARLIE_UPMASK, MXCHARLIE_UPMASK};
            this->_exeDDRDn[0] = 0b11111111;
            this->_exeDDRDn[1] = 0b00111111;
            this->_exeDDRUp[0] = MXCHARLIE_UPMASK;
            this->_exeDDRUp[1] = MXCHARLIE_UPMASK;
            this->_exePORTDn[0] = 0b11111111;
            this->_exePORTDn[1] = 0b00111111;
            this->_exePORTUp[0] = MXCHARLIE_UPMASK;
            this->_exePORTUp[1] = MXCHARLIE_UPMASK;
            //for (uint8_t i = 0; i < 5; i++) {
            for (uint8_t i = 0; i < this->_noOfPins; i++) {
                _sinkPin(*(this->_pins + i));
            }
            this->_ioDDR[0] = this->_exeDDRDn[0];
            this->_ioDDR[1] = this->_exeDDRDn[1];
            this->_ioPORT[0] = this->_exePORTDn[0];
            this->_ioPORT[1] = this->_exePORTDn[1];
        }
        _reset();
    }

//    void MatrixCharlieplex::_print() {
//        Serial.println("Status:-");
//        Serial.print("this->ioDDR[0]: ");
//        Serial.println(this->_ioDDR[0], BIN);
//        Serial.print("this->ioDDR[1]: ");
//        Serial.println(this->_ioDDR[1], BIN);
//        Serial.print("this->_ioPORT[0]: ");
//        Serial.println(this->_ioPORT[0], BIN);
//        Serial.print("this->_ioPORT[1]: ");
//        Serial.println(this->_ioPORT[1], BIN);
//        Serial.print("this->_exeDDRDn[0]: ");
//        Serial.println(this->_exeDDRDn[0], BIN);
//        Serial.print("this->_exeDDRDn[1]: ");
//        Serial.println(this->_exeDDRDn[1], BIN);
//        Serial.print("this->_exeDDRUp[0]: ");
//        Serial.println(this->_exeDDRUp[0], BIN);
//        Serial.print("this->_exeDDRUp[1]: ");
//        Serial.println(this->_exeDDRUp[1], BIN);
//        Serial.print("this->_exePORTDn[0]: ");
//        Serial.println(this->_exePORTDn[0], BIN);
//        Serial.print("this->_exePORTDn[1]: ");
//        Serial.println(this->_exePORTDn[1], BIN);
//        Serial.print("this->_exePORTUp[0]: ");
//        Serial.println(this->_exePORTUp[0], BIN);
//        Serial.print("this->_exePORTUp[1]: ");
//        Serial.println(this->_exePORTUp[1], BIN);
//        Serial.println("----------");
//    }

    boolean MatrixCharlieplex::_reset() {
        this->_exeDDRDn[0] = this->_ioDDR[0];
        this->_exeDDRDn[1] = this->_ioDDR[1];
        this->_exeDDRUp[0] = MXCHARLIE_UPMASK;
        this->_exeDDRUp[1] = MXCHARLIE_UPMASK;
        this->_exePORTDn[0] = this->_ioPORT[0];
        this->_exePORTDn[1] = this->_ioPORT[1];
        this->_exePORTUp[0] = MXCHARLIE_UPMASK;
        this->_exePORTUp[1] = MXCHARLIE_UPMASK;
        return _execute();
    }

    boolean MatrixCharlieplex::_execute() { //(boolean needClearance) {
        //_print();
//        if (needClearance) {
//            // Clear all previous setup
//            DDRD = DDRD & this->_ioDDR[0];
//            DDRB = DDRB & this->_ioDDR[1];
//            PORTD = PORTD & this->_ioPORT[0];
//            PORTB = PORTB & this->_ioPORT[1];
//        }
        DDRD = DDRD & this->_exeDDRDn[0] | this->_exeDDRUp[0];
        DDRB = DDRB & this->_exeDDRDn[1] | this->_exeDDRUp[1];
        PORTD = PORTD & this->_exePORTDn[0] | this->_exePORTUp[0];
        PORTB = PORTB & this->_exePORTDn[1] | this->_exePORTUp[1];
        return true;
    }

    // Help from http://www.instructables.com/id/Charlieplexing-the-Arduino/

    boolean MatrixCharlieplex::_upPin(uint8_t pin) {
        //        pinMode(pin, OUTPUT);
        //        digitalWrite(pin, HIGH);
        BitMan* bm = _getBitMan(pin);
        //this->_exeDDRUp[bm->y] |= 0b1 << bm->x;
        //this->_exePORTUp[bm->y] |= 0b1 << bm->x;
        //bitWrite(this->_exeDDRUp[bm->y], bm->x, 1);
        //bitWrite(this->_exePORTUp[bm->y], bm->x, 1);
        bitSet(this->_exeDDRUp[bm->y], bm->x);
        bitSet(this->_exePORTUp[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going up");
        return true;
    }

    boolean MatrixCharlieplex::_downPin(uint8_t pin) {
        //        pinMode(pin, OUTPUT);
        //        digitalWrite(pin, LOW);
        BitMan* bm = _getBitMan(pin);
        //this->_exeDDRUp[bm->y] |= 0b1 << bm->x;
        //bitWrite(this->_exePORTUp[bm->y], bm->x, 0);
        //bitWrite(this->_exeDDRUp[bm->y], bm->x, 1);
        //bitWrite(this->_exePORTDn[bm->y], bm->x, 0);
        bitClear(this->_exePORTUp[bm->y], bm->x);
        bitSet(this->_exeDDRUp[bm->y], bm->x);
        bitClear(this->_exePORTDn[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going down");
        return true;
    }

    boolean MatrixCharlieplex::_sinkPin(uint8_t pin) {
        //        pinMode(pin, INPUT);
        //        digitalWrite(pin, LOW);
        BitMan* bm = _getBitMan(pin);
        //bitWrite(this->_exeDDRUp[bm->y], bm->x, 0);
        //bitWrite(this->_exePORTUp[bm->y], bm->x, 0);
        //bitWrite(this->_exeDDRDn[bm->y], bm->x, 0);
        //bitWrite(this->_exePORTDn[bm->y], bm->x, 0);
        bitClear(this->_exeDDRUp[bm->y], bm->x);
        bitClear(this->_exePORTUp[bm->y], bm->x);
        bitClear(this->_exeDDRDn[bm->y], bm->x);
        bitClear(this->_exePORTDn[bm->y], bm->x);
        //Serial.print(pin);
        //Serial.println("-going sink");
        return true;
    }

    boolean MatrixCharlieplex::_setNode(DiodeNode* pin, uint8_t state) {
        if (state) {
            uint8_t _chkMatch = 0;

            if (this->_activeNode->vcc == pin->vcc) {
                _chkMatch |= 0b10;
            } else if (this->_activeNode->vcc == pin->gnd) {
                _downPin(pin->gnd);
                _chkMatch |= 0b01;
            } else {
                if (this->_state)
                    _sinkPin(this->_activeNode->vcc);
            }

            if (this->_activeNode->gnd == pin->vcc) {
                _upPin(pin->vcc);
                _chkMatch |= 0b10;
            } else if (this->_activeNode->gnd == pin->gnd) {
                _chkMatch |= 0b01;
            } else {
                if (this->_state)
                    _sinkPin(this->_activeNode->gnd);
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
            this->_activeNode->vcc = pin->vcc;
            this->_activeNode->gnd = pin->gnd;
            this->_state = MXCHARLIE_ACTIVE;
            //return true;
            return _execute();
        } else { //The objective is to check the given Node doesn't get conflict
            uint8_t _chkMatch = 0; // Whether the given node is the ActiveNode
            uint8_t _chkConflict = 0; // Whether it conflicts with ActiveNode
            uint8_t _chkClear = 0; // Whether it doesn't conflict with ActiveNode

            if (this->_activeNode->vcc == pin->vcc) {
                _chkMatch = (_chkMatch << 1) | 1;
            } else if (this->_activeNode->vcc == pin->gnd) {
                _chkConflict = (_chkConflict << 1) | 1;
            } else {
                _chkClear = (_chkClear << 1) | 1;
            }

            if (this->_activeNode->gnd == pin->vcc) {
                _chkConflict = (_chkConflict << 1) | 1;
            } else if (this->_activeNode->gnd == pin->gnd) {
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
                this->_activeNode->vcc = 0;
                this->_activeNode->gnd = 0;
                this->_state = MXCHARLIE_INACTIVE;
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
        //node->vcc = *(this->_pins + (row - 1)); // same as row number
        //node->gnd = *(this->_pins + ((col < row) ? (col - 1) : col)); // complicated
        uint8_t x = *(this->_pins + (row - 1));
        uint8_t y = *(this->_pins + ((col < row) ? (col - 1) : col));
        if (this->_commonType) {
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
        uint8_t row = (index - 1) / (this->_noOfPins - 1);
        uint8_t col = (index - 1) % (this->_noOfPins - 1);
        //node->vcc = *(this->_pins + row);
        //node->gnd = *(this->_pins + ((col < row) ? col : (col + 1)));
        uint8_t x = *(this->_pins + row);
        uint8_t y = *(this->_pins + ((col < row) ? col : (col + 1)));
        if (this->_commonType) {
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
//        return this->_activeNode;
//    }
//
//    uint8_t* MatrixCharlieplex::getPins() {
//        return this->_pins;
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
        return ((this->_state == MXCHARLIE_INACTIVE) ? true : _setNode(this->_activeNode, LOW));
    }

    boolean MatrixCharlieplex::reset() {
        //if (_setNode(this->_activeNode, LOW))
        //    return _init(false);
        //return false;
        _init(false);
        return true;
    }
#pragma endregion Public Functions
}
