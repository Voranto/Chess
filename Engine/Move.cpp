#include "Move.h"
#include "pieceColor.h"
#include "PieceType.h"

#include "Move.h"

Move::Move(PieceType type, PieceColor color, int from, int to,
           PieceType promotionPiece, PieceType pieceEatenType, bool isEnPassant)
    : pieceType(type), pieceColor(color), from(from), to(to),
      promotionPiece(promotionPiece), pieceEatenType(pieceEatenType), isEnPassant(isEnPassant) {}

  Move::Move(): from(-1), to(-1), promotionPiece(None) {}