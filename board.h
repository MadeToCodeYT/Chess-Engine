/*
    Board's state is an 8x8 board with standard piece notation
    White pieces are uppercase, Black pieces are lowercase
    R - Rook
    N - Knight
    B - Bishop
    Q - Queen
    K - King
    P - Pawn (usually doesn't have a letter during moves -> e4, d4)
*/

class Board {
    public:
        char state[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}, // Row 8 (Black)
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'}, // Row 7
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // Row 6
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // Row 5
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // Row 4
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // Row 3
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'}, // Row 2
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}  // Row 1 (White)
        };

        bool whiteToMove = true;
};