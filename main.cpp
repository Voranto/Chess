#include <iostream>
#include <optional>
#include "SFML/Graphics.hpp"
#include <SFML/Network.hpp>
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
#include "tbprobe.h"
#include "GUI/MultiplayerChessGUI.h"
bool DEBUG = false;
sf::RenderWindow window;
sf::Vector2f windowSize;
void renderStartGUI();
void renderSingleplayerGUI();
void renderBotGUI();
void renderLocalGUI();
void renderJoinGameGUI();
void renderHostGameGUI();
void renderMultiplayerGameGUI(PieceColor color,sf::TcpSocket* socket);
std::vector<uint8_t> serializeMove(const Move& move);
Move deserializeMove(const uint8_t* data, size_t dataLength);
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
    if (!tb_init("../tablebases") ||TB_LARGEST == 0){
        std::exception();
    }
    
    
    std::srand(std::time(0));
    if (DEBUG){
        Board board = Board();
        board.parseFEN("2r1kb1r/1Q2p1pp/2pBq3/1p3pR1/5P2/8/P4P1P/3R1K2 w k - 0 1");
        board.whiteToMove = false;
        board.castlingRights = 0;
        Search searcher = Search();
        std::cout << "Best move: " << searcher.findBestMoveIterative(board).toString() << std::endl;
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
            else if (clickedButton.value().textString == "HOST GAME") {
                std::cout << "HOST GAME" << std::endl;
                renderHostGameGUI();
            }
            else if (clickedButton.value().textString == "JOIN GAME") {
                std::cout << "JOIN GAME" << std::endl;
                renderJoinGameGUI();
            }
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
    
    

    
    Button restoreMove = Button(sf::Vector2f(200,windowSize.y/2),font,"Undo Move",
    50,sf::Color(255,255,255),sf::Color(150,150,150),60);

    botGUI.buttons.emplace_back(restoreMove);

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
            

            Move bestMove = moveFinder.findBestMoveIterative(botGUI.chessboard);
            botGUI.chessboard.makeMove(bestMove);
            
        }
        botGUI.drawChessBoard(window, boardOffset);

        botGUI.drawSelectedPieceSquare(window, boardOffset);

        botGUI.drawLastMove(window,boardOffset);
        botGUI.drawPieces(window, boardOffset);

        botGUI.drawSelectedPiece(clickEvent, window, boardOffset);

        botGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = botGUI.renderButtons(window, clickEvent == 1);

        if (clickedButton.has_value()){
            if(clickedButton.value().text.getString() == "Undo Move"){
                if(!botGUI.chessboard.moveHistory.empty()){
                    if (botGUI.chessboard.whiteToMove){
                        botGUI.chessboard.unmakeMove(botGUI.chessboard.moveHistory.back());
                    }
                    botGUI.chessboard.unmakeMove(botGUI.chessboard.moveHistory.back());
                }
            }
        }


        window.display();


    }
}

void renderLocalGUI(){
    int clickEvent = -1;
    int newEvent;
    Board board = Board();
    board.parseFEN("2r1kb1r/1Q2p1pp/2pBq3/1p3pR1/5P2/8/P4P1P/3R1K2 w k - 0 1");
    board.whiteToMove = false;
    board.castlingRights = 0;

    ChessGUI localGUI = ChessGUI(SINGLEPLAYER_LOCAL, board);
    localGUI.font = font;
    //FEATURES BUTTONS
    
    Button restoreMove = Button(sf::Vector2f(200,windowSize.y/2),font,"Undo Move",
    50,sf::Color(255,255,255),sf::Color(150,150,150),60);

    localGUI.buttons.emplace_back(restoreMove);

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

        if (clickedButton.has_value()){
            if(clickedButton.value().text.getString() == "Undo Move"){
                if(!localGUI.chessboard.moveHistory.empty()){
                    localGUI.chessboard.unmakeMove(localGUI.chessboard.moveHistory.back());
                }
            }
        }

        window.display();


    }


}

void renderHostGameGUI() {

    sf::TcpListener listener;
    if (listener.listen(7777) != sf::Socket::Status::Done) {
        std::cerr << "Failed to start the listener!" << std::endl;
        return;
    }
    std::cout << "Server is listening on port 7777..." << std::endl;
    // Accept a client connection
    sf::TcpSocket client;
    GUI hostGameGUI = GUI(MULTIPLAYER_HOST);
    sf::Texture background;
    if (!background.loadFromFile("..\\assets\\img\\ChessWallpaper.png")){
        throw std::exception();
    }
    hostGameGUI.setBackground(background, windowSize);
    sf::Text joinGuideString( font,"", 25);
    joinGuideString.setString("To host a game, you have to use ngrok \n"
        "To activate ngrok, download it, go to its directory and write \n"
        " \"  ngrok tcp 7777 \" in the command line. After that, \n"
        "copy the ip adress and port and send it to your friend. \n"
        "It should look something like tcp://2.tcp.eu.ngrok.io:16886, \n"
        "where 2.tcp.eu.ngrok.io is the ip and 16886 is the port \n"
        "In the first text box  have your friend write the ip adress, in the second one the port, and click join");
    joinGuideString.setPosition(sf::Vector2f(400, 100));

    sf::RectangleShape guideRect(sf::Vector2f(joinGuideString.getGlobalBounds().size.x + 20, joinGuideString.getGlobalBounds().size.y + 20));
    guideRect.setPosition(joinGuideString.getPosition() - sf::Vector2f(10, 10));
    guideRect.setFillColor(sf::Color(50, 50, 50));

    


    if (listener.accept(client) != sf::Socket::Status::Done) {
        std::cerr << "Failed to accept the client connection!" << std::endl;
        return;
    }
    std::cout << "Client connected!" << std::endl;
    const std::string message = "Hello from the server!";
    if (client.send(message.c_str(), message.size() + 1) != sf::Socket::Status::Done) {
        std::cerr << "Failed to send message!" << std::endl;
    }

    // Receive data from the client
    char buffer[128];
    std::size_t received;
    if (client.receive(buffer, sizeof(buffer), received) != sf::Socket::Status::Done) {
        std::cerr << "Failed to receive message!" << std::endl;
    }
    std::cout << "Received from client: " << buffer << std::endl;
    renderMultiplayerGameGUI(white, &client);
}


// ---------- renderJoinGameGUI ----------
void renderJoinGameGUI() {

    int clickEvent;
    sf::Texture background;
    if (!background.loadFromFile("..\\assets\\img\\ChessWallpaper.png")) {
        std::cerr << "Failed to load background image." << std::endl;
    }

    GUI joinGameGUI = GUI(GUI_SCREENS(MULTIPLAYER_JOIN));
    joinGameGUI.setBackground(background, windowSize);

 

    // Connect to the server host
    
    
    TextBox ipAddressBox(sf::Vector2f(760, 400), sf::Vector2f(400, 100), sf::Color(255, 255, 255));
    ipAddressBox.fontSize = 50;
    ipAddressBox.text = "";
    joinGameGUI.textBoxes.emplace_back(ipAddressBox);
    

    TextBox portBox(sf::Vector2f(760, 510), sf::Vector2f(400, 100), sf::Color(255, 255, 255));
    portBox.fontSize = 50;
    portBox.text = "";
    joinGameGUI.textBoxes.emplace_back(portBox);

    Button ipAddressJoin = Button(sf::Vector2f(880, 650), font, "Join", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 30.0);
    joinGameGUI.buttons.emplace_back(ipAddressJoin);

    sf::Text joinGuideString(font,"",25);
    joinGuideString.setString("To join a friends game, ensure that they have hosted a game and that they have ngrok active. \n"
        "To activate ngrok, download it, go to its directory and write \n"
        " \"  ngrok tcp 7777 \" in the command line. After that, \n"
        "have your friend copy the ip adress and port and send it to you. \n"
        "It should look something like tcp://2.tcp.eu.ngrok.io:16886, \n"
        "where 2.tcp.eu.ngrok.io is the ip and 16886 is the port \n"
        "In the first text box write the ip adress, in the second one the port, and click join");
    joinGuideString.setPosition(sf::Vector2f(400, 100));

    sf::RectangleShape guideRect(sf::Vector2f(joinGuideString.getGlobalBounds().size.x + 20, joinGuideString.getGlobalBounds().size.y + 20));
    guideRect.setPosition(joinGuideString.getPosition() - sf::Vector2f(10,10));
    guideRect.setFillColor(sf::Color(50, 50, 50));
    
    bool connected = false;
    bool gameReady = false;
    std::optional<Button> clickedButton;
    while (!gameReady && window.isOpen()) {



        
        clickEvent = joinGameGUI.processEventsAndReturnOnClick(window);
        window.clear();
        joinGameGUI.drawBackground(window);

        clickedButton = joinGameGUI.renderButtons(window, (clickEvent == 2));

        joinGameGUI.renderTextBoxes(window, (clickEvent == 2));

        if (clickedButton.has_value() && clickedButton.value().textString == "Join") {
            sf::TcpSocket socket;

            // Connect to the server (change this to your server's IP address)

            if (socket.connect(sf::IpAddress::LocalHost , std::stoi(joinGameGUI.textBoxes[1].text)) != sf::Socket::Status::Done) {
                std::cerr << "Failed to connect to the server!" << std::endl;
                return;
            }
            std::cout << "Connected to server!" << std::endl;
            // Receive the message from the server
            char buffer[128];
            std::size_t received;
            if (socket.receive(buffer, sizeof(buffer), received) != sf::Socket::Status::Done) {
                std::cerr << "Failed to receive message!" << std::endl;
            }
            std::cout << "Received from server: " << buffer << std::endl;

            // Send a message to the server
            const std::string message = "Hello from the client!";
            if (socket.send(message.c_str(), message.size() + 1) != sf::Socket::Status::Done) {
                std::cerr << "Failed to send message!" << std::endl;
            }
            
            // Launch multiplayer game GUI for the joiner (client plays black, for example)
            renderMultiplayerGameGUI(black, &socket);
            

            }
        window.draw(guideRect);
        window.draw(joinGuideString);
        window.display();
    }
}

// ----------renderMultiplayerGameGUI----------
// Note: This version now takes pointers to the ENetPeer and ENetHost
void renderMultiplayerGameGUI(PieceColor color,sf::TcpSocket* socket) {
    int clickEvent = -1;
    Board board = Board();
    board.setStartingPosition();
    MultiplayerChessGUI multiplayerGameGUI(color, board);
    multiplayerGameGUI.font = font;
    int moveCount = 0;
    MoveGenerator gen(board);
    gen.generateLegalMoves(moves,moveCount,0);

    sf::Vector2i boardOffset(500, 50);
    std::optional<Button> clickedButton = std::nullopt;
    std::optional<Move> moveMade = std::nullopt;
    // Setup socket selector to monitor the socket for reading
    sf::SocketSelector selector = sf::SocketSelector();
    selector.add(*socket);  // Add socket to selector

    bool sendPackage = true;
    while (window.isOpen()) {
        window.clear();

        clickEvent = multiplayerGameGUI.updateClickEvent(clickEvent,
            multiplayerGameGUI.processEventsAndReturnOnClick(window));

        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

        // Assume processClick now returns an optional<Move> if a move was made
        multiplayerGameGUI.processClick(clickEvent, window, boardOffset);

        multiplayerGameGUI.drawChessBoard(window, boardOffset);
        multiplayerGameGUI.drawSelectedPieceSquare(window, boardOffset);
        multiplayerGameGUI.drawPieces(window, boardOffset);
        multiplayerGameGUI.drawSelectedPiece(clickEvent, window, boardOffset);
        multiplayerGameGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = multiplayerGameGUI.renderButtons(window, (clickEvent == 1));
        if (clickedButton.has_value()) {

        }


        //Handle multiplayer
        std::vector<uint8_t> buffer(1024);  // Allocate a buffer for up to 1024 bytes
        std::size_t received;
        // Set a timeout for socket activity (e.g., 100 milliseconds)
        PieceColor toMove = multiplayerGameGUI.chessboard.whiteToMove ? white : black;
        if (multiplayerGameGUI.clientColor != toMove) {
            sf::Time timeout = sf::milliseconds(50);
            if (selector.wait(timeout)) {
                if (selector.isReady(*socket)) {
                    // Socket is ready to read
                    sf::Socket::Status status = socket->receive(buffer.data(), buffer.size(), received);
                    if (status == sf::Socket::Status::Done) {
                        Move moveToDo = deserializeMove(buffer.data(), 7 * sizeof(int));
                        multiplayerGameGUI.chessboard.makeMove(moveToDo);
                        sendPackage = true;
                    }
                }
            }
        }

        if (multiplayerGameGUI.clientColor != toMove
            && !multiplayerGameGUI.chessboard.moveHistory.empty()
             && multiplayerGameGUI.chessboard.moveHistory.back().pieceColor == multiplayerGameGUI.clientColor
            && sendPackage) {
            std::vector<uint8_t> message = serializeMove(multiplayerGameGUI.chessboard.moveHistory.back());
            sf::Socket::Status stat= socket->send(message.data(),message.size());
            std::cout << "MESSAGE SENT" << std::endl;
            if (stat == sf::Socket::Status::Error ){
                throw std::exception();
            }
            sendPackage = false;
        }


        window.display();

    }
}


std::vector<uint8_t> serializeMove(const Move& move) {
    std::vector<uint8_t> buffer;

    // Convert enum fields to int (assuming they are enums)
    int pieceTypeVal = static_cast<int>(move.pieceType);
    int pieceColorVal = static_cast<int>(move.pieceColor);
    int from = move.from;
    int to = move.to;
    int pieceEatenType = static_cast<int>(move.pieceEatenType);
    int enPassantSquare = move.isEnPassant;
    int promotionPiece = static_cast<int>(move.promotionPiece);

    // Helper lambda to append an int's bytes to the buffer
    auto appendInt = [&buffer](int value) {
        uint8_t bytes[sizeof(int)];
        std::memcpy(bytes, &value, sizeof(int));
        buffer.insert(buffer.end(), bytes, bytes + sizeof(int));
        };

    appendInt(pieceTypeVal);
    appendInt(pieceColorVal);
    appendInt(from);
    appendInt(to);
    appendInt(pieceEatenType);
    appendInt(enPassantSquare);
    appendInt(promotionPiece);
    return buffer;
}

Move deserializeMove(const uint8_t* data, size_t dataLength) {
    // Check if the packet is large enough: we expect 6 ints
    if (dataLength < 7 * sizeof(int)) {
        std::cerr << "Received packet is too small to contain a valid Move." << std::endl;
        return Move(); // return a default move or handle error
    }
    int pieceTypeVal, pieceColorVal, from, to, pieceEaten, enPassant, promotion;
    std::memcpy(&pieceTypeVal, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&pieceColorVal, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&from, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&to, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&pieceEaten, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&enPassant, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&promotion, data, sizeof(int));
    return Move(
        static_cast<PieceType>(pieceTypeVal),
        static_cast<PieceColor>(pieceColorVal),
        from,
        to,
        static_cast<PieceType>(promotion),
        static_cast<PieceType>(pieceEaten),
        static_cast<bool>(enPassant)

    );
}