#include "Board.h"
#include <cstdint>
#include <random>
#include <iostream>
#include "MoveGenerator.h"
#include <stdexcept>

Move moves[MAX_DEPTH][MAX_MOVES];

uint64_t ZobristTable[12][64]; // 12 piece types 64 squares
uint64_t ZobristSide;          // Side to move
uint64_t ZobristCastling[16];  // Castling  rights states
uint64_t ZobristEnPassant[8];  // En passant file

int pieceIndex(PieceType type, PieceColor color) {
    int base = (color == white) ? 0 : 6;
    return base + static_cast<int>(type); // assuming Pawn=0..King=5
}

int castlingRightsAfterMove(const Move& move, int oldCastling){
	int newCastling = oldCastling;
// Moving king removes both rights for that color
    if (move.pieceType == King) {
        if (move.pieceColor == white)
            newCastling &= 0b1100; // remove white K+Q
        else
            newCastling &= 0b0011; // remove black K+Q
    }

    // Moving rooks removes relevant side
    if (move.pieceType == Rook) {
        if (move.pieceColor == white) {
            if (move.from == 0) newCastling &= 0b1110; // a1 rook, remove Q
            if (move.from == 7) newCastling &= 0b1101; // h1 rook, remove K
        } else {
            if (move.from == 56) newCastling &= 0b0111; // a8 rook, remove q
            if (move.from == 63) newCastling &= 0b1011; // h8 rook, remove k
        }
    }
	if (move.to == 0) newCastling &= 0b1110; // a1 rook, remove Q
	if (move.to == 7) newCastling &= 0b1101; // h1 rook, remove K
	if (move.to == 56) newCastling &= 0b0111; // a8 rook, remove q
	if (move.to == 63) newCastling &= 0b1011; // h8 rook, remove k
	return newCastling;
}

void Board::parseFEN(std::string FEN){
	int n = FEN.size();
	int currentX = 0, currentY = 0;
	for (int i = 0; i < n; i++) {
		char currentChar = FEN[i];
		if (currentChar == ' ') {
			i++;
			break;
		}
		else if (currentChar == '/') {
			currentY++;
			currentX = 0;
			continue;
		}
		if (isdigit(currentChar)) {
			currentX += currentChar - '0';
		}
		else {
			int oneDimensionalIndex = (56 - currentY * 8) + currentX;

			switch (currentChar) {
			case 'p': this->blackPawns |= (1ULL << ( oneDimensionalIndex)); break;
			case 'r': this->blackRooks |= (1ULL << (oneDimensionalIndex)); break;
			case 'b': this->blackBishops |= (1ULL << (oneDimensionalIndex)); break;
			case 'n': this->blackKnights |= (1ULL << (oneDimensionalIndex)); break;
			case 'k': this->blackKing |= (1ULL << (oneDimensionalIndex)); break;
			case 'q': this->blackQueens |= (1ULL << (oneDimensionalIndex)); break;
			case 'P': this->whitePawns |= (1ULL << (oneDimensionalIndex)); break;
			case 'R': this->whiteRooks |= (1ULL << (oneDimensionalIndex)); break;
			case 'B': this->whiteBishops |= (1ULL << (oneDimensionalIndex)); break;
			case 'N': this->whiteKnights |= (1ULL << (oneDimensionalIndex)); break;
			case 'K': this->whiteKing |= (1ULL << (oneDimensionalIndex)); break;
			case 'Q': this->whiteQueens |= (1ULL << (oneDimensionalIndex)); break;
			}
			currentX++;
		
	}
	}
}

Board::Board() {
	


	blackPawns = 0;
	blackRooks = 0;
	blackBishops = 0;
	blackKing = 0;
	blackQueens = 0;
	blackKnights = 0;
	
	whitePawns = 0;
	whiteKing = 0;
	whiteRooks = 0;
	whiteBishops = 0;
	whiteKnights = 0;
	whiteQueens = 0;

	whiteToMove = true;
	castlingRights = 15;

	halfMoveClock = 0;
	fullMoveNumber = 0;

	enPassantSquare = -1;

	zobristHash = 0;

	this->initZobristKeys();
	
	
	history = {};
	moveHistory = {};
}

void Board::initZobristKeys() {
	std::mt19937_64 rng(0xDEADBEEF); // fixed seed for reproducibility
	std::uniform_int_distribution<uint64_t> dist;

	// Fill piece-square table
	for (int piece = 0; piece < 12; ++piece) {
		for (int square = 0; square < 64; ++square) {
			ZobristTable[piece][square] = dist(rng);
		}
	}

	// Side to move
	ZobristSide = dist(rng);

	// Castling rights (16 possibilities)
	for (int i = 0; i < 16; ++i) {
		ZobristCastling[i] = dist(rng);
	}

	// En passant files
	for (int file = 0; file < 8; ++file) {
		ZobristEnPassant[file] = dist(rng);
	}
}

void Board::makeMove(const Move& move){
	moveHistory.push_back(move);

	BoardState state(this->castlingRights,this->enPassantSquare, this->halfMoveClock, this->
	zobristHash, move.pieceEatenType);
	this->history.push_back(state);
	
	this->updateZobrist(move);

	//Update boards
	uint64_t* currBoard = this->getBoardOfType(move.pieceType, move.pieceColor); 

	//Delete old position
	*currBoard &= ~(1ULL << (move.from));

	//add new Position 
	*currBoard |= (1ULL << (move.to));


	//Update pieceEaten
	if (move.pieceEatenType != None){
		uint64_t* pieceEatenBoard = this->getBoardOfType(move.pieceEatenType, move.pieceColor == white ? black : white); 
		*pieceEatenBoard &= ~(1ULL << (move.to));
	}	

	//Update en passant capture
	if(move.isEnPassant){
		//Delete corresponding piece
		uint64_t* pieceEatenBoard = this->getBoardOfType(Pawn, move.pieceColor == white ? black : white); 
		if (move.pieceColor == white){
			*pieceEatenBoard &= ~(1ULL << (move.to - 8));
		}
		else{
			*pieceEatenBoard &= ~(1ULL << (move.to + 8));
		}
	}

	//Update new en passant square
	if (move.pieceType == Pawn && std::abs(move.from - move.to) == 16){
		this->enPassantSquare = move.to + (move.pieceColor == white ? -8 : 8);
	}
	else{
		this->enPassantSquare = -1;
	}

	if (move.promotionPiece != None){
		*currBoard &= ~(1ULL << (move.to));

		uint64_t* promotionBoard = this->getBoardOfType(move.promotionPiece,move.pieceColor);
		*promotionBoard |= (1ULL << (move.to)); 
	}



	//Update castling rights
	if (move.pieceType == King){
		if (move.pieceColor == white){
			this->castlingRights &= ~(1ULL << (0));
			this->castlingRights &= ~(1ULL << (1));
		}
		else{
			this->castlingRights &= ~(1ULL << (2));
			this->castlingRights &= ~(1ULL << (3));
		}
	}
	if(move.from == 0 || move.to == 0){this->castlingRights &= ~(1ULL << (1));}
	else if(move.from == 7|| move.to == 7){this->castlingRights &= ~(1ULL << (0));}
	else if(move.from == 56 || move.to == 56){this->castlingRights &= ~(1ULL << (3));}
	else if(move.from == 63 || move.to == 63){this->castlingRights &= ~(1ULL << (2));}

	//Apply castling move
	if(move.pieceType == King && std::abs(move.from-move.to) == 2){
		//Move appropiate rook
		uint64_t* rookBoard = this->getBoardOfType(Rook,move.pieceColor);

		if (move.to == 2){
			*rookBoard &= ~(1ULL << (0));
			*rookBoard |= (1ULL << (3));
		}
		else if(move.to == 6){
			*rookBoard &= ~(1ULL << (7));
			*rookBoard |= (1ULL << (5));
		}
		else if(move.to == 58){
			*rookBoard &= ~(1ULL << (56));
			*rookBoard |= (1ULL << (59));
		}
		else if(move.to == 62){
			*rookBoard &= ~(1ULL << (63));
			*rookBoard |= (1ULL << (61));
		}
	}
	if(move.pieceColor == black){
		this->fullMoveNumber += 1;
	}

	if (move.pieceEatenType != None || move.pieceType == Pawn){
		this->halfMoveClock = 0;
	}
	else{
		this->halfMoveClock += 1;
	}

	this->whiteToMove = !this->whiteToMove;

}

void Board::unmakeMove(const Move& move){
	moveHistory.pop_back();
	//Pop history back
	if (this->history.size() == 0){
		throw std::invalid_argument("history is empty");
	}

	BoardState state = this->history[this->history.size() - 1];

	this->history.pop_back(); 
	
	

	//Update boards
	uint64_t* currBoard = this->getBoardOfType(move.pieceType, move.pieceColor); 

	//Delete old position
	*currBoard |= (1ULL << (move.from));

	//add new Position 
	*currBoard &= ~(1ULL << (move.to));


	//Update pieceEaten
	if (move.pieceEatenType != None){
		uint64_t* pieceEatenBoard = this->getBoardOfType(move.pieceEatenType, move.pieceColor == white ? black : white); 
		*pieceEatenBoard |= (1ULL << (move.to));
	}	

	//Update en passant capture
	if(move.isEnPassant){
		//Delete corresponding piece
		uint64_t* pieceEatenBoard = this->getBoardOfType(Pawn, move.pieceColor == white ? black : white); 
		if (move.pieceColor == white){
			*pieceEatenBoard |= (1ULL << (move.to - 8));
		}
		else{
			*pieceEatenBoard |= (1ULL << (move.to + 8));
		}
	}

	//Update new en passant square
	this->enPassantSquare = state.enPassantSquare;


	if (move.promotionPiece != None){
		uint64_t* promotionBoard = this->getBoardOfType(move.promotionPiece,move.pieceColor);
		*promotionBoard &= ~ (1ULL << (move.to)); 
	}



	if(move.pieceType == King && std::abs(move.from-move.to) == 2){
		//Move appropiate rook
		uint64_t* rookBoard = this->getBoardOfType(Rook,move.pieceColor);

		if (move.to == 2){
			*rookBoard |=(1ULL << (0));
			*rookBoard &= ~ (1ULL << (3));
		}
		else if(move.to == 6){
			*rookBoard |=(1ULL << (7));
			*rookBoard &= ~(1ULL << (5));
		}
		else if(move.to == 58){
			*rookBoard |=(1ULL << (56));
			*rookBoard &= ~(1ULL << (59));
		}
		else if(move.to == 62){
			*rookBoard |= (1ULL << (63));
			*rookBoard &= ~(1ULL << (61));
		}
	}
	this->castlingRights = state.castlingRights;

	if(move.pieceColor == black){
		this->fullMoveNumber -= 1;
	}

	this->halfMoveClock = state.halfMoveClock;

	this->whiteToMove = !this->whiteToMove;

	this->updateZobrist(move);

	if (this->zobristHash != state.zobristHash){
		throw std::invalid_argument("error with zobrist hash");
	}
}

uint64_t* Board::getBoardOfType(PieceType type, PieceColor color){
	uint64_t* ans; 
	if (type == Pawn){
		ans =  color == white ? &this->whitePawns : &this->blackPawns;
	} 
	else if (type == Knight){
		ans =  color == white ? &this->whiteKnights : &this->blackKnights;
	} 
	else if (type == Bishop){
		ans =  color == white ? &this->whiteBishops : &this->blackBishops;
	} 
	else if (type == Rook){
		ans =  color == white ? &this->whiteRooks : &this->blackRooks;
	} 
	else if (type == Queen){
		ans =  color == white ? &this->whiteQueens : &this->blackQueens;
	} 
	else if (type == King){
		ans =  color == white ? &this->whiteKing : &this->blackKing;
	} 
	else{
		throw std::invalid_argument("type must have a value, cannot be none");
	}
	return ans;
}

//NOTE: HAS TO BE CALLED BEFORE EXECUTING MOVE
void Board::updateZobrist(const Move& move) {

    int idx = pieceIndex(move.pieceType, move.pieceColor);
	
    zobristHash ^= ZobristTable[idx][move.from];

    zobristHash ^= ZobristTable[idx][move.to];

    if (move.pieceEatenType != None) {
        PieceColor enemyColor = (move.pieceColor == white) ? black : white;
        int capIdx = pieceIndex(move.pieceEatenType, enemyColor);
        zobristHash ^= ZobristTable[capIdx][move.to];
    }

    if (move.promotionPiece != None) {
        // Remove pawn from destination
        int pawnIdx = pieceIndex(Pawn, move.pieceColor);
        zobristHash ^= ZobristTable[pawnIdx][move.to];

        // Add promoted piece
        int promoIdx = pieceIndex(move.promotionPiece, move.pieceColor);
        zobristHash ^= ZobristTable[promoIdx][move.to];
    }

    // 5️⃣ En passant
    if (move.isEnPassant) {
        int epSquare = (move.pieceColor == white) ? move.to - 8 : move.to + 8;
        int epPawnIdx = pieceIndex(Pawn, (move.pieceColor == white) ? black : white);
        zobristHash ^= ZobristTable[epPawnIdx][epSquare];
    }


	int newCastling = castlingRightsAfterMove(move,this->castlingRights);

    

	zobristHash ^= ZobristCastling[this->castlingRights];
	zobristHash ^= ZobristCastling[newCastling];


    // 6️⃣ Side to move
    zobristHash ^= ZobristSide;
}

void Board::setStartingPosition() {
	whiteToMove = true;

	//setting up board (LITTLE ENDIAN)
	whitePawns = 0x000000000000FF00;
	whiteRooks = 0x0000000000000081;
	whiteKnights = 0x0000000000000042;
	whiteBishops = 0x0000000000000024;
	whiteQueens = 0x0000000000000008;
	whiteKing = 0x0000000000000010;
	
	blackPawns = 0x00FF000000000000;
	blackRooks = 0x8100000000000000;
	blackBishops = 0x2400000000000000;
	blackKnights = 0x4200000000000000;
	blackQueens = 0x0800000000000000;
	blackKing = 0x1000000000000000;

	whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
	blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
	allPieces = whitePieces | blackPieces;

}

void Board::setTestingPosition(){

    whiteToMove = true;

    whitePawns   = 0x000000001C003B00ULL;
    whiteKnights = 0x0000000000240000ULL;
    whiteBishops = 0x0000000000001004ULL;
    whiteRooks   = 0x0000000000000021ULL;
    whiteQueens  = 0x0000000000000008ULL;
    whiteKing    = 0x0000000000000040ULL;

    blackPawns   = 0x00D8200818000000ULL;
    blackKnights = 0x0220000002000000ULL;
    blackBishops = 0x0440000000000000ULL;
    blackRooks   = 0x8200000000000000ULL;
    blackQueens  = 0x0800000000000000ULL;
    blackKing    = 0x4000000000000000ULL;

    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    allPieces   = whitePieces | blackPieces;

}

//helper functions
char Board::getLetterOfPieceType(PieceType type) {
	if (type == None) { return '.'; }
	if (type == Knight){return 'N';}
	return pieceTypeNames[type][0];
}

uint64_t Board::getCombinedBoard(PieceColor color) const{
	if (color == white){
		return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
	}
	else{
		return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
	}
}

int Board::getKingPosition(PieceColor color) const{
	uint64_t kingBoard = color == white ? this->whiteKing : this->blackKing;
    
    while (kingBoard){
        int targetSquare = __builtin_ctzll(kingBoard);
		return targetSquare;
	}
	throw std::invalid_argument("No king position found");
	return -1;
}

int Board::countMoves(int depth){
	if (depth == 0){return 1;}

	MoveGenerator gen(*this,true);
	int moveCount = 0;
	gen.generateLegalMoves(moves,moveCount,depth);
	if (moveCount == 0){return 0;}
	int c = 0;

	for (int i = 0; i < moveCount; i++){
		this->makeMove(moves[depth][i]);
		c+= countMoves(depth - 1);
		this->unmakeMove(moves[depth][i]);
	}

	return c;
}

//using little endian, bit 0 is a1
std::pair<PieceType, PieceColor> Board::getPieceTypeAtBit( int bit) const {
	uint64_t mask = 1ULL << bit;
	if ((blackPawns & mask) != 0) {
		return { Pawn,black };
	}
	if ((blackRooks & mask) != 0) {
		return { Rook,black };
	}
	if ((blackQueens & mask) != 0) {
		return { Queen,black };
	}
	if ((blackBishops & mask) != 0) {
		return { Bishop,black };
	}
	if ((blackKnights & mask) != 0) {
		return { Knight,black };
	}
	if ((blackKing & mask) != 0) {
		return { King,black };
	}
	if ((whitePawns & mask) != 0) {
		return { Pawn,white };
	}
	if ((whiteRooks & mask) != 0) {
		return { Rook,white };
	}
	if ((whiteQueens & mask) != 0) {
		return { Queen,white };
	}
	if ((whiteBishops & mask) != 0) {
		return { Bishop,white };
	}
	if ((whiteKnights & mask) != 0) {
		return { Knight,white };
	}
	if ((whiteKing & mask) != 0) {
		return { King,white };
	}

	return { None,white };
}