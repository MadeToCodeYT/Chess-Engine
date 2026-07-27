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
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 15, 20, 30, 30, 20, 15, 10},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {5, 10, 10, -20, -20, 10, 10, 5},
    {0, 0, 0, 0, 0, 0, 0, 0},
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
	{0,  0,  0,  5,  5,  0,  0,  0}
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
    { 25,  30,  35,  35,  35,  35,  30,  25},
    { 30,  40,  60,  80,  80,  60,  40,  30}
};

double EvaluateBoardState(Board board) {
    double whiteMaterial = 0;
    double blackMaterial = 0;
    double locationEval = 0;

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            switch (board.state[rank][file]) {
                case 'P':
                    whiteMaterial += pawnValue;
                    locationEval += pawnLocationEval[rank][file];
                    break;
                case 'N':
                    whiteMaterial += knightValue;
                    locationEval += knightLocationEval[rank][file];
                    break;
                case 'B':
                    whiteMaterial += bishopValue;
                    locationEval += bishopLocationEval[rank][file];
                    break;
                case 'R':
                    whiteMaterial += rookValue;
                    locationEval += rookLocationEval[rank][file];
                    break;
                case 'Q':
                    whiteMaterial += queenValue;
                    locationEval += queenLocationEval[rank][file];
                    break;

                case 'p':
                    blackMaterial += pawnValue;
                    locationEval += pawnLocationEval[7 - rank][file];
                    break;
                case 'n':
                    blackMaterial += knightValue;
                    locationEval += knightLocationEval[7 - rank][file];
                    break;
                case 'b':
                    blackMaterial += bishopValue;
                    locationEval += bishopLocationEval[7 - rank][file];
                    break;
                case 'r':
                    blackMaterial += rookValue;
                    locationEval += rookLocationEval[7 - rank][file];
                    break;
                case 'q':
                    blackMaterial += queenValue;
                    locationEval += queenLocationEval[7 - rank][file];
                    break;
               
            }
        }
    }

    double materialTotal = whiteMaterial - blackMaterial;

    return materialTotal + locationEval;
}

double Search(Board& board, int depth, double alpha, double beta, bool isMaximizing) {
    if (depth == 0) {
        return EvaluateBoardState(board);
    }
    vector<Move> legalMoves = board.GetLegalMoves();
    if (legalMoves.size() == 0) {
        if (board.IsInCheck(board.whiteToMove)) {
            return board.whiteToMove ? -1e10 : 1e10; // Checkmate
        }
        return 0; // Stalemate
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
    vector<Move> moves = board.GetLegalMoves();
    if (moves.empty()) {
        // Can't return an empty move since it'll be invalid
    }
    Move bestMove = moves[0]; // Fallback initialization

    double bestValue = board.whiteToMove ? -1e10 : 1e10;
    double alpha = -1e10;
    double beta = 1e10;

    for (const auto& move : moves) {
        Board tempBoard = board;
        tempBoard.MakeMove(move);
        double evaluation = Search(tempBoard, depth - 1, alpha, beta, !board.whiteToMove);

        if (board.whiteToMove) {
            if (evaluation > bestValue) {
                bestValue = evaluation;
                bestMove = move;
            }
            alpha = max(alpha, evaluation);
        } else {
            if (evaluation < bestValue) {
                bestValue = evaluation;
                bestMove = move;
            }
            beta = min(beta, evaluation);
        }
    }
    return bestMove;
}