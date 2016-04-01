# Arduino-Charlieplexing
A library for Arduino to control Charlieplexed LED matrices

Drive (N * (N-1)) number of "Charlieplxed" diode matrices
(e.g. LED Matrix) with N number of outputs capable of tri-state.

           C A T H O D E
           1 2 3 4 5 . N
      A  1 X 0 0 0 0
      N  2 0 X 0 0 0
      O  3 0 0 X 0 0
      D  4 0 0 0 X 0
      E  5 0 0 0 0 X
         .           X
         N             X
