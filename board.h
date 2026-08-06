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
    char promotionPiece = ' ';
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

        vector<string> boardPositions;
        
        int enPassantSquares[2][2]; // Allows these two squares to En Passant
        bool w_hasLeftRookMoved = false;
        bool w_hasRightRookMoved = false;
        bool w_hasKingMoved = false;
        bool b_hasLeftRookMoved = false;
        bool b_hasRightRookMoved = false;
        bool b_hasKingMoved = false;

        bool whiteToMove = true;

        vector<Move> GetLegalMoves() {
            vector<Move> legalMoves;
            vector<Move> pseudoLegalMoves = GetPseudoLegalMoves();

            // To determine legality, we need to check moves from the perspective BEFORE the move (whiteToMove).
            // After making the move, it is opponent's turn, so must look for the side that just moved's king.

            for (const Move &move : pseudoLegalMoves) {
                Board tempBoard = *this;

                // Disable castling if the king is in check and moves out of a castle
                if (IsInCheck(whiteToMove)
                && tolower(state[move.start.rank][move.start.file]) == 'k'
                && abs(move.start.file - move.end.file) >= 2) {
                    continue;
                }

                // NOTE: MakeMove should NOT switch turns here, otherwise the board's turn is wrong for checking king safety.
                tempBoard.MakeMove(move, false);

                // Find the king of the side who is moving (same as 'this->whiteToMove').
                char kingChar = whiteToMove ? 'K' : 'k';
                Position kingPos = {-1, -1};
                for (int rank = 0; rank < 8; rank++) {
                    for (int file = 0; file < 8; file++) {
                        if (tempBoard.state[rank][file] == kingChar) {
                            kingPos = {rank, file};
                        }
                    }
                }

                if (kingPos.rank == -1) {
                    // King missing? Illegal position.
                    continue;
                }

                // Switch turn for opponent's moves
                tempBoard.whiteToMove = !whiteToMove;

                bool kingAttacked = false;
                vector<Move> opponentMoves = tempBoard.GetPseudoLegalMoves();
                for (const Move& reply : opponentMoves) {
                    if (reply.end.rank == kingPos.rank && reply.end.file == kingPos.file) {
                        kingAttacked = true;
                        break;
                    }
                }

                if (!kingAttacked) {
                    legalMoves.push_back(move);
                }
            }

            return legalMoves;
        }

        void MakeMove(Move move, bool switchTurns=true) {  // Assuming `move` parameter is a valid move
            // Save pieces before changing state for en-passant checks
            char movingPiece = getPieceAtSquare(move.start.rank, move.start.file);
            char targetBeforeMove = getPieceAtSquare(move.end.rank, move.end.file);
            
            // Make the move
            if (move.promotionPiece != ' ') {
                // Handle bot promotions
                state[move.end.rank][move.end.file] = move.promotionPiece;
            } else {
                state[move.end.rank][move.end.file] = movingPiece;
            }
            state[move.start.rank][move.start.file] = ' ';

            // Pseudocode: If a piece of type pawn has moved two spaces across the ranks
            if (tolower(getPieceAtSquare(move.end.rank, move.end.file)) == 'p' && abs(move.start.rank - move.end.rank) == 2) {
                // Pawn moved two spaces
                enPassantSquares[0][0] = move.end.rank;
                enPassantSquares[0][1] = move.end.file-1;
                enPassantSquares[1][0] = move.end.rank;
                enPassantSquares[1][1] = move.end.file+1;
            } else {
                // Disable En Passaunt if another move has been made
                enPassantSquares[0][0] = 0;
                enPassantSquares[0][1] = 0;
                enPassantSquares[1][0] = 0;
                enPassantSquares[1][1] = 0;
            }

            // Check if En Passant to remove the captured pawn (target square was empty before move)
            if (tolower(movingPiece) == 'p' && move.start.file != move.end.file && targetBeforeMove == ' ') {
                // Captured pawn is on the same rank as the pawn moved from, and in the destination file
                state[move.start.rank][move.end.file] = ' ';
            }

            // Check if one of the rooks has moved from their start square
            if (tolower(getPieceAtSquare(move.end.rank, move.end.file)) == 'r') {
                if (move.start.rank == 0 && move.start.file == 0 && getPieceAtSquare(move.end.rank, move.end.file) == 'r') {
                    b_hasLeftRookMoved = true;
                }
                if (move.start.rank == 0 && move.start.file == 7 && getPieceAtSquare(move.end.rank, move.end.file) == 'r') {
                    b_hasRightRookMoved = true;
                }
                if (move.start.rank == 7 && move.start.file == 0 && getPieceAtSquare(move.end.rank, move.end.file) == 'R') {
                    w_hasLeftRookMoved = true;
                }
                if (move.start.rank == 7 && move.start.file == 7 && getPieceAtSquare(move.end.rank, move.end.file) == 'R') {
                    w_hasRightRookMoved = true;
                }
            }

            // Check if a rook has been captured in their starting square (disables castling on that side)
            if (targetBeforeMove == 'r') {
                if (move.end.rank == 0) {
                    if (move.end.file == 0) {
                        b_hasRightRookMoved = true;
                    }
                    if (move.end.file == 7) {
                        b_hasLeftRookMoved = true;
                    }
                } else if (move.end.rank == 7) {
                    if (move.end.file == 0) {
                        w_hasRightRookMoved = true;
                    }
                    if (move.end.file == 7) {
                        w_hasLeftRookMoved = true;
                    }
                }
            }

            // Check if king moved
            if (tolower(getPieceAtSquare(move.end.rank, move.end.file)) == 'k') {
                if (whiteToMove) {
                    w_hasKingMoved = true;
                } else {
                    b_hasKingMoved = true;
                }

                // If king moved two squares, it means they castled
                if (abs(move.start.file - move.end.file) == 2) {
                    if (whiteToMove) {
                        if (move.end.file == 6) {
                            // Castle king-side
                            state[7][5] = 'R';
                            state[7][7] = ' ';
                        }
                        if (move.end.file == 2) {
                            // Castle queen-side
                            state[7][3] = 'R';
                            state[7][0] = ' ';
                        }
                    } else {
                        if (move.end.file == 6) {
                            // Castle king-side
                            state[0][5] = 'r';
                            state[0][7] = ' ';
                        }
                        if (move.end.file == 2) {
                            // Castle queen-side
                            state[0][3] = 'r';
                            state[0][0] = ' ';
                        }
                    }
                }
            }

            // Add the combined string of the board into boardPositions
            string boardString = concatBoard();
            boardPositions.push_back(boardString);
    
            // Switch board turn
            if (switchTurns) {
                whiteToMove = !whiteToMove;
            }
        }

        bool IsThreeFoldRep() {
            // Returns true if the current position occurred three times (threefold repetition)
            int count = 0;
            string currentBoardString;
            if (!boardPositions.empty()) {
                currentBoardString = boardPositions.back();
            } else {
                // No positions recorded, cannot be repetition
                return false;
            }
            for (const string &pos : boardPositions) {
                if (pos == currentBoardString) {
                    count++;
                }
            }
            return count >= 3;
        }

        bool hasReachedPositionBefore() {
            if (boardPositions.size() < 2) return false;
            const string &currentBoardString = boardPositions.back();

            for (size_t i = 0; i + 1 < boardPositions.size(); ++i) {
                if (boardPositions[i] == currentBoardString) {
                    return true;
                }
            }
            return false;
        }

        string concatBoard() {
            string boardString;
            for (int rank = 0; rank < 8; rank++) {
                for (int file = 0; file < 8; file++) {
                    boardString += state[rank][file];
                }
            }
            return boardString;
        }

        bool IsInCheck(bool whiteSide) {
            char kingChar = whiteSide ? 'K' : 'k';
            Position kingPos = {-1, -1};
            for (int rank = 0; rank < 8; rank++) {
                for (int file = 0; file < 8; file++) {
                    if (state[rank][file] == kingChar) {
                        kingPos = {rank, file};
                    }
                }
            }
            if (kingPos.rank == -1) return false;

            bool savedWhiteToMove = whiteToMove;
            whiteToMove = !whiteSide;
            vector<Move> opponentMoves = GetPseudoLegalMoves();
            whiteToMove = savedWhiteToMove;

            for (Move &reply : opponentMoves) {
                if (reply.end.rank == kingPos.rank && reply.end.file == kingPos.file) {
                    return true;
                }
            }
            return false;
        }

        string isDraw() {
            // Check for insufficient material
            int w_pawns=0, w_knights=0, w_bishops=0, w_rooks=0, w_queens=0;
            int b_pawns=0, b_knights=0, b_bishops=0, b_rooks=0, b_queens=0;
            for (int r = 0; r < 8; r++) {
                for (int f = 0; f < 8; f++) {
                    char c = state[r][f];
                    switch (c) {
                        case 'P': w_pawns++; break;
                        case 'N': w_knights++; break;
                        case 'B': w_bishops++; break;
                        case 'R': w_rooks++; break;
                        case 'Q': w_queens++; break;
                        case 'p': b_pawns++; break;
                        case 'n': b_knights++; break;
                        case 'b': b_bishops++; break;
                        case 'r': b_rooks++; break;
                        case 'q': b_queens++; break;
                        default: break;
                    }
                }
            }

            int piecesCount = b_pawns+b_knights+b_bishops+b_rooks+b_queens + w_pawns+w_knights+w_bishops+w_rooks+w_queens + 2; // +2 for kings
            if (
                // King vs king
                (piecesCount == 2)

                // King and bishop vs king or king and knight vs king
                || (piecesCount == 3 && (
                    (w_bishops == 1 && w_knights == 0 && w_pawns == 0 && w_rooks == 0 && w_queens == 0) ||
                    (w_bishops == 0 && w_knights == 1 && w_pawns == 0 && w_rooks == 0 && w_queens == 0) ||
                    (b_bishops == 1 && b_knights == 0 && b_pawns == 0 && b_rooks == 0 && b_queens == 0) ||
                    (b_bishops == 0 && b_knights == 1 && b_pawns == 0 && b_rooks == 0 && b_queens == 0)
                ) && b_pawns == 0 && w_pawns == 0)
            ) {
                return "Draw by insufficient material.";
            }

            if (IsThreeFoldRep()) {
                return "Draw by repetition.";
            }

            // Ignore stalemate and 50-move-rule (for now)

            // Return nothing
            return "";
        }

    private:
        vector<Move> GetPseudoLegalMoves() {
            vector<Move> pseudoLegalMoves;

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
                                if (piece.rank - 1 == 0) {
                                    // Promotion moves
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}, 'Q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}, 'R'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}, 'B'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}, 'N'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file}} );
                                }
                                
                                if (getPieceAtSquare(piece.rank - 2, piece.file) == ' ' && piece.rank == 6) {
                                    // Move up 2 on first move
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 2, piece.file}} );
                                }
                            }

                            // Capture diagonally 1
                            if (!isupper( getPieceAtSquare(piece.rank - 1, piece.file - 1) ) && getPieceAtSquare(piece.rank - 1, piece.file - 1) != ' ') {
                                if (piece.rank - 1 == 0) {
                                    // Promotion captures
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}, 'Q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}, 'R'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}, 'B'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}, 'N'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file - 1}} );
                                }
                            }
                            // Capture diagonally 2
                            if (!isupper( getPieceAtSquare(piece.rank - 1, piece.file + 1) ) && getPieceAtSquare(piece.rank - 1, piece.file + 1) != ' ') {
                                if (piece.rank - 1 == 0) {
                                    // Promotion captures
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}, 'Q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}, 'R'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}, 'B'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}, 'N'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank - 1, piece.file + 1}} );
                                }
                            }

                            // En Passant
                            if (
                                (
                                    enPassantSquares[0][0] == piece.rank &&
                                    enPassantSquares[0][1] == piece.file
                                ) ||
                                (
                                    enPassantSquares[1][0] == piece.rank &&
                                    enPassantSquares[1][1] == piece.file
                                )
                            ) {
                                pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {enPassantSquares[0][0]-1, enPassantSquares[0][1]+1}} );
                            }
                        } else { // Black pawn moves
                            if (getPieceAtSquare(piece.rank + 1, piece.file) == ' ') {
                                // Move up 1 if front if empty
                                if (piece.rank + 1 == 7) {
                                    // Promotion moves
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}, 'q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}, 'r'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}, 'b'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}, 'n'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file}} );
                                }
                                
                                if (getPieceAtSquare(piece.rank + 2, piece.file) == ' ' && piece.rank == 1) {
                                    // Move up 2 on first move
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 2, piece.file}} );
                                }
                            }

                            // Capture diagonally 1
                            if (isupper( getPieceAtSquare(piece.rank + 1, piece.file + 1) ) && getPieceAtSquare(piece.rank + 1, piece.file + 1) != ' ') {
                                if (piece.rank + 1 == 7) {
                                    // Promotion captures
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}, 'q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}, 'r'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}, 'b'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}, 'n'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file + 1}} );
                                }
                            }
                            // Capture diagonally 2
                            if (isupper( getPieceAtSquare(piece.rank + 1, piece.file - 1) ) && getPieceAtSquare(piece.rank + 1, piece.file - 1) != ' ') {
                                if (piece.rank + 1 == 7) {
                                    // Promotion captures
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}, 'q'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}, 'r'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}, 'b'} );
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}, 'n'} );
                                } else {
                                    pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {piece.rank + 1, piece.file - 1}} );
                                }
                            }

                            // En Passant
                            if (
                                (
                                    enPassantSquares[0][0] == piece.rank &&
                                    enPassantSquares[0][1] == piece.file
                                ) ||
                                (
                                    enPassantSquares[1][0] == piece.rank &&
                                    enPassantSquares[1][1] == piece.file
                                )
                            ) {
                                pseudoLegalMoves.push_back( {{piece.rank, piece.file}, {enPassantSquares[0][0]+1, enPassantSquares[0][1]+1}} );
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
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {targetRank, targetFile}});
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
                        
                        pseudoLegalMoves.insert(pseudoLegalMoves.end(), moves.begin(), moves.end());
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
                        
                        pseudoLegalMoves.insert(pseudoLegalMoves.end(), moves.begin(), moves.end());
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
                        
                        pseudoLegalMoves.insert(pseudoLegalMoves.end(), moves.begin(), moves.end());
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
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {targetRank, targetFile}});
                            }
                        }
                        
                        // Check if king has castling rights both left and right
                        if (isWhitePiece) {
                            if (!w_hasLeftRookMoved && !w_hasKingMoved 
                                && getPieceAtSquare(7, 1) == ' '
                                && getPieceAtSquare(7, 2) == ' '
                                && getPieceAtSquare(7, 3) == ' '
                            ) {
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {7, 2}});
                            }
                            if (!w_hasRightRookMoved && !w_hasKingMoved 
                                && getPieceAtSquare(7, 5) == ' '
                                && getPieceAtSquare(7, 6) == ' '
                            ) {
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {7, 6}});
                            }
                        } else {
                            if (!b_hasLeftRookMoved && !b_hasKingMoved 
                                && getPieceAtSquare(0, 1) == ' '
                                && getPieceAtSquare(0, 2) == ' '
                                && getPieceAtSquare(0, 3) == ' '
                            ) {
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {0, 2}});
                            }
                            if (!b_hasRightRookMoved && !b_hasKingMoved 
                                && getPieceAtSquare(0, 5) == ' '
                                && getPieceAtSquare(0, 6) == ' '
                            ) {
                                pseudoLegalMoves.push_back({{piece.rank, piece.file}, {0, 6}});
                            }
                        }
                        break;
                    }
                }
            }
            
            return pseudoLegalMoves;
        }

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