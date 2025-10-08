#include "Board.h"
#include "Search.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Search.h"
#include "Evaluator.h"
#include <immintrin.h>


constexpr int PawnTable[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     50,  50,  50,  50,  50,  50,  50,  50,
     10,  10,  20,  30,  30,  20,  10,  10,
      5,   5,  10,  25,  25,  10,   5,   5,
      0,   0,   0,  20,  20,   0,   0,   0,
      5,  -5, -10,   0,   0, -10,  -5,   5,
      5,  10,  10, -20, -20,  10,  10,   5,
      0,   0,   0,   0,   0,   0,   0,   0
};

constexpr int KnightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr int BishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

constexpr int RookTable[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

constexpr int QueenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr int KingTable[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};


inline int mirrorSquare(int sq) {
    return sq ^ 56; // flips rank
}

int Evaluator::evaluate(const Board& board){

    int score = 0;
    int whiteBishops = _mm_popcnt_u64(board.whiteBishops);
    int blackBishops = _mm_popcnt_u64(board.blackBishops);
    if (whiteBishops >= 2) score += 30;
    if (blackBishops >= 2) score -= 30;
    score += (_mm_popcnt_u64(board.whitePawns) - _mm_popcnt_u64(board.blackPawns)) * PIECE_VALUES[0];
    score += (_mm_popcnt_u64(board.whiteKnights) - _mm_popcnt_u64(board.blackKnights)) * PIECE_VALUES[1];
    score += (whiteBishops - blackBishops) * PIECE_VALUES[2];
    score += (_mm_popcnt_u64(board.whiteRooks) - _mm_popcnt_u64(board.blackRooks)) * PIECE_VALUES[3];
    score += (_mm_popcnt_u64(board.whiteQueens) - _mm_popcnt_u64(board.blackQueens)) * PIECE_VALUES[4];
    score += (_mm_popcnt_u64(board.whiteKing) - _mm_popcnt_u64(board.blackKing)) * PIECE_VALUES[5];
    if (board.whiteToMove){
        score += KingTable[board.getKingPosition(board.whiteToMove ? white : black)];
    }
    else{
        score += KingTable[mirrorSquare(board.getKingPosition(board.whiteToMove ? white : black))];
    }
    
    score += (board.whiteToMove ? 10 : -10);
    // Perspective: positive means white is better
    return score;

}