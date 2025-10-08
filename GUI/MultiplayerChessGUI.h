#pragma once
#include "ChessGUI.h"
#include "../Engine/Board.h"
class MultiplayerChessGUI: public ChessGUI
{
public:
	PieceColor clientColor;

	MultiplayerChessGUI(PieceColor color, Board board);

	void drawPieces(sf::RenderWindow& window, sf::Vector2i offset);

	void drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	std::pair<int, int> convertCoordsByColor(std::pair<int, int> coords);

	void drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

	void setSelectedPiece(std::pair<int, int> gridPos, PieceColor color);

	void processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

	void drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i boardOffset);

	void drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

};



