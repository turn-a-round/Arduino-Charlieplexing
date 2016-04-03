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

    MatrixCharlieplex::MatrixCharlieplex(uint8_t pins[]) {
        this->_pins = pins;
        this->_noOfPins = sizeof (pins) / sizeof (uint8_t);
        this->_maxNode = this->_noOfPins * (this->_noOfPins - 1);
        this->_activeNode = new DiodeNode();
        this->_init(false);
    }
#pragma endregion Constructor

#pragma region Private Functions

    void MatrixCharlieplex::_init(boolean isReset) {
        // Sink all output pins
        for (uint8_t i = 0; i < this->_noOfPins; i++) {
            _sinkPin(*(this->_pins + i));
        }
        this->_activeNode->vcc = 0;
        this->_activeNode->gnd = 0;
        this->_state = MXCHARLIE_INACTIVE;
    }

    // Help from http://www.instructables.com/id/Charlieplexing-the-Arduino/

    boolean MatrixCharlieplex::_upPin(uint8_t pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        //        Serial.print(pin);
        //        Serial.println("-going up");
        return true;
    }

    boolean MatrixCharlieplex::_downPin(uint8_t pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        //        Serial.print(pin);
        //        Serial.println("-going down");
        return true;
    }

    boolean MatrixCharlieplex::_sinkPin(uint8_t pin) {
        pinMode(pin, INPUT);
        digitalWrite(pin, LOW);
        //        Serial.print(pin);
        //        Serial.println("-going sink");
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
            return true;
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

            if (this->_activeNode->vcc == pin->vcc) {
                _chkConflict = (_chkConflict << 1) | 1;
            } else if (this->_activeNode->vcc == pin->gnd) {
                _chkMatch = (_chkMatch << 1) | 1;
            } else {
                _chkClear = (_chkClear << 1) | 1;
            }

            if (0b11 == _chkClear) { // No harm in changing
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                return true;
            } else if (0b11 & _chkConflict) { // If any conflict happens
                return false;
            } else if (0b11 == _chkMatch) { // Exact match to ActiveNode
                _sinkPin(pin->vcc);
                _sinkPin(pin->gnd);
                this->_activeNode->vcc = 0;
                this->_activeNode->gnd = 0;
                this->_state = MXCHARLIE_INACTIVE;
                return true;
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
        node->vcc = *(this->_pins + (row - 1)); // same as row number
        node->gnd = *(this->_pins + ((col < row) ? (col - 1) : col)); // complicated
        return node;
    }

    DiodeNode* MatrixCharlieplex::_getNode(uint16_t index) {
        DiodeNode* node = new DiodeNode();
        /* This one is also calculated based on 0-based indexing like the above
         * one.
         */
        uint8_t row = (index - 1) / (this->_noOfPins - 1);
        uint8_t col = (index - 1) % (this->_noOfPins - 1);
        node->vcc = *(this->_pins + row); // same as row number
        node->gnd = *(this->_pins + ((col < row) ? col : (col + 1))); // again complicated
        return node;
    }
#pragma endregion Private Functions

#pragma region Public Functions

    DiodeNode* MatrixCharlieplex::GetActiveNode() {
        return this->_activeNode;
    }

    uint8_t* MatrixCharlieplex::GetPins() {
        return this->_pins;
    }

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
        //if (_setNode(this->_activeNode, LOW))
        //    return _init(true);
        //return false;
        _init(true);
        return true;
    }
#pragma endregion Public Functions
}
