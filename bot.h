#include "board.h"
#include <vector>
#include <algorithm>
using namespace std;

double EvaluateBoardState(Board board) {
    double whiteMaterial = 0;
    double blackMaterial = 0;

    int legalMovesCount = board.GetLegalMoves().size();
    if (legalMovesCount == 0) {
        if (board.IsInCheck(board.whiteToMove)) {
            return board.whiteToMove ? -10000 : 10000;
        }
        return 0;
    }

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            switch (board.state[rank][file]) {
                case 'P':
                    whiteMaterial += 1;
                    break;
                case 'N':
                    whiteMaterial += 3;
                    break;
                case 'B':
                    whiteMaterial += 3.5;
                    break;
                case 'R':
                    whiteMaterial += 5;
                    break;
                case 'Q':
                    whiteMaterial += 9;
                    break;
                case 'p':
                    blackMaterial += 1;
                    break;
                case 'n':
                    blackMaterial += 3;
                    break;
                case 'b':
                    blackMaterial += 3.5;
                    break;
                case 'r':
                    blackMaterial += 5;
                    break;
                case 'q':
                    blackMaterial += 9;
                    break;
            }
        }
    }

    double materialTotal = whiteMaterial - blackMaterial;

    return materialTotal;
}

double Search(Board& board, int depth, double alpha, double beta, bool isMaximizing) {
    if (depth == 0) {
        return EvaluateBoardState(board);
    }
    vector<Move> legalMoves = board.GetLegalMoves();
    if (legalMoves.size() == 0) {
        return EvaluateBoardState(board);
    }

    if (isMaximizing) {
        double maxEval = -1e9;
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
        double minEval = 1e9;
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

    double bestValue = board.whiteToMove ? -1e9 : 1e9;
    double alpha = -1e9;
    double beta = 1e9;

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