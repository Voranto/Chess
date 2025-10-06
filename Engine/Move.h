#pragma once
#include "PieceType.h"
#include "pieceColor.h"
#include <tuple>
#include <iostream>
#include <optional>

class Move
{

public:
	PieceType pieceType;
	PieceColor pieceColor;

	int from;
    int to;

    PieceType promotionPiece;
    PieceType pieceEatenType;

    bool isEnPassant;

    Move(PieceType type, PieceColor color, int from, int to, PieceType promotionPiece = None,
        PieceType pieceEatenType = None, bool isEnPassant = false);
    Move();
    std::string toString() const {
        // Convert to algebraic notation like "e2e4" or "e7e8Q"
        std::string s = "";
        s += squareToString(this->from);
        s += squareToString(this->to);
        if (promotionPiece != None){s+= ':';s+= pieceTypeNames[promotionPiece];}
        return s;
    }
    static int stringToSquare(std::string square) {
            return (square[0] - 'a') + 8 * (square[1] - '1');
        }
    static std::string squareToString(int square)  {
            char file = 'a' + (square % 8);
            char rank = '1' + (square / 8);
            return { file, rank };
        }
    
    bool operator==(const Move& other) const {
		return this->pieceType == other.pieceType &&
                this->pieceColor == other.pieceColor && 
                this->from == other.from && 
                this->to == other.to &&
                this->pieceEatenType == other.pieceEatenType&& 
                this->promotionPiece == other.promotionPiece && 
                this->isEnPassant == other.isEnPassant;
	}
	bool operator!=(const Move& other) const {
        return !(*this == other);
    }
};

