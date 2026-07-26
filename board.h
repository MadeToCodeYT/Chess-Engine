#pragma once

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

#include <vector>
using namespace std;

struct Position {
    int rank;
    int file;
};

struct Move {
    Position start;
    Position end;
};

class Board {
    public:
        char state[8][8] = { // state [ rank ] [ file ]
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

        vector<Move> GetPsuedoLegalMoves() {
            vector<Move> psuedoLegalMoves;

            vector<Position> turnPieces; // All pieces of whoever's turn it is to move
            for (int file = 0; file < 8; file++) {
                for (int rank = 0; rank < 8; rank++) {
                    if (isupper(state[rank][file]) == whiteToMove) {
                        turnPieces.push_back({rank, file});
                    }
                }    
            }

            // Loop through board pieces and add each possible move
            for (Position piece : turnPieces) {
                bool isWhitePiece = isupper(state[piece.rank][piece.file]);
                switch ((char)tolower(state[piece.rank][piece.file])) {
                    case 'p': {
                        if (isWhitePiece) { // White pawn moves
                            if (getPieceAtSquare(piece.rank - 1, piece.file) == ' ') {
                                // Move up 1 if front if empty
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}} );
                                
                                if (getPieceAtSquare(piece.rank - 2, piece.file) == ' ' && piece.rank == 6) {
                                    // Move up 2 on first move
                                    psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 2, piece.file}} );
                                }
                            }

                            // Capture diagonally 1
                            if (!isupper( getPieceAtSquare(piece.rank - 1, piece.file - 1) ) && getPieceAtSquare(piece.rank - 1, piece.file - 1) != ' ') {
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}} );
                            }
                            // Capture diagonally 2
                            if (!isupper( getPieceAtSquare(piece.rank - 1, piece.file + 1) ) && getPieceAtSquare(piece.rank - 1, piece.file + 1) != ' ') {
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}} );
                            }
                        } else { // Black pawn moves
                            if (getPieceAtSquare(piece.rank + 1, piece.file) == ' ') {
                                // Move up 1 if front if empty
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}} );
                                
                                if (getPieceAtSquare(piece.rank + 2, piece.file) == ' ' && piece.rank == 1) {
                                    // Move up 2 on first move
                                    psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 2, piece.file}} );
                                }
                            }

                            // Capture diagonally 1
                            if (isupper( getPieceAtSquare(piece.rank + 1, piece.file + 1) ) && getPieceAtSquare(piece.rank + 1, piece.file + 1) != ' ') {
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}} );
                            }
                            // Capture diagonally 2
                            if (isupper( getPieceAtSquare(piece.rank + 1, piece.file - 1) ) && getPieceAtSquare(piece.rank + 1, piece.file - 1) != ' ') {
                                psuedoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}} );
                            }
                        }

                        break;
                    }
                    case 'n': {
                        int movesOffsets[8][2] = {
                            {1, 2}, {2, 1}, {-1, 2}, {-2, 1},
                            {1, -2}, {2, -1}, {-1, -2}, {-2, -1}
                        };
                        for (int i = 0; i < 8; i++) {
                            int df = movesOffsets[i][0];
                            int dr = movesOffsets[i][1];
                            int targetRank = piece.rank + dr;
                            int targetFile = piece.file + df;
                            char target = getPieceAtSquare(targetRank, targetFile);
                            if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7) continue;
                            
                            // Move to empty or enemy piece
                            if (target == ' ' || (isupper(target) != isWhitePiece)) {
                                psuedoLegalMoves.push_back({{piece.rank, piece.file}, {targetRank, targetFile}});
                            }
                        }
                        break;
                    }
                    case 'b': {
                        int movements[4][2] = {
                            {1, 1},
                            {-1, 1},
                            {1, -1},
                            {-1, -1}
                        };
                        vector<Move> moves = longRangePiece(piece, movements, 4, isWhitePiece);
                        
                        psuedoLegalMoves.insert(psuedoLegalMoves.end(), moves.begin(), moves.end());
                        break;
                    }
                    case 'r': {
                        int movements[4][2] = {
                            {1, 0},
                            {0, 1},
                            {-1, 0},
                            {0, -1}
                        };
                        vector<Move> moves = longRangePiece(piece, movements, 4, isWhitePiece);
                        
                        psuedoLegalMoves.insert(psuedoLegalMoves.end(), moves.begin(), moves.end());
                        break;
                    }
                    case 'q': {
                        int movements[8][2] = {
                            {1, 0},
                            {0, 1},
                            {-1, 0},
                            {0, -1},
                            {1, 1},
                            {-1, 1},
                            {1, -1},
                            {-1, -1}
                        };
                        vector<Move> moves = longRangePiece(piece, movements, 8, isWhitePiece);
                        
                        psuedoLegalMoves.insert(psuedoLegalMoves.end(), moves.begin(), moves.end());
                        break;
                    }
                    case 'k': {
                        int movesOffsets[8][2] = {
                            {1, 0}, {0, 1},
                            {-1, 0}, {0, -1},
                            {1, 1}, {-1, 1},
                            {1, -1}, {-1, -1}
                        };
                        for (int i = 0; i < 8; i++) {
                            int df = movesOffsets[i][0];
                            int dr = movesOffsets[i][1];
                            int targetRank = piece.rank + dr;
                            int targetFile = piece.file + df;
                            char target = getPieceAtSquare(targetRank, targetFile);
                            if (targetRank < 0 || targetRank > 7 || targetFile < 0 || targetFile > 7) continue;
                            
                            // Move to empty or enemy piece
                            if (target == ' ' || (isupper(target) != isWhitePiece)) {
                                psuedoLegalMoves.push_back({{piece.rank, piece.file}, {targetRank, targetFile}});
                            }
                        }
                        break;
                    }
                }
            }
            
            return psuedoLegalMoves;
        }

        void MakeMove(Move move) {
            char movingPiece = getPieceAtSquare(move.start.rank, move.start.file);
            
            state[move.end.rank][move.end.file] = movingPiece;
            state[move.start.rank][move.start.file] = ' ';

            whiteToMove = !whiteToMove;
        }

    private:
        vector<Move> longRangePiece(Position pos, int dx, int dy, bool isWhitePiece) {
            vector<Move> legalPieceMoves;
            Position startPos = pos; // original starting position of the piece
            pos.rank += dy;
            pos.file += dx;
            
            while (pos.rank >= 0 && pos.rank < 8 && pos.file >= 0 && pos.file < 8) {
                char square = getPieceAtSquare(pos.rank, pos.file);
                if (square == ' ') {
                    legalPieceMoves.push_back({startPos, pos});
                } else {
                    if (isupper(square) != isWhitePiece) {
                        legalPieceMoves.push_back({startPos, pos});
                    }
                    break;
                }
                pos.rank += dy;
                pos.file += dx;
            }

            return legalPieceMoves;
        }

        // For each movement, run the function above to reduce clutter
        vector<Move> longRangePiece(Position pos, int movements[][2], int numDirections, bool isWhitePiece) {
            vector<Move> legalPieceMoves;
            for (int i = 0; i < numDirections; i++) {
                int dx = movements[i][0];
                int dy = movements[i][1];
                vector<Move> moves = longRangePiece(pos, dx, dy, isWhitePiece);
                legalPieceMoves.insert(legalPieceMoves.end(), moves.begin(), moves.end());
            }

            return legalPieceMoves;
        }

        char getPieceAtSquare(int rank, int file) {
            if (rank > 7 || file > 7 || rank < 0 || file < 0) {
                return ' ';
            }

            return state[rank][file];
        }
};