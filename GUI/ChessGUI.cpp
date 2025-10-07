#include "ChessGUI.h"

#include <array>
ChessGUI::ChessGUI(GUI_SCREENS screen, Board board)
    : GUI(screen),
      chessboard(board),
      squareSize(120),
      selectedPiece(-1, -1, white, Pawn),
      whitePawnSprite(whitePawnTexture),
      blackPawnSprite(blackPawnTexture),
      whiteKnightSprite(whiteKnightTexture),
      blackKnightSprite(blackKnightTexture),
      whiteBishopSprite(whiteBishopTexture),
      blackBishopSprite(blackBishopTexture),
      whiteRookSprite(whiteRookTexture),
      blackRookSprite(blackRookTexture),
      whiteQueenSprite(whiteQueenTexture),
      blackQueenSprite(blackQueenTexture),
      whiteKingSprite(whiteKingTexture),
      blackKingSprite(blackKingTexture)
{
	this->mode = screen;
	this->chessboard = board;
	
	this->squareSize = 120;
	this->selectedPiece = Piece(-1,-1,white,Pawn);

	if (!whitePawnTexture.loadFromFile("..\\assets\\img\\whitePawn.png")) {
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whitePawnSprite = sf::Sprite(whitePawnTexture);

	if (!blackPawnTexture.loadFromFile("..\\assets\\img\\blackPawn.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackPawnSprite = sf::Sprite(blackPawnTexture);

	if (!whiteKnightTexture.loadFromFile("..\\assets\\img\\whiteKnight.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whiteKnightSprite = sf::Sprite(whiteKnightTexture);
	if (!blackKnightTexture.loadFromFile("..\\assets\\img\\blackKnight.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackKnightSprite = sf::Sprite(blackKnightTexture);
	if (!whiteBishopTexture.loadFromFile("..\\assets\\img\\whiteBishop.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whiteBishopSprite = sf::Sprite(whiteBishopTexture);
	if (!blackBishopTexture.loadFromFile("..\\assets\\img\\blackBishop.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackBishopSprite = sf::Sprite(blackBishopTexture);
	if (!whiteRookTexture.loadFromFile("..\\assets\\img\\whiteRook.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whiteRookSprite = sf::Sprite(whiteRookTexture);
	if (!blackRookTexture.loadFromFile("..\\assets\\img\\blackRook.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackRookSprite = sf::Sprite(blackRookTexture);
	if (!whiteQueenTexture.loadFromFile("..\\assets\\img\\whiteQueen.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whiteQueenSprite = sf::Sprite(whiteQueenTexture);
	if (!blackQueenTexture.loadFromFile("..\\assets\\img\\blackQueen.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackQueenSprite = sf::Sprite(blackQueenTexture);
	if (!whiteKingTexture.loadFromFile("..\\assets\\img\\whiteKing.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->whiteKingSprite = sf::Sprite(whiteKingTexture);
	
	if (!blackKingTexture.loadFromFile("..\\assets\\img\\blackKing.png")){
		std::cerr << "Failed to load texture" << std::endl;
	}
	this->blackKingSprite = sf::Sprite(blackKingTexture);
}

void ChessGUI::drawChessBoard(sf::RenderWindow& window, sf::Vector2i offset) {
	
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	for (int i = 0; i < 8; i ++) {
		for (int j = 0; j < 8 ; j ++) {
			if ((i + j) % 2 != 0) {
				rect.setFillColor(sf::Color(37, 82, 36));
			}
			else {
				rect.setFillColor(sf::Color(222, 218, 146));
			}
			rect.setPosition(sf::Vector2f(offset.x + this->squareSize * j, offset.y + this->squareSize * i));
			window.draw(rect);
		}
	}
}

void ChessGUI::drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i offset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	rect.setFillColor(sf::Color(255, 0, 0));
	sf::Vector2f newPosition = sf::Vector2f(offset) + sf::Vector2f(std::get<0>(this->selectedPiece.pos) * this->squareSize , std::get<1>(this->selectedPiece.pos) * this->squareSize);
	rect.setPosition(newPosition);
	window.draw(rect);
}

void ChessGUI::drawPieces(sf::RenderWindow& window, sf::Vector2i offset) {
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

std::pair<int,int> convertGridCoords(int square){
	int x = square % 8;
    int rankFromBottom = square / 8;
    int y = 7 - rankFromBottom;     // flip back
    return {x, y};
}

int convertGridCoords(std::pair<int,int> square){
	int x = square.first;
    int y = square.second;
    int rankFromBottom = 7 - y;     // flip vertically
    return rankFromBottom * 8 + x;  // little-endian square index
}

void ChessGUI::drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		if (gridCoords == this->selectedPiece.pos) { continue; }
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x* this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		if (gridCoords == this->selectedPiece.pos) { continue; }
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = convertGridCoords(targetSquare);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

std::pair<int, int> ChessGUI::getGridOfClick(sf::Vector2i mousePos, sf::Vector2i offset) {
	mousePos -= offset;

	if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > 8 * this->squareSize || mousePos.y > 8 * this->squareSize) {
		return std::make_pair(-1, -1);
	}
	return std::make_pair(std::trunc(mousePos.x / this->squareSize), std::trunc(mousePos.y / this->squareSize));
}

void ChessGUI::processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	
	//Mouse position data
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2i centeredMousePos = mousePos - boardOffset;

	std::pair<int, int> gridPos = std::make_pair(centeredMousePos.x / this->squareSize, centeredMousePos.y / this->squareSize);


	//Draw selected piece according to clickevent
	if (clickEvent == 1) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		std::pair<int, int> gridClick = getGridOfClick(mousePos, boardOffset);

		int oneDimensionalClick = convertGridCoords(gridClick);

		uint64_t combinedSameColorBoard = this->chessboard.getCombinedBoard(this->chessboard.whiteToMove ? white : black);
		uint64_t combinedOppositeColorBoard = this->chessboard.getCombinedBoard(this->chessboard.whiteToMove ? black : white);
		
		
		//If no selected Piece, check for selection
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) {
			if ((combinedSameColorBoard & (1ULL << oneDimensionalClick)) != 0) {
				this->setSelectedPiece(gridClick, this->chessboard.whiteToMove ? white : black);
				
			}
		}
		else if (this->selectedPiece.pos != std::make_pair(-1, -1)) {

			//Clicking in and empty square or clicking in square of opposite color
			if ((((combinedSameColorBoard ) & (1ULL <<oneDimensionalClick)) == 0 )) {
				Move attemptedMove;
				MoveGenerator gen(this->chessboard);
				int moveCount = 0;
				gen.generateLegalMoves(moves,moveCount,0);
				bool flag = false;
				for (int i = 0; i < moveCount; i++){
					Move currMove = moves[0][i];
					if (currMove.to == convertGridCoords(gridPos) && currMove.from == convertGridCoords(this->selectedPiece.pos)){
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
				this->setSelectedPiece(gridClick, this->chessboard.whiteToMove ? white : black);
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
					if (currMove.to == convertGridCoords(gridPos) && currMove.from == convertGridCoords(this->selectedPiece.pos)){
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



void ChessGUI::setSelectedPiece(std::pair<int, int> gridPos, PieceColor color) {
	int oneDimensionalIndex = convertGridCoords(gridPos);
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

void ChessGUI::clearSelectedPiece() {

	this->selectedPiece = Piece(-1,-1, white, Pawn);
}

sf::Sprite ChessGUI::getSpriteOfPiece(PieceType type, PieceColor color) {
	switch (type) {
	case Pawn: 
		return color == white ? this->whitePawnSprite : this->blackPawnSprite;
		break;
	case Knight:
		return color == white ? this->whiteKnightSprite : this->blackKnightSprite;
		break;
	case Bishop:
		return color == white ? this->whiteBishopSprite : this->blackBishopSprite;
		break;
	case Rook:
		return color == white ? this->whiteRookSprite : this->blackRookSprite;
		break;
	case Queen:
		return color == white ? this->whiteQueenSprite : this->blackQueenSprite;
		break;
	case King:
		return color == white ? this->whiteKingSprite : this->blackKingSprite;
		break;
	}
	std::cerr << "NO TEXTURE FOUND" << std::endl;
	return this->whitePawnSprite;
}

void ChessGUI::drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	//Firewall incase there is no selected Piece
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::Sprite spriteToDraw = this->getSpriteOfPiece(this->selectedPiece.type, this->selectedPiece.color);
	//If it just has been clicked, render normally
	sf::Vector2i vectorPos = sf::Vector2i(std::get<0>(this->selectedPiece.pos), std::get<1>(this->selectedPiece.pos));
	vectorPos *= this->squareSize;
	vectorPos += boardOffset;
	spriteToDraw.setScale(sf::Vector2f(2,2));
	if (clickEvent == 1) {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	else if (clickEvent == 3) {
		spriteToDraw.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x - this->squareSize / 2 , sf::Mouse::getPosition(window).y - this->squareSize / 2) );
		window.draw(spriteToDraw);
	}
	else {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	
}

void ChessGUI::drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }

	int moveCount = 0;
	MoveGenerator gen(this->chessboard);
	gen.generateLegalMoves(moves,moveCount,0);

	sf::Vector2f circlePos;
	Move move;
	for (size_t i = 0; i < moveCount; i++) {
		move = moves[0][i];
		if (move.from == convertGridCoords(this->selectedPiece.pos)) {
			circlePos = sf::Vector2f(std::get<0>(convertGridCoords(move.to)) * this->squareSize, std::get<1>(convertGridCoords(move.to)) * this->squareSize);
			circlePos += sf::Vector2f(boardOffset);
			circlePos += sf::Vector2f(40, 40);
			this->drawCircle(window, circlePos,  20, sf::Color(100, 100, 100));
		}
	}
}

void ChessGUI::handlePromotions(Move& move, sf::RenderWindow& window) {
	//Create buttons
	Button queenButton = Button(sf::Vector2f(250, 100), this->font, "QUEEN", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button rookButton = Button(sf::Vector2f(250, 200), this->font, "ROOK", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button knightButton = Button(sf::Vector2f(250, 300), this->font, "KNIGHT", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button bishopButton = Button(sf::Vector2f(250, 400), this->font, "BISHOP", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	
	this->buttons.emplace_back(queenButton);
	this->buttons.emplace_back(knightButton);
	this->buttons.emplace_back(rookButton);
	this->buttons.emplace_back(bishopButton);



	int numberOfButtons = this->buttons.size();

	sf::Vector2i boardOffset(500, 50);

	std::optional<Button> clickedButton = std::nullopt;
	int clickEvent = -1;

	Piece pieceToReturn;
	
	//Loop until button clicked
	while (true) {
		window.clear();
		clickEvent = updateClickEvent(clickEvent, processEventsAndReturnOnClick(window));


		sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

		this->processClick(clickEvent, window, boardOffset);

		this->drawChessBoard(window, boardOffset);

		this->drawSelectedPieceSquare(window, boardOffset);


		this->drawPieces(window, boardOffset);

		this->drawSelectedPiece(clickEvent, window, boardOffset);

		this->drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

		clickedButton = this->renderButtons(window, clickEvent == 1);

		if (clickedButton.has_value()) {
			if (clickedButton.value().textString == "QUEEN") {
				move.promotionPiece = Queen;
				break;
			}
			if (clickedButton.value().textString == "ROOK") {
				move.promotionPiece = Rook;
				break;
			}
			if (clickedButton.value().textString == "BISHOP") {
				move.promotionPiece = Bishop;
				break;
			}
			if (clickedButton.value().textString == "KNIGHT") {
				move.promotionPiece = Knight;
				break;
			}

		}

		window.display();
	}
	//Delete the buttons
	for (int i = 0;i < 4;i++) {
		this->buttons.pop_back();
	}
	
}

bool ChessGUI::isTherePromotion(Move& move) {
	return move.promotionPiece != None;
}

void ChessGUI::drawLastMove(sf::RenderWindow& window , sf::Vector2i boardOffset){
	if (this->chessboard.moveHistory.size() == 0){return;}

	Move lastMove = this->chessboard.moveHistory.back();

	//Draw a square in the from and to place of the 
	sf::Color color(187,203,43);

	std::pair<int,int> from = convertGridCoords(lastMove.from);
	std::pair<int,int> to = convertGridCoords(lastMove.to);

	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	rect.setFillColor(color);
	sf::Vector2f newPosition = sf::Vector2f(boardOffset) + sf::Vector2f(std::get<0>(from) * this->squareSize , std::get<1>(from) * this->squareSize);
	rect.setPosition(newPosition);
	window.draw(rect);
	
	newPosition = sf::Vector2f(boardOffset) + sf::Vector2f(std::get<0>(to) * this->squareSize , std::get<1>(to) * this->squareSize);
	rect.setPosition(newPosition);
	
	window.draw(rect);
}

