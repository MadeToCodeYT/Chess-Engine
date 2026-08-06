/*
This was inspired by Sebastian Lague's video on chess bots (https://www.youtube.com/watch?v=U4ogK0MIzqk).
Some parts of the engine will be near identical to his version but I have added some of my own changes.
*/

#include "board.h"
#include <vector>
#include <algorithm>

using namespace std;

const int pawnValue = 100;
const int knightValue = 300;
const int bishopValue = 320;
const int rookValue = 500;
const int queenValue = 900;

double pawnLocationEval[8][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0 },
    { 50,  50,  50,  50,  50,  50,  50,  50 },
    { 10,  15,  20,  30,  30,  20,  15,  10 },
    {  5,   5,  10,  27,  27,  10,   5,   5 },
    {  0,   0,   0,  40,  40,   0,   0,   0 },
    {  5,  -5, -10,   0,   0, -10,  -5,   5 },
    {  5,  10,  10, -20, -20,  10,  10,   5 },
    {  0,   0,   0,   0,   0,   0,   0,   0 }
};

double knightLocationEval[8][8] = {
    {-30, -28, -20, -18, -18, -20, -28, -30},
    {-28, -16,  -5,  -2,  -2,  -5, -16, -28},
    {-20,  -5,  10,  14,  14,  10,  -5, -20},
    {-18,   0,  14,  22,  22,  14,   0, -18},
    {-18,   2,  14,  22,  22,  14,   2, -18},
    {-20,   5,  10,  14,  14,  10,   5, -20},
    {-28, -15,  -2,   3,   3,  -2, -15, -28},
    {-30, -28, -20, -18, -18, -20, -28, -30}
};

double bishopLocationEval[8][8] = {
    {-14, -5, -5, -8, -8, -5, -5, -14},
    {-5,   4,  0,  0,  0,  0,  4,  -5},
    {-8,   0, 10, 14, 14, 10,  0,  -8},
    {-8,   8, 14, 18, 18, 14,  8,  -8},
    {-8,   0, 14, 18, 18, 14,  0,  -8},
    {-8, 10, 14, 14, 14, 14, 10,  -8},
    {-5,   8,  0,  0,  0,  0,  8,  -5},
    {-14, -5, -8, -8, -8, -8, -5, -14}
};

double rookLocationEval[8][8] = {
	{0,  0,  0,  0,  0,  0,  0,  0},
	{5, 10, 10, 10, 10, 10, 10,  5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{0,  0,  0,  7,  7,  0,  0,  0}
};

double queenLocationEval[8][8] = {
    {-18, -2, -2, -1, -1, -2, -2, -18},
    { -5,  0,  0,  1,  1,  0,  0,  -5},
    { -8,  0,  6,  7,  7,  6,  0,  -8},
    { -1,  1,  8, 10, 10,  8,  1,  -1},
    {  0,  2,  8, 10, 10,  8,  2,   0},
    { -8,  6,  7,  7,  7,  7,  6,  -8},
    { -5,  0,  4,  0,  0,  4,  0,  -5},
    {-18, -2, -2, -1, -1, -2, -2, -18}
};

double kingLocationEval[8][8] = {
    {-70, -65, -60, -50, -50, -60, -65, -70},
    {-45, -40, -35, -28, -28, -35, -40, -45},
    {-30, -20, -15, -10, -10, -15, -20, -30},
    {-10,   0,   5,  10,  10,   5,   0, -10},
    {  7,  15,  15,  20,  20,  15,  15,   7},
    { 15,  25,  30,  30,  30,  30,  25,  15},
    { 25,  30,  20,  20,  20,  20,  30,  25},
    { 30,  40,  60,  80,  80,  40,  60,  30}
};

double kingLocationEval_EndGame[8][8] = {
    {  0,   5,  10,  15,  15,  10,   5,  0 },
    {  5,  10,  20,  25,  25,  20,  10,  5 },
    { 10,  20,  30,  35,  35,  30,  20, 10 },
    { 15,  25,  35,  40,  40,  35,  25, 15 },
    { 15,  25,  35,  40,  40,  35,  25, 15 },
    { 10,  20,  30,  35,  35,  30,  20, 10 },
    {  5,  10,  20,  25,  25,  20,  10,  5 },
    {  0,   5,  10,  15,  15,  10,   5,  0 }
};

vector<vector<Move>> openings = {
    // Italian Game
    {
        { {6, 4}, {4, 4} }, // e4
        { {1, 4}, {3, 4} }, // e5
        { {7, 6}, {5, 5} }, // Nf3
        { {0, 1}, {2, 2} }, // Nc6
        { {7, 5}, {4, 2} }  // Bc4
    },

    // Ruy Lopez (Spanish Opening)
    {
        { {6, 4}, {4, 4} }, // e4
        { {1, 4}, {3, 4} }, // e5
        { {7, 6}, {5, 5} }, // Nf3
        { {0, 1}, {2, 2} }, // Nc6
        { {7, 5}, {3, 1} }  // Bc5
    },

    // Petrov's Defense
    {
        { {6, 4}, {4, 4} }, // e4
        { {1, 4}, {3, 4} }, // e5
        { {7, 6}, {5, 5} }, // Nf3
        { {0, 6}, {2, 5} }  // Nf6
    },
};


double EvalKingNearEdge(char state[8][8], int friendlyKing[2], int enemyKing[2], int endgameWeight) {
    int evaluation = 0;

    // Favour positions where the enemy king is farther away from the center
    int enemyDistanceToCenterFile = max(3 - enemyKing[1], enemyKing[1] - 4);
    int enemyDistanceToCenterRank = max(3 - enemyKing[0], enemyKing[0] - 4);
    evaluation += enemyDistanceToCenterFile + enemyDistanceToCenterRank;

    // Favour moving the king closer to the opponent king to cutoff their squares
    int distanceBetweenKingsFile = abs(friendlyKing[1] - enemyKing[1]);
    int distanceBetweenKingsRank = abs(friendlyKing[0] - enemyKing[0]);
    evaluation += 14 - (distanceBetweenKingsFile + distanceBetweenKingsRank);

    // Remove squares around the enemy king to make checkmating easier (subtract 8 if surrounded on all sides)
    int avaliableSquares = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int df = -1; df <= 1; df++) {
            if (dr == 0 && df == 0) continue;
            int r = enemyKing[0] + dr;
            int f = enemyKing[1] + df;
            if (r >= 0 && r < 8 && f >= 0 && f < 8) {
                avaliableSquares++;
            }
        }
    }
    evaluation += 10 * (8 - avaliableSquares);

    return evaluation * 10 * endgameWeight;
}

double EvaluateBoardState(Board board) {
    double whiteMaterial = 0;
    double blackMaterial = 0;
    double locationEval = 0;
    double endGameEval = 0;

    int b_kingPos[2];
    int w_kingPos[2];
    int piecesCount = 2;

    if (board.IsThreeFoldRep() || board.hasReachedPositionBefore()) {
        return 0;
    }

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            switch (board.state[rank][file]) {
                case 'P':
                    whiteMaterial += pawnValue;
                    locationEval += pawnLocationEval[rank][file];
                    piecesCount++;
                    break;
                case 'N':
                    whiteMaterial += knightValue;
                    locationEval += knightLocationEval[rank][file];
                    piecesCount++;
                    break;
                case 'B':
                    whiteMaterial += bishopValue;
                    locationEval += bishopLocationEval[rank][file];
                    piecesCount++;
                    break;
                case 'R':
                    whiteMaterial += rookValue;
                    locationEval += rookLocationEval[rank][file];
                    piecesCount++;
                    break;
                case 'Q':
                    whiteMaterial += queenValue;
                    locationEval += queenLocationEval[rank][file];
                    piecesCount++;
                    break;
                case 'K':
                    w_kingPos[0] = rank;
                    w_kingPos[1] = file;
                    break;

                case 'p':
                    blackMaterial += pawnValue;
                    locationEval += pawnLocationEval[7 - rank][file];
                    piecesCount++;
                    break;
                case 'n':
                    blackMaterial += knightValue;
                    locationEval += knightLocationEval[7 - rank][file];
                    piecesCount++;
                    break;
                case 'b':
                    blackMaterial += bishopValue;
                    locationEval += bishopLocationEval[7 - rank][file];
                    piecesCount++;
                    break;
                case 'r':
                    blackMaterial += rookValue;
                    locationEval += rookLocationEval[7 - rank][file];
                    piecesCount++;
                    break;
                case 'q':
                    blackMaterial += queenValue;
                    locationEval += queenLocationEval[7 - rank][file];
                    piecesCount++;
                    break;
                case 'k':
                    b_kingPos[0] = rank;
                    b_kingPos[1] = file;
                    break;
            }
        }
    }

    if (board.isDraw() != "") {
        return 0;
    }

    double materialDiff = whiteMaterial - blackMaterial;

    // Endgame factor is between 0 and 1, where 1 is full-on endgame and 0 is opening
    double endGameFactor = min(1.0, max(0.0, (16.0 - piecesCount) / 12.0));

    // Add king location eval (murges opening/middle game with endgame eval)
    // Blend king square tables between opening and endgame
    locationEval += (1 - endGameFactor) * kingLocationEval[w_kingPos[0]][w_kingPos[1]]
                    + endGameFactor * kingLocationEval_EndGame[w_kingPos[0]][w_kingPos[1]];
    locationEval += (1 - endGameFactor) * kingLocationEval[7 - b_kingPos[0]][b_kingPos[1]]
                    + endGameFactor * kingLocationEval_EndGame[7 - b_kingPos[0]][b_kingPos[1]];

    endGameEval = EvalKingNearEdge(board.state, w_kingPos, b_kingPos, endGameFactor)
                - EvalKingNearEdge(board.state, b_kingPos, w_kingPos, endGameFactor);

    return materialDiff + locationEval + (endGameEval * endGameFactor);
}

double GetPieceValue(char piece) {
    switch ((char)tolower(piece)) {
        case 'p': return pawnValue;
        case 'n': return knightValue;
        case 'b': return bishopValue;
        case 'r': return rookValue;
        case 'q': return queenValue;
        default:  return 0.0;
    }
}
vector<Move> OrderMoves(const Board& board, const vector<Move>& moves) {
    vector<pair<int, Move>> scoredMoves;
    scoredMoves.reserve(moves.size());

    for (const Move& move : moves) {
        int score = 0;

        // Prioritize capturing an opponent's higher ranked piece
        if (board.state[move.end.rank][move.end.file] != ' ') {
            int movedPieceVal = GetPieceValue(board.state[move.start.rank][move.start.file]);
            int capturedPieceVal = GetPieceValue(board.state[move.end.rank][move.end.file]);

            score += 15 * capturedPieceVal - movedPieceVal;
        }

        // Promotion is likely a good move so let it be checked first
        if (move.promotionPiece != ' ') {
            score += 20 * GetPieceValue(move.promotionPiece);
        }

        scoredMoves.emplace_back(score, move);
    }

    sort(scoredMoves.begin(), scoredMoves.end(),
        [](const pair<int, Move>& a, const pair<int, Move>& b) {
            return a.first > b.first; // Sorts by score in descending order
        });

    vector<Move> orderedMoves;
    orderedMoves.reserve(scoredMoves.size());
    for (const auto& scored : scoredMoves) {
        orderedMoves.push_back(scored.second);
    }

    return orderedMoves;
}

double Search(Board& board, int depth, double alpha, double beta, bool isMaximizing) {
    if (board.IsThreeFoldRep() || board.hasReachedPositionBefore()) {
        return 0; // Draw by repetition or repeated position history
    }

    vector<Move> legalMoves = board.GetLegalMoves();
    legalMoves = OrderMoves(board, legalMoves); // Use OrderMoves here

    if (legalMoves.empty()) {
        if (board.IsInCheck(board.whiteToMove)) {
            double mateScore = 1e10 - (100.0 - depth);
            return board.whiteToMove ? -mateScore : mateScore; // Checkmate with depth preference
        }
        return 0; // Stalemate
    }

    if (depth == 0) {
        return EvaluateBoardState(board);
    }

    if (isMaximizing) {
        double maxEval = -1e10;
        for (const Move& move : legalMoves) {
            Board tempBoard = board;
            tempBoard.MakeMove(move);
            double evaluation = Search(tempBoard, depth - 1, alpha, beta, false);

            maxEval = max(maxEval, evaluation);
            alpha = max(alpha, evaluation);
            if (beta <= alpha)
                break;
        }

        return maxEval;
    } else {
        double minEval = 1e10;
        for (const Move& move : legalMoves) {
            Board tempBoard = board;
            tempBoard.MakeMove(move);
            double evaluation = Search(tempBoard, depth - 1, alpha, beta, true);

            minEval = min(minEval, evaluation);
            beta = min(beta, evaluation);
            if (beta <= alpha)
                break;
        }

        return minEval;
    }
}

Move FindBestMove(Board& board, int depth) {
    // Collect all possible next moves from openings that contain the current board state
    vector<Move> candidateMoves;
    for (const vector<Move>& opening : openings) {
        Board openingBoard;

        for (size_t i = 0; i < opening.size(); ++i) {
            openingBoard.MakeMove(opening[i]);

            if (openingBoard.concatBoard() == board.concatBoard()) {
                if (i + 1 < opening.size()) {
                    candidateMoves.push_back(opening[i + 1]);
                }

                break;
            }
        }
    }

    // If multiple candidate opening moves exist, pick one at random
    if (!candidateMoves.empty()) {
        size_t idx = static_cast<size_t>(rand()) % candidateMoves.size();
        return candidateMoves[idx];
    }

    vector<Move> moves = board.GetLegalMoves();
    moves = OrderMoves(board, moves); // Use OrderMoves here

    if (moves.empty()) {
        return Move();
    }
    Move bestMove = moves[0]; // Fallback initialization

    double bestValue = board.whiteToMove ? -1e10 : 1e10;
    double alpha = -1e10;
    double beta = 1e10;

    for (const Move move : moves) {
        Board tempBoard = board;
        tempBoard.MakeMove(move);
        double evaluation = Search(tempBoard, depth - 1, alpha, beta, !board.whiteToMove);

        if (board.whiteToMove) {
            if (evaluation > bestValue) {
                bestValue = evaluation;
                bestMove = move;
            }
            alpha = max(alpha, evaluation);
            if (evaluation >= 1e10) { // Early exit on checkmate for white
                break;
            }
        } else {
            if (evaluation < bestValue) {
                bestValue = evaluation;
                bestMove = move;
            }
            beta = min(beta, evaluation);
            if (evaluation <= -1e10) { // Early exit on checkmate for black
                break;
            }
        }
    }

    return bestMove;
}