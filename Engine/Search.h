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

		Move findBestMove(Board& board, int depth);

	private:
		int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);

};
