#include "MultiplayerChessGUI.h"
#include "ChessGUI.h"
#include <array>

MultiplayerChessGUI::MultiplayerChessGUI(PieceColor color, Board board) :ChessGUI(GUI_SCREENS(MULTIPLAYER_JOIN), board) {
	this->clientColor = color;
}

std::pair<int,int> convertGridCoords2(int square){
	int x = square % 8;
    int rankFromBottom = square / 8;
    int y = 7 - rankFromBottom;     // flip back
    return {x, y};
}

int convertGridCoords2(std::pair<int,int> square){
	int x = square.first;
    int y = square.second;
    int rankFromBottom = 7 - y;     // flip vertically
    return rankFromBottom * 8 + x;  // little-endian square index
}

void MultiplayerChessGUI::drawPieces(sf::RenderWindow& window, sf::Vector2i offset) {
	this->drawPawns(white, window, offset);
	this->drawPawns(black, window, offset);
	this->drawKnights(white, window, offset);
	this->drawKnights(black, window, offset);
	this->drawBishops(white, window, offset);
	this->drawBishops(black, window, offset);
	this->drawRooks(white, window, offset);
	this->drawRooks(black, window, offset);
	this->drawQueens(white, window, offset);
	this->drawQueens(black, window, offset);
	this->drawKing(white, window, offset);
	this->drawKing(black, window, offset);

}

void MultiplayerChessGUI::drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whiteKnightSprite : this->blackKnightSprite;
	if (color == white) {
		currBoard = this->chessboard.whiteKnights;
	}
	else {
		currBoard = this->chessboard.blackKnights;
	}



	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x* this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whitePawnSprite : this->blackPawnSprite;

	if (color == white) {
		currBoard = this->chessboard.whitePawns;
	}
	else {
		currBoard = this->chessboard.blackPawns;
	}


	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);
		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whiteBishopSprite : this->blackBishopSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteBishops;
	}
	else {
		currBoard = this->chessboard.blackBishops;
	}


	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);

		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whiteRookSprite : this->blackRookSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteRooks;
	}
	else {
		currBoard = this->chessboard.blackRooks;
	}


	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);

		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whiteKingSprite : this->blackKingSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteKing;
	}
	else {
		currBoard = this->chessboard.blackKing;
	}


	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);

		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite = color == white ? this->whiteQueenSprite : this->blackQueenSprite; 

	if (color == white) {
		currBoard = this->chessboard.whiteQueens;
	}
	else {
		currBoard = this->chessboard.blackQueens;
	}


	std::pair<int, int> gridCoords;
	int x;
	int y;


	while (currBoard){
        int targetSquare = __builtin_ctzll(currBoard);
		currBoard &= currBoard -1 ;
		gridCoords = convertGridCoords2(targetSquare);
		gridCoords = convertCoordsByColor(gridCoords);

		if (gridCoords == convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

std::pair<int, int> MultiplayerChessGUI::convertCoordsByColor(std::pair<int, int> coords) {
	if (this->clientColor == white) {
		return coords;
	}

	return std::make_pair(7 - std::get<0>(coords), 7 -std::get<1>(coords));
}


void MultiplayerChessGUI::drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	//Firewall incase there is no selected Piece
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::Sprite spriteToDraw = this->getSpriteOfPiece(this->selectedPiece.type, this->selectedPiece.color);
	//If it just has been clicked, render normally
	sf::Vector2i vectorPos = sf::Vector2i(std::get<0>(this->convertCoordsByColor(this->selectedPiece.pos)), std::get<1>(this->convertCoordsByColor(this->selectedPiece.pos)));
	vectorPos *= this->squareSize;
	vectorPos += boardOffset;
	spriteToDraw.setScale(sf::Vector2f(2, 2));
	if (clickEvent == 1) {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	else if (clickEvent == 3) {
		spriteToDraw.setPosition(sf::Vector2f(sf::Mouse::getPosition().x - this->squareSize / 2, sf::Mouse::getPosition().y - this->squareSize / 2));
		window.draw(spriteToDraw);
	}
	else {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}

}

void MultiplayerChessGUI::setSelectedPiece(std::pair<int, int> gridPos, PieceColor color) {
	int oneDimensionalIndex = convertGridCoords2(gridPos);

	

	uint64_t valueToCompare = (1ULL << oneDimensionalIndex);
	if (color == white) {
		if ((this->chessboard.whiteKnights & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Knight);
		}
		else if ((this->chessboard.whiteBishops & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Bishop);
		}
		else if ((this->chessboard.whitePawns & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Pawn);
		}
		else if ((this->chessboard.whiteRooks & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Rook);
		}
		else if ((this->chessboard.whiteQueens & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Queen);
		}
		else if ((this->chessboard.whiteKing & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, King);
		}
	}
	else {
		if ((this->chessboard.blackKnights & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Knight);
		}
		else if ((this->chessboard.blackBishops & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Bishop);
		}
		else if ((this->chessboard.blackPawns & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Pawn);
		}
		else if ((this->chessboard.blackRooks & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Rook);
		}
		else if ((this->chessboard.blackQueens & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Queen);
		}
		else if ((this->chessboard.blackKing & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, King);
		}
	}
	

}

void MultiplayerChessGUI::processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {

	if ((this->chessboard.whiteToMove ? white : black) != this->clientColor) { return; }

	//Mouse position data
	sf::Vector2i mousePos = sf::Mouse::getPosition();
	sf::Vector2i centeredMousePos = mousePos - boardOffset;

	std::pair<int, int> gridPos = std::make_pair(centeredMousePos.x / this->squareSize, centeredMousePos.y / this->squareSize);



	//Draw selected piece according to clickevent
	if (clickEvent == 1) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		std::pair<int, int> gridClick = this->convertCoordsByColor(getGridOfClick(mousePos, boardOffset));

		int oneDimensionalClick = convertGridCoords2(gridClick);

		uint64_t combinedSameColorBoard = this->chessboard.getCombinedBoard(this->clientColor);
		uint64_t combinedOppositeColorBoard = this->chessboard.getCombinedBoard(this->clientColor == white ? black : white);


		//If no selected Piece, check for selection
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) {
			if ((combinedSameColorBoard & (1ULL << oneDimensionalClick)) != 0) {
				std::cout << std::get<0>(gridClick) << std::get<1>(gridClick) << std::endl;
				this->setSelectedPiece(gridClick, this->clientColor);
				std::cout << "Selected Piece multiplayer: " << this->selectedPiece << std::endl;
			}
		}
		else if (this->selectedPiece.pos != std::make_pair(-1, -1)) {

			//Clicking in and empty square or clicking in square of opposite color
			if ((((combinedSameColorBoard) & (1ULL << oneDimensionalClick)) == 0)) {

				Move attemptedMove;
				MoveGenerator gen(this->chessboard);
				int moveCount = 0;
				gen.generateLegalMoves(moves,moveCount,0);
				bool flag = false;
				for (int i = 0; i < moveCount; i++){
					Move currMove = moves[0][i];
					if (currMove.to == convertGridCoords2(convertCoordsByColor(gridPos)) && currMove.from == convertGridCoords2(this->selectedPiece.pos)){
						flag = true;
						attemptedMove = currMove;
					}
				}

				if (flag) {
					if (this->isTherePromotion(attemptedMove)) {
						this->handlePromotions(attemptedMove, window);
						std::cout << "ended promotion" << std::endl;
					}
					this->chessboard.makeMove(attemptedMove);
					
					this->clearSelectedPiece();

					int moveCount = 0;
					gen.generateLegalMoves(moves,moveCount,0);
					if (this->chessboard.whiteToMove) {
				
						if (moveCount == 0) {
							if (gen.isSquareAttacked(chessboard.getKingPosition(white),black)){
								std::cout << "CHECKMATE: BLACK WINS" << std::endl;
							}
							else{
								std::cout << "DRAW" << std::endl;
							}
							
						}
					}
					else {
						if (moveCount == 0) {
							if (gen.isSquareAttacked(chessboard.getKingPosition(black),white)){
								std::cout << "CHECKMATE: WHITE WINS" << std::endl;
							}
							else{
								std::cout << "DRAW" << std::endl;
							}
						}
					}
				}
				else {

					this->clearSelectedPiece();
				}
			}
			if (((combinedSameColorBoard) & (1ULL << oneDimensionalClick)) != 0) {
				this->setSelectedPiece(gridClick, this->clientColor);
			}
		}
	}
	//Handle stuff that happens when RELEASING mouse button
	else if (clickEvent == 2) {
		//In case no selected piece, release of click has no use
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }


		
		if (centeredMousePos.x >= 0 && centeredMousePos.x <= this->squareSize * 8 &&
			centeredMousePos.y >= 0 && centeredMousePos.y <= this->squareSize * 8) {
			Move attemptedMove;
				MoveGenerator gen(this->chessboard);
				int moveCount = 0;
				gen.generateLegalMoves(moves,moveCount,0);
				bool flag = false;
				for (int i = 0; i < moveCount; i++){
					Move currMove = moves[0][i];
					if (currMove.to == convertGridCoords2(convertCoordsByColor(gridPos)) && currMove.from == convertGridCoords2(this->selectedPiece.pos)){
						flag = true;
						attemptedMove = currMove;
					}
				}
				if (flag) {
					

					if (this->isTherePromotion(attemptedMove)) {
						this->handlePromotions(attemptedMove, window);
						std::cout << "ended promotion" << std::endl;
					}
					this->chessboard.makeMove(attemptedMove);

					this->clearSelectedPiece();

					int moveCount = 0;
					gen.generateLegalMoves(moves,moveCount,0);
					if (this->chessboard.whiteToMove) {
				
						if (moveCount == 0) {
							if (gen.isSquareAttacked(chessboard.getKingPosition(white),black)){
								std::cout << "CHECKMATE: BLACK WINS" << std::endl;
							}
							else{
								std::cout << "DRAW" << std::endl;
							}
							
						}
					}
					else {
						if (moveCount == 0) {
							if (gen.isSquareAttacked(chessboard.getKingPosition(black),white)){
								std::cout << "CHECKMATE: WHITE WINS" << std::endl;
							}
							else{
								std::cout << "DRAW" << std::endl;
							}
						}
					}

				
			}

		}

	}
}

void MultiplayerChessGUI::drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	rect.setFillColor(sf::Color(255, 0, 0));
	std::pair<int, int> gridPos = this->convertCoordsByColor(this->selectedPiece.pos);
	sf::Vector2f newPosition = sf::Vector2f(boardOffset) + sf::Vector2f(std::get<0>(gridPos) * this->squareSize, std::get<1>(gridPos) * this->squareSize);
	rect.setPosition(newPosition);
	window.draw(rect);
}

void MultiplayerChessGUI::drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }

	int moveCount = 0;
	MoveGenerator gen(this->chessboard);
	gen.generateLegalMoves(moves,moveCount,0);

	sf::Vector2f circlePos;
	Move move;
	for (size_t i = 0; i < moveCount; i++) {
		move = moves[0][i];
		if (move.from == convertGridCoords2(this->selectedPiece.pos)) {
			circlePos = sf::Vector2f(std::get<0>(convertCoordsByColor(convertGridCoords2(move.to))) * this->squareSize, std::get<1>(convertCoordsByColor(convertGridCoords2(move.to))) * this->squareSize);
			circlePos += sf::Vector2f(boardOffset);
			circlePos += sf::Vector2f(40, 40);
			this->drawCircle(window, circlePos,  20, sf::Color(100, 100, 100));
		}
	}
}
