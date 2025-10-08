#include "Board.h"
#include "Search.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "TTEntry.h"
#include <iostream>
#include "algorithm"
#include <fstream>
#include "csv.hpp"
#include "MoveTree.h"
#include <chrono>
using namespace std::chrono_literals;

Move parseAlgebraic(std::string notation, Board board) {
    std::string originalNotation = notation;
    // Remove move numbers (e.g., "1.e4") and check/checkmate symbols
    while (!notation.empty() && (isdigit(notation[0]) || notation[0] == '.'))
        notation = notation.substr(1);
    if (!notation.empty() && (notation.back() == '+' || notation.back() == '#'))
        notation.pop_back();

    // Generate all legal moves for current position
    MoveGenerator gen(board, true);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, 0);

    // Handle castling
    if (notation == "O-O" || notation == "0-0") {
        for (int i = 0; i < moveCount; i++) {
            Move& m = moves[0][i];
            if (m.pieceType == King && abs(m.to - m.from) == 2) return m;
        }
        
        throw std::runtime_error("No matching kingside castle move found");
    }
    if (notation == "O-O-O" || notation == "0-0-0") {
        for (int i = 0; i < moveCount; i++) {
            Move& m = moves[0][i];
            if (m.pieceType == King && abs(m.to - m.from) == 2) return m;
        }
        throw std::runtime_error("No matching queenside castle move found");
    }

    // Remove capture symbol
    notation.erase(std::remove(notation.begin(), notation.end(), 'x'), notation.end());

    // Extract promotion if present (e.g., e8=Q)
    PieceType promotion = None;
    size_t eqPos = notation.find('=');
    if (eqPos != std::string::npos) {
        char promChar = notation[eqPos + 1];
        switch (promChar) {
            case 'Q': promotion = Queen; break;
            case 'R': promotion = Rook; break;
            case 'B': promotion = Bishop; break;
            case 'N': promotion = Knight; break;
            default: promotion = None;
        }
        notation = notation.substr(0, eqPos); // remove promotion part
    }

    // Destination square: last 2 chars
    if (notation.size() < 2)
        throw std::runtime_error("Invalid SAN: " + notation);
    std::string destSquare = notation.substr(notation.size() - 2);
    int dest = Move::stringToSquare(destSquare);
    notation = notation.substr(0, notation.size() - 2); // remaining part

    // Determine moving piece
    PieceType piece = Pawn;
    if (!notation.empty() && isupper(notation[0])) {
        switch (notation[0]) {
            case 'K': piece = King; break;
            case 'Q': piece = Queen; break;
            case 'R': piece = Rook; break;
            case 'B': piece = Bishop; break;
            case 'N': piece = Knight; break;
        }
        notation = notation.substr(1); // remove piece char
    }

    // Disambiguation: file or rank
    char disambFile = 0;
    char disambRank = 0;
    if (!notation.empty()) {
        if (notation.size() == 2) {
            disambFile = notation[0];
            disambRank = notation[1];
        } else if (isdigit(notation[0])) {
            disambRank = notation[0];
        } else {
            disambFile = notation[0];
        }
    }

    // Determine captured piece at destination
    PieceType eaten = board.getPieceTypeAtBit(dest).first;

    // Search legal moves
    for (int i = 0; i < moveCount; i++) {
        Move& m = moves[0][i];
        if (m.pieceType != piece) continue;
        if (m.to != dest) continue;
        if (m.pieceEatenType != eaten) continue;
        if (promotion != None && m.promotionPiece != promotion) continue;

        std::string fromSq = Move::squareToString(m.from);
        if (disambFile && fromSq[0] != disambFile) continue;
        if (disambRank && fromSq[1] != disambRank) continue;

        return m;
    }

    board.print();
    for (Move& mv : board.moveHistory){
        std::cout << mv.toString() << std::endl;
    }
    throw std::runtime_error("No matching move found for notation: " + originalNotation);
}

std::string parseAlgebraic(Move mv, Board board){
    // Handle castling first
    if (mv.pieceType == King && std::abs(mv.to - mv.from) == 2) {
        int fileDiff = (mv.to > mv.from) ? 1 : -1;
        return (fileDiff > 0) ? "O-O" : "O-O-O";
    }

    std::string notation;

    // Piece letter (pawns have none)
    if (mv.pieceType != Pawn) {
        switch (mv.pieceType) {
            case Knight: notation += 'N'; break;
            case Bishop: notation += 'B'; break;
            case Rook:   notation += 'R'; break;
            case Queen:  notation += 'Q'; break;
            case King:   notation += 'K'; break;
            default: break;
        }
    }

    // Generate all legal moves to detect disambiguation
    MoveGenerator gen(board, true);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, 0);

    std::string fromStr = Move::squareToString(mv.from);
    std::string toStr   = Move::squareToString(mv.to);

    bool needsFile = false;
    bool needsRank = false;

    // Check if another piece of same type can reach same square
    for (int i = 0; i < moveCount; i++) {
        const Move& m2 = moves[0][i];
        if (m2.to == mv.to && m2.pieceType == mv.pieceType && m2.from != mv.from) {
            std::string otherFrom = Move::squareToString(m2.from);
            if (otherFrom[0] == fromStr[0])
                needsRank = true;
            if (otherFrom[1] == fromStr[1])
                needsFile = true;
            else {
                needsFile = true; // default to file disambiguation
            }
        }
    }

    // Disambiguation if needed
    if (needsFile) notation += fromStr[0];
    if (needsRank) notation += fromStr[1];

    // Capture handling
    if (mv.pieceEatenType != None) {
        if (mv.pieceType == Pawn && !needsFile) {
            // Pawn captures need file of origin
            notation += fromStr[0];
        }
        notation += 'x';
    }

    // Destination square
    notation += toStr;

    // Promotion
    if (mv.promotionPiece != None) {
        notation += '=';
        switch (mv.promotionPiece) {
            case Queen:  notation += 'Q'; break;
            case Rook:   notation += 'R'; break;
            case Bishop: notation += 'B'; break;
            case Knight: notation += 'N'; break;
            default: break;
        }
    }

    // Optional: detect if move gives check or checkmate
    // (requires making the move on a copy of the board)
    Board temp = board;
    temp.makeMove(mv);
    MoveGenerator after(temp, true);
    int nextMoveCount = 0;
    after.generateLegalMoves(moves, nextMoveCount, 0);
    bool kingInCheck = gen.isSquareAttacked(board.getKingPosition(board.whiteToMove ? white : black),board.whiteToMove ? black : white);
    if (kingInCheck) {
        if (nextMoveCount == 0)
            notation += '#';
        else
            notation += '+';
    }

    return notation;
}


Search::Search(){
    
}
MoveTree Search::openingTree = {};


std::vector<std::string> parsePythonListString(const std::string& input) {
    std::vector<std::string> moves;

    // Remove [ and ]
    size_t start = input.find('[');
    size_t end = input.rfind(']');
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return moves; // empty vector if invalid

    std::string content = input.substr(start + 1, end - start - 1);
    std::istringstream ss(content);
    std::string token;

    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        token.erase(std::find_if(token.rbegin(), token.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), token.end());

        // Remove surrounding single quotes if present
        if (!token.empty() && token.front() == '\'') token.erase(0, 1);
        if (!token.empty() && token.back() == '\'') token.pop_back();

        if (!token.empty()) moves.push_back(token);
    }

    return moves;
}


void Search::initOpeningTreeTXT(){
    openingTree.root = MoveNode();

    std::ifstream OpeningFile("Games.txt");

    std::string currentOpening;
    while (getline (OpeningFile, currentOpening)) {

        std::stringstream ss(currentOpening);
	
        // Temporary object to store 
        // the splitted string
        std::string t;
    
        // Delimiter
        char del = ' ';

        MoveNode* currentMoveNode = &openingTree.root;


        // Splitting the str string 
        // by delimiter
        int c = 0;
        while (getline(ss, t, del)){
            if (c == 8){break;}
            if (std::find(t.begin(),t.end(),'-') != t.end()){break;}


            auto it = std::find_if(currentMoveNode->children.begin(), currentMoveNode->children.end(),
                                [&](const MoveNode& node){ return node.value == t; });

            if (it == currentMoveNode->children.end()){
                currentMoveNode->children.push_back(MoveNode(t));
                currentMoveNode = &currentMoveNode->children.back(); // go deeper
            } else {
                currentMoveNode = &(*it);
            }
            c++;

        }
    }

}

void Search::initOpeningTreeCSV(){
    openingTree.root = MoveNode();
    

    csv::CSVReader reader("high_elo_opening.csv");
    
    std::vector<std::string> moveList;
    for (auto& row: reader) {
        Board board = Board();
        board.setStartingPosition();
        // Note: Can also use index of column with [] operator
        moveList = parsePythonListString(row["moves_list"].get());
        

        MoveNode* currentMoveNode = &openingTree.root;


        Move convertNotationToMove;
        for (std::string move : moveList){

            auto it = std::find_if(currentMoveNode->children.begin(), currentMoveNode->children.end(),
                                [&](const MoveNode& node){ return node.value == move; });

            if (it == currentMoveNode->children.end()){
                currentMoveNode->children.push_back(MoveNode(move));
                currentMoveNode = &currentMoveNode->children.back(); // go deeper
            } else {
                currentMoveNode = &(*it);
            }
        }
    }
    

}


std::chrono::milliseconds MAX_SEARCH_TIME = std::chrono::milliseconds(1000);

int probeResult(const Board& b) {
    // convert your board to bitboards as Fathom expects
    uint64_t white = b.getCombinedBoard(PieceColor::white);
    uint64_t black = b.getCombinedBoard(PieceColor::black);
    uint64_t kings = b.whiteKing | b.blackKing;
    uint64_t queens = b.whiteQueens | b.blackQueens;
    uint64_t rooks = b.whiteRooks | b.blackRooks;
    uint64_t bishops = b.whiteBishops | b.blackBishops;
    uint64_t knights = b.whiteKnights | b.blackKnights;
    uint64_t pawns = b.whitePawns | b.blackPawns;
    bool sideToMove = b.whiteToMove;

    int wdl = tb_probe_wdl(
        white, black, kings, queens, rooks, bishops, knights, pawns,
        b.halfMoveClock, b.castlingRights, b.enPassantSquare == -1 ? 0: b.enPassantSquare, b.whiteToMove
    );

    return wdl; // 0 = loss, 1 = draw, 2 = win
}

Move Search::findBestMoveEndgame(Board& board, unsigned int score){
    Move move;
    move.from = TB_GET_FROM(score);
    move.to = TB_GET_TO(score);

    move.pieceType = board.getPieceTypeAtBit(move.from).first;
    move.pieceColor = board.getPieceTypeAtBit(move.from).second;

    move.pieceEatenType = board.getPieceTypeAtBit(move.to).first;
    move.promotionPiece = static_cast<PieceType>(TB_GET_PROMOTES(score));
    if(move.promotionPiece == Pawn){move.promotionPiece = None;}
    move.isEnPassant = TB_GET_EP(score) != 0;
    return move;
}

Move Search::findBestMoveIterative(Board& board){
    if (board.countPieces() <= 5){
        std::cout << "Less than 5 pieces" << std::endl;
        uint64_t white = board.getCombinedBoard(PieceColor::white); // e1 + f2
        uint64_t black = board.getCombinedBoard(PieceColor::black);                // h1
        uint64_t kings = board.whiteKing | board.blackKing ;
        uint64_t queens = board.whiteQueens |board.blackQueens;
        uint64_t rooks = board.whiteRooks |board.blackRooks;
        uint64_t bishops = board.whiteBishops | board.blackBishops;
        uint64_t knights = board.whiteKnights | board.blackKnights;
        uint64_t pawns = board.whitePawns |board.blackPawns;
        bool whiteToMove = board.whiteToMove;
        unsigned int* results;
        unsigned int wdl = tb_probe_root(
            white, black, kings, queens, rooks, bishops, knights, pawns,
            0, 0, 0, false,results
        );
        

        if (wdl != TB_RESULT_FAILED){

            return findBestMoveEndgame(board, wdl);
        }
    }


    clearTT();

    int currentDepth = 1;
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    Move bestMove;
    while (true){
        bestMove = findBestMove(board,currentDepth);
        std::chrono::time_point now = std::chrono::high_resolution_clock::now();

        if(std::chrono::duration_cast<std::chrono::milliseconds>(now-start) > MAX_SEARCH_TIME
            ||currentDepth == MAX_DEPTH - 1){
            break;
        }
        currentDepth++;
    }
    std::cout << "DEPTH ACHIEVED: " << currentDepth << std::endl;
    return bestMove;
}


Move Search::findBestMove(Board& board, int depth) {
    MoveNode* currNode = &openingTree.root;
    bool flag = true;
    for (Move& mv : board.moveHistory){
        std::vector<MoveNode>& children = currNode->children;
        std::vector<std::string> moveChildren = {};
        for (MoveNode& mv : children){
            moveChildren.emplace_back(mv.value);
        }
        board.unmakeMove(moves[depth][i]);
        if (score > bestScore) {
            bestScore = score;
            bestMove = moves[depth][i];
        }
    }


    return bestMove;
}

std::chrono::milliseconds SEARCH_TIME_MILLISECONDS = std::chrono::milliseconds(1s);


Move Search::findBestMoveIterative(Board& board, bool printEvals, bool startingPos){
    
    if (startingPos){
        MoveNode* currNode = &openingTree.root;
        bool flag = true;
        for (Move& mv : board.moveHistory){
            std::vector<MoveNode>& children = currNode->children;
            std::vector<std::string> moveChildren = {};
            for (MoveNode& mvC : children){
                moveChildren.emplace_back(mvC.value);
            }


            if (std::find(moveChildren.begin(), moveChildren.end(),parseAlgebraic(mv,board)) != moveChildren.end()){
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
        else{
            flag = false;
            break;
        }
    }
<<<<<<< HEAD


    clearTT();
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    int currentDepth = 1;
    Move bestMove;
    std::chrono::milliseconds elapsed;
    while (true){
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << std::endl;
        if (elapsed >= SEARCH_TIME_MILLISECONDS || currentDepth == MAX_DEPTH - 1)
            break;
=======
    if (flag){
>>>>>>> 62b9c896acd04eaa0e6f225dccc0b29579049e58
        


        
    }
    MoveGenerator gen(board);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, depth);

    int bestScore = INT_MAX;
    Move bestMove;
    for (int i = 0; i < moveCount; i++) {
        board.makeMove(moves[depth][i]);
        int score = alphaBeta(board, depth - 1, INT_MIN, INT_MAX, true);
        board.unmakeMove(moves[depth][i]);
        if (score < bestScore) {
            bestScore = score;
            bestMove = moves[depth][i];
        }
    }

        currentDepth++;
    }

    std::cout << "Pick is: " << bestMove.toString()<< std::endl;
    std::cout << "Depth arrived: " << currentDepth << std::endl;
    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << std::endl;
    return bestMove;
    
}

int Search::alphaBeta(Board& board, int depth, int alpha, int beta) {
    uint64_t key = board.zobristHash;
    int originalAlpha = alpha; 

    // 1️⃣ TT probe
    if (auto* entry = probeTT(key)) {
        if (entry->depth >= depth) {
            if (entry->flag == EXACT) return entry->score;
            if (entry->flag == LOWERBOUND && entry->score >= beta) return entry->score;
            if (entry->flag == UPPERBOUND && entry->score <= alpha) return entry->score;
        }
    }

    // 2️⃣ Terminal node
    if (depth == 0)
        return Evaluator::evaluate(board);

    MoveGenerator gen(board);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, depth);

    // 3️⃣ No legal moves → checkmate or stalemate
    if (moveCount == 0) {
        if (gen.isSquareAttacked(
                board.getKingPosition(board.whiteToMove ? white : black),
                board.whiteToMove ? black : white))
            return -100000 + depth; // lose sooner = worse
        return 0; // stalemate
    }

    // 4️⃣ Move ordering
    std::sort(moves[depth], moves[depth] + moveCount, [](const Move& a, const Move& b) {
        int scoreA = 0, scoreB = 0;
        if (a.pieceEatenType != None) scoreA = PIECE_VALUES[a.pieceEatenType] - PIECE_VALUES[a.pieceType];
        if (b.pieceEatenType != None) scoreB = PIECE_VALUES[b.pieceEatenType] - PIECE_VALUES[b.pieceType];
        if (a.promotionPiece != None) scoreA += 1000;
        if (b.promotionPiece != None) scoreB += 1000;
        return scoreA > scoreB;
    });

    // 5️⃣ Core negamax recursion
    int bestValue = -1000000;
    Move bestMove;

    for (int i = 0; i < moveCount; i++) {
        board.makeMove(moves[depth][i]);
        int score = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.unmakeMove(moves[depth][i]);

        if (score > bestValue) {
            bestValue = score;
            bestMove = moves[depth][i];
        }

        alpha = std::max(alpha, score);
        if (alpha >= beta)
            break; // β cutoff
    }

    // 6️⃣ Store in TT
    TTFlag flag;
    if (bestValue <= originalAlpha) flag = UPPERBOUND;
    else if (bestValue >= beta) flag = LOWERBOUND;
    else flag = EXACT;
    storeTT(key, depth, bestValue, flag, bestMove);

    return bestValue;
}
