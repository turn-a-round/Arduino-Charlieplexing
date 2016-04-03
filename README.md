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

 There will be N number of rows and (N-1) number of columns. 
The same line cannot be connected to both Anode & Cathode of the same node. 
Hence, the "X" is marked, where they cannot be both at same time.

It's best practice to connect the Anodes in same row & then connect the Cathodes.
e.g. Cathode Connection for a 5 pin setup

         (-)C A T H O D E
    R  1 -> 2 3 4 5
    O  2 -> 1 3 4 5
    W  3 -> 1 2 4 5
    S  4 -> 1 2 3 5
       5 -> 1 2 3 4
