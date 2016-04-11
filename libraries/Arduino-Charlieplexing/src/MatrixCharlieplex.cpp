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
    MatrixCharlieplex::MatrixCharlieplex(uint8_t pins[], uint8_t noOfPins, uint8_t commonType) {
        this->_pins = pins;
        this->_commonType = commonType;
        this->_noOfPins = noOfPins;
        this->_maxNode = this->_noOfPins * (this->_noOfPins - 1);
        this->_activeNode = new DiodeNode();
        this->_init(true);
    }
#pragma endregion Constructor

#pragma region Private Functions
    void MatrixCharlieplex::_init(boolean isStart) {
        this->_activeNode->vcc = 0;
        this->_activeNode->gnd = 0;
        this->_state = MXCHARLIE_INACTIVE;
        if (isStart) {
            this->_exeDDRDn[0] = 0b11111111;
            this->_exeDDRDn[1] = 0b00111111;
            this->_exeDDRUp[0] = MXCHARLIE_UPMASK;
            this->_exeDDRUp[1] = MXCHARLIE_UPMASK;
            this->_exePORTDn[0] = 0b11111111;
            this->_exePORTDn[1] = 0b00111111;
            this->_exePORTUp[0] = MXCHARLIE_UPMASK;
            this->_exePORTUp[1] = MXCHARLIE_UPMASK;
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

    boolean MatrixCharlieplex::_execute() {
        DDRD = DDRD & this->_exeDDRDn[0] | this->_exeDDRUp[0];
        DDRB = DDRB & this->_exeDDRDn[1] | this->_exeDDRUp[1];
        PORTD = PORTD & this->_exePORTDn[0] | this->_exePORTUp[0];
        PORTB = PORTB & this->_exePORTDn[1] | this->_exePORTUp[1];
        return true;
    }

    boolean MatrixCharlieplex::_upPin(uint8_t pin) {
        BitMan* bm = _getBitMan(pin);
        bitSet(this->_exeDDRUp[bm->y], bm->x);
        bitSet(this->_exePORTUp[bm->y], bm->x);
        return true;
    }

    boolean MatrixCharlieplex::_downPin(uint8_t pin) {
        BitMan* bm = _getBitMan(pin);
        bitClear(this->_exePORTUp[bm->y], bm->x);
        bitSet(this->_exeDDRUp[bm->y], bm->x);
        bitClear(this->_exePORTDn[bm->y], bm->x);
        return true;
    }

    boolean MatrixCharlieplex::_sinkPin(uint8_t pin) {
        BitMan* bm = _getBitMan(pin);
        bitClear(this->_exeDDRUp[bm->y], bm->x);
        bitClear(this->_exePORTUp[bm->y], bm->x);
        bitClear(this->_exeDDRDn[bm->y], bm->x);
        bitClear(this->_exePORTDn[bm->y], bm->x);
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

            if ((0b10 & _chkMatch) != 0b10) {
                _upPin(pin->vcc);
            }
            if ((0b01 & _chkMatch) != 0b01) {
                _downPin(pin->gnd);
            }
            this->_activeNode->vcc = pin->vcc;
            this->_activeNode->gnd = pin->gnd;
            this->_state = MXCHARLIE_ACTIVE;
            return _execute();
        } else {
            uint8_t _chkMatch = 0;
            uint8_t _chkConflict = 0;
            uint8_t _chkClear = 0;

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

            if (0b11 == _chkClear) {
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                return _execute();
            } else if (0b11 & _chkConflict) {
                return false;
            } else if (0b11 == _chkMatch) {
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                this->_activeNode->vcc = 0;
                this->_activeNode->gnd = 0;
                this->_state = MXCHARLIE_INACTIVE;
                return _execute();
            }
            return false;
        }
        return false;
    }

    DiodeNode* MatrixCharlieplex::_getNode(uint8_t row, uint8_t col) {
        DiodeNode* node = new DiodeNode();
        uint8_t x = *(this->_pins + (row - 1));
        uint8_t y = *(this->_pins + ((col < row) ? (col - 1) : col));
        if (this->_commonType) {
            node->vcc = x;
            node->gnd = y;
        } else {
            node->vcc = y;
            node->gnd = x;
        }
        return node;
    }

    DiodeNode* MatrixCharlieplex::_getNode(uint16_t index) {
        DiodeNode* node = new DiodeNode();
        uint8_t row = (index - 1) / (this->_noOfPins - 1);
        uint8_t col = (index - 1) % (this->_noOfPins - 1);
        uint8_t x = *(this->_pins + row);
        uint8_t y = *(this->_pins + ((col < row) ? col : (col + 1)));
        if (this->_commonType) {
            node->vcc = x;
            node->gnd = y;
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
    boolean MatrixCharlieplex::TurnOn(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), HIGH);
    }

    boolean MatrixCharlieplex::TurnOff(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), LOW);
    }

    boolean MatrixCharlieplex::TurnOn(uint16_t index) {
        return _setNode(_getNode(index), HIGH);
    }

    boolean MatrixCharlieplex::TurnOff(uint16_t index) {
        return _setNode(_getNode(index), LOW);
    }

    boolean MatrixCharlieplex::Clear() {
        return ((this->_state == MXCHARLIE_INACTIVE) ? true : _setNode(this->_activeNode, LOW));
    }

    boolean MatrixCharlieplex::Reset() {
        _init(false);
        return true;
    }
#pragma endregion Public Functions
}
