#include "Move.h"
#include "Board.h"
#include "MoveTree.h"
#include "../Engine/MoveGenerator.h"
#pragma once

Move parseAlgebraic(std::string notation, Board board);

std::string parseAlgebraic(Move mv, Board board);

class Search{
	public: 
		static MoveTree openingTree;
		static void initOpeningTreeCSV();
		static void initOpeningTreeTXT();
		Search();

		Move findBestMove(Board& board, int depth, bool printEvals = false , bool startingPos = true);
		Move findBestMoveIterative(Board& board, bool printEvals = false , bool startingPos = true);
		void debugAlphaBeta(Board& board, int depth, int alpha, int beta);
	private:
		int alphaBeta(Board& board, int depth, int alpha, int beta);

};
