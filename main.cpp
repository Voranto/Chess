#include <iostream>
#include <optional>
#include "SFML/Graphics.hpp"
#include "GUI/GUI.h"
#include <stdexcept>
#include "GUI/ChessGUI.h"
#include "Engine/Board.h"
#include <bitset>
#include <chrono>
#include <thread>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include "Engine/MoveGenerator.h"
#include <bits/stdc++.h>
#include <chrono>
#include "Engine/Search.h"
#include "Engine/Evaluator.h"
#include "Engine/TTEntry.h"
bool DEBUG = true;

sf::RenderWindow window;
sf::Vector2f windowSize;
void renderStartGUI();
void renderSingleplayerGUI();
void renderBotGUI();
void renderLocalGUI();
sf::Font font;
void printBitboard(uint64_t board) {
    std::bitset<64> bits(board);
    for (int rank = 7; rank >= 0; --rank) { // rank 8 to 1
        for (int file = 0; file < 8; ++file) { // file a to h
            int sq = rank * 8 + file; 
            std::cout << bits[sq] << " ";
        }
        std::cout << std::endl;
    }
}

bool comp(Move a, Move b){
    return a.toString() < b.toString();
}



int main()
{
    MoveGenerator::initKnightAttacks();
    MoveGenerator::initKingAttacks();
    MoveGenerator::initSlidingAttacks();
    MoveGenerator::initPawnAttacks();
    Search::initOpeningTreeTXT();

    

    std::srand(std::time(0));
    if (DEBUG){
        Board board = Board();
        board.parseFEN("2r1kb1r/1Q2p1pp/2pBq3/1p3pR1/5P2/8/P4P1P/3R1K2 w k - 0 1");
        board.whiteToMove = false;
        board.castlingRights = 0;
        Search searcher = Search();
        std::cout << "BEST MOVE: " << searcher.findBestMove(board,7,true,false).toString() << std::endl;
        

    }
    else{
        Board board = Board();
        board.setStartingPosition();
        board.enPassantSquare = -1;
        MoveGenerator gen(board);

        window.create(sf::VideoMode::getDesktopMode(),"CHESS GAME",sf::Style::Default);
        windowSize = sf::Vector2f(window.getSize().x, window.getSize().y);

        window.setPosition(sf::Vector2i(0, 0));
        if (!font.openFromFile("..\\assets\\font\\arial.ttf")){
            throw std::invalid_argument("couldnt find file font arial");
        }
        window.setMouseCursorVisible(true);
        renderStartGUI();
    }
}


void renderStartGUI() {
    
    int clickEvent;
    // Load from a font file on disk
    
    sf::Texture background;
    if (!background.loadFromFile("..\\assets\\img\\ChessWallpaper.png")){
        throw std::invalid_argument("couldnt find file wallpaper");
    }

    GUI startGUI = GUI(START,background);
    startGUI.setBackground(background, windowSize);
    Button singleplayerButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.3), font, "SINGLEPLAYER", 70, sf::Color(255, 255, 255), sf::Color(200,200,200), 80);
    startGUI.buttons.emplace_back(singleplayerButton);

    Button hostGameButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.5), font, "HOST GAME", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    startGUI.buttons.emplace_back(hostGameButton);

    Button joinGameButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.7), font, "JOIN GAME", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    startGUI.buttons.emplace_back(joinGameButton);
    

    std::optional<Button> clickedButton;

    while (window.isOpen())
    {

        
        clickEvent = startGUI.processEventsAndReturnOnClick(window);
        
        window.clear();


        startGUI.drawBackground(window);

        clickedButton = startGUI.renderButtons(window,clickEvent == 2 ? true : false);

        startGUI.renderTextBoxes(window, clickEvent == 2 ? true : false);
        
        window.display();
      

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "SINGLEPLAYER") {
                std::cout << "SINGLEPLAYER" << std::endl;
                renderSingleplayerGUI();
            }
            /*else if (clickedButton.value().textString == "HOST GAME") {
                std::cout << "HOST GAME" << std::endl;
                renderHostGameGUI();
            }
            else if (clickedButton.value().textString == "JOIN GAME") {
                std::cout << "JOIN GAME" << std::endl;
                renderJoinGameGUI();
            }*/
            break;
        }
    }
}

void renderSingleplayerGUI() {
    int clickEvent;
    sf::Texture background;
    if (!background.loadFromFile("..\\assets\\img\\ChessWallpaper.png")){
        throw std::invalid_argument("couldnt find file wallpaper");
    }

    GUI singlePlayerGUI = GUI(SINGLEPLAYER,background);
    singlePlayerGUI.setBackground(background, windowSize);
    Button playLocallyButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.4), font, "PLAY LOCALLY", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    singlePlayerGUI.buttons.emplace_back(playLocallyButton);

    Button playBotButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.6), font, "PLAY AGAINST BOT", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    singlePlayerGUI.buttons.emplace_back(playBotButton);
    std::optional<Button> clickedButton;
    while (window.isOpen())
    {
        clickEvent = singlePlayerGUI.processEventsAndReturnOnClick(window);

        window.clear();


        singlePlayerGUI.drawBackground(window);

        clickedButton = singlePlayerGUI.renderButtons(window, clickEvent == 2 ? true : false);
        window.display();

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "PLAY LOCALLY") {
                std::cout << "PLAY LOCALLY" << std::endl;
                renderLocalGUI();
            }
            else if (clickedButton.value().textString == "PLAY AGAINST BOT") {
                std::cout << "PLAY AGAINST BOT" << std::endl;
                renderBotGUI();
            }
            break;
        }


    }
}

void renderBotGUI() {
    int clickEvent = -1;
    int newEvent;
    Board board = Board();
    board.setStartingPosition();

    Search moveFinder = Search();

    ChessGUI botGUI = ChessGUI(SINGLEPLAYER_BOT, board);
    botGUI.font = font;
    //FEATURES BUTTONS
    
    


    sf::Vector2i boardOffset(500, 50);

    std::optional<Button> clickedButton = std::nullopt;

    while (window.isOpen())
    {


        window.clear();
        clickEvent = botGUI.updateClickEvent(clickEvent, botGUI.processEventsAndReturnOnClick(window));


        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

        if (botGUI.chessboard.whiteToMove){
            botGUI.processClick(clickEvent,window,boardOffset);
        }
        else{
            std::cout << "BOT MOVING" << std::endl;


            Move bestMove = moveFinder.findBestMove(botGUI.chessboard,6);
            botGUI.chessboard.makeMove(bestMove);
            

            std::cout << "Your possible moves: " << std::endl;
            MoveNode* currNode = &Search::openingTree.root;
            bool flag = true;
            for (Move& mv : botGUI.chessboard.moveHistory){
                std::vector<MoveNode>& children = currNode->children;
                std::vector<std::string> moveChildren = {};
                for (MoveNode& mv : children){
                    moveChildren.emplace_back(mv.value);
                }


                if (std::find(moveChildren.begin(), moveChildren.end(),parseAlgebraic(mv,botGUI.chessboard)) != moveChildren.end()){
                    for (MoveNode& child : currNode->children) {
                        if (child.value == parseAlgebraic(mv, board)) {
                            currNode = &child;
                            break;
                        }
                    }

                }
                else{
                    flag = false;
                    break;
                }
            }
            if (flag){
                for (MoveNode mvN : currNode->children){
                    std::cout << mvN.value << std::endl;

                }

            }

        }
        botGUI.drawChessBoard(window, boardOffset);

        botGUI.drawSelectedPieceSquare(window, boardOffset);

        botGUI.drawLastMove(window,boardOffset);
        botGUI.drawPieces(window, boardOffset);

        botGUI.drawSelectedPiece(clickEvent, window, boardOffset);

        botGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = botGUI.renderButtons(window, clickEvent == 1);


        window.display();


    }
}

void renderLocalGUI(){
    int clickEvent = -1;
    int newEvent;
    Board board = Board();
    board.parseFEN("2r1kb1r/1Q2p1pp/2pBq3/1p3pR1/5P2/8/P4P1P/3R1K2 w k - 0 1");
    board.whiteToMove = false;


    ChessGUI localGUI = ChessGUI(SINGLEPLAYER_LOCAL, board);
    localGUI.font = font;
    //FEATURES BUTTONS
    
    


    sf::Vector2i boardOffset(500, 50);

    std::optional<Button> clickedButton = std::nullopt;

    while (window.isOpen())
    {


        window.clear();
        clickEvent = localGUI.updateClickEvent(clickEvent, localGUI.processEventsAndReturnOnClick(window));


        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

        localGUI.processClick(clickEvent,window,boardOffset);
        
        localGUI.drawChessBoard(window, boardOffset);

        localGUI.drawSelectedPieceSquare(window, boardOffset);

        localGUI.drawLastMove(window,boardOffset);
        localGUI.drawPieces(window, boardOffset);

        localGUI.drawSelectedPiece(clickEvent, window, boardOffset);

        localGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = localGUI.renderButtons(window, clickEvent == 1);


        window.display();


    }


}