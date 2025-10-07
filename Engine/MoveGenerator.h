#include "Board.h"
#include "Move.h"
#include <vector>
#include <cstdint>

#pragma once


class MoveGenerator{
	public:
		explicit MoveGenerator(Board& board  , bool fast= true);

		void generateLegalMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth);
		void generatePseudoLegalMoves(Move (*moves)[MAX_MOVES], int& moveCount , int currentDepth) const;
    	void generateCaptures(Move (*moves)[MAX_MOVES], int& moveCount) const;
		bool isSquareAttacked(int square, PieceColor oppositeColor) const;

		bool isLegal(Move& move);
		//Initializers for lookups and magic
		static void initKnightAttacks();
		static void initKingAttacks();
		static void initSlidingAttacks();
		static void initPawnAttacks();
		

	private:
    		Board& board;
			bool fast;


			static uint64_t knightAttacks[64]; // all squares a knight can jump to
			static uint64_t kingAttacks[64];   // all squares a king can move to
			static uint64_t RookAttackTable[64][4096];
			static uint64_t BishopAttackTable[64][512];
			static uint64_t RookMasks[64];
			static uint64_t BishopMasks[64];
			static uint64_t RookMagics[64];
			static uint64_t BishopMagics[64];
			static int RookRelevantBits[64];
			static int BishopRelevantBits[64];

			static uint64_t WhitePawnPush[64];    // single-square push
			static uint64_t WhitePawnDouble[64];  // double push (only from rank 2)
			static uint64_t WhitePawnAttacks[64]; // diagonal captures

			static uint64_t BlackPawnPush[64];    // single-square push
			static uint64_t BlackPawnDouble[64];  // double push (only from rank 2)
			static uint64_t BlackPawnAttacks[64]; // diagonal captures



    // Piece-specific helpers
    		void generatePawnMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;
    		void generateKnightMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;
			void generateBishopMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;
    		void generateRookMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;
    		void generateQueenMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;
    		void generateKingMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const;

			

    		// Legality check
    		

    		// Attack utilities
    		


			static uint64_t generateRookMask(int sq);
			static uint64_t generateBishopMask(int sq);
			static  std::vector<uint64_t> generateOccupancies(uint64_t mask);
			static uint64_t rookAttacksOnTheFly(int square, uint64_t blockers);
			static uint64_t bishopAttacksOnTheFly(int square, uint64_t blockers);

			static uint64_t getRookAttacks(int square, uint64_t occupancy);
			static uint64_t getBishopAttacks(int square, uint64_t occupancy);
			static uint64_t getQueenAttacks(int square, uint64_t occupancy);
			uint64_t getPawnAttacks(int square, uint64_t combinedSame, uint64_t combinedOpposite) const;

};
