#pragma once
#include "GUI.h"
#include "Piece.h"
#include "../Engine/Board.h"
#include "../Engine/MoveGenerator.h"
class ChessGUI: public GUI
{
public:
	Board chessboard;

	sf::Texture whitePawnTexture;
	sf::Texture blackPawnTexture;
	sf::Texture whiteKnightTexture;
	sf::Texture blackKnightTexture;
	sf::Texture whiteBishopTexture;
	sf::Texture blackBishopTexture;
	sf::Texture whiteRookTexture;
	sf::Texture blackRookTexture;
	sf::Texture whiteQueenTexture;
	sf::Texture blackQueenTexture;
	sf::Texture whiteKingTexture;
	sf::Texture blackKingTexture;

	sf::Sprite whitePawnSprite;
	sf::Sprite blackPawnSprite;
	sf::Sprite whiteKnightSprite;
	sf::Sprite blackKnightSprite;
	sf::Sprite whiteBishopSprite;
	sf::Sprite blackBishopSprite;
	sf::Sprite whiteRookSprite;
	sf::Sprite blackRookSprite;
	sf::Sprite whiteQueenSprite;
	sf::Sprite blackQueenSprite;
	sf::Sprite whiteKingSprite;
	sf::Sprite blackKingSprite;

	sf::Font font;

	int squareSize;

	Piece selectedPiece;


	ChessGUI(GUI_SCREENS screen, Board board);

	void drawChessBoard(sf::RenderWindow& window, sf::Vector2i offset);

	void drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i offset);

	void drawPieces(sf::RenderWindow& window, sf::Vector2i offset);

	void drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	void drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset);

	std::pair<int, int> getGridOfClick(sf::Vector2i mousePos, sf::Vector2i offset);

	void processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

	void setSelectedPiece(std::pair<int,int> gridPos, PieceColor color);

	void clearSelectedPiece();

	void drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

	sf::Sprite getSpriteOfPiece(PieceType type, PieceColor color);

	void drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset);

	void drawLastMove(sf::RenderWindow& window , sf::Vector2i boardOffset);

	void handlePromotions(Move& move, sf::RenderWindow& window);

	bool isTherePromotion(Move& move);

	

};


