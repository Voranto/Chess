#include <vector>
#include <iostream>
#include "Move.h"
#include <cstdint>
#include <bitset>
#include <iomanip>
#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "Move.h"
#include "TTEntry.h"

constexpr int MAX_DEPTH = 14;
constexpr int MAX_MOVES = 218;

extern Move moves[MAX_DEPTH][MAX_MOVES];  // just a declaration



#endif



struct BoardState {
	int castlingRights;
	int enPassantSquare;
	int halfMoveClock;
	uint64_t zobristHash;
	PieceType capturedPiece;
	BoardState(int castle, int passant, int halfMove, uint64_t zobrish, PieceType captured) :
	 castlingRights(castle), enPassantSquare(passant), halfMoveClock(halfMove), 
	 zobristHash(zobrish), capturedPiece(captured){

	 }
};

class Board{
public:
	uint64_t blackPawns, blackRooks, blackBishops, blackKnights, blackQueens, blackKing;

	uint64_t whitePawns, whiteRooks, whiteBishops, whiteKnights, whiteQueens, whiteKing;
	
	//aggregate bitboards
	uint64_t whitePieces;
	uint64_t blackPieces;
	uint64_t allPieces;

	//GameState
	bool whiteToMove;
	//1 = WK, 2 = WQ, 4 = BK, 8 = BQ
	int castlingRights;
	int halfMoveClock;
	int fullMoveNumber;
	int enPassantSquare;

	uint64_t zobristHash;

	std::vector<BoardState> history;
	
	std::vector<Move> moveHistory;

	Board();

	void setStartingPosition();
	void setTestingPosition();

	void makeMove(const Move& move);
	void unmakeMove(const Move& move);

	int getKingPosition(PieceColor color) const;
	uint64_t getCombinedBoard(PieceColor color) const;

	std::pair<PieceType, PieceColor> getPieceTypeAtBit(int bit) const;
	char getLetterOfPieceType(PieceType type) const;


	void print() const {
		std::cout << "   +-----------------+\n";
		for (int i = 0; i < 8; i++) {
			std::cout << 8 - i << "  | ";  // rank label
			for (int j = 7; j >= 0; j--) {
				int bit = 63 - (i * 8 + j);
				std::pair<PieceType,PieceColor> piece = this->getPieceTypeAtBit(bit);
				char c = getLetterOfPieceType(piece.first);
				
				if (piece.first == None) {
					std::cout << ". ";
				} else if (piece.second == white) {
					std::cout << c << " ";
				} else {
					std::cout << static_cast<char>(std::tolower(c)) << " ";
				}
				} 
			std::cout << "|\n";
		}
		std::cout << "   +-----------------+\n";
		std::cout << "     a b c d e f g h\n\n";
		std::cout << "To move: "<< (whiteToMove ? "white" : "black") << "\n";
		std::cout << "Halfmove:  " << halfMoveClock << "\n";
		std::cout << "Fullmove:  " << fullMoveNumber << "\n";
		std::cout << "Castling:  " << std::bitset<4>(castlingRights) << "\n";
		std::cout << "EnPassant: " << enPassantSquare << "\n";
	};

	void parseFEN(std::string FEN);

	int countMoves(int depth);


	void initZobristKeys();
	void updateZobrist(const Move& move);

	uint64_t* getBoardOfType(PieceType type, PieceColor color);


	bool operator==(const Board& other) const {
		return whitePawns   == other.whitePawns &&
			whiteKnights == other.whiteKnights &&
			whiteBishops == other.whiteBishops &&
			whiteRooks   == other.whiteRooks &&
			whiteQueens  == other.whiteQueens &&
			whiteKing    == other.whiteKing &&
			blackPawns   == other.blackPawns &&
			blackKnights == other.blackKnights &&
			blackBishops == other.blackBishops &&
			blackRooks   == other.blackRooks &&
			blackQueens  == other.blackQueens &&
			blackKing    == other.blackKing &&
			castlingRights == other.castlingRights &&
			enPassantSquare == other.enPassantSquare &&
			whiteToMove == other.whiteToMove;
	}
	bool operator!=(const Board& other) const {
    return !(*this == other);
}
};


