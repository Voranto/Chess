#include "MoveGenerator.h"
#include "Move.h"
#include <intrin.h>
#include "Board.h"
#include <bits/stdc++.h>


MoveGenerator::MoveGenerator(Board& b, bool fast) : board(b){
    this->fast = fast;
}

bool MoveGenerator::isSquareAttacked(int square, PieceColor oppositeColor) const{
    //mask for knights
    uint64_t* knightBoard = board.getBoardOfType(Knight, oppositeColor);
    if ((*knightBoard & knightAttacks[square]) != 0){
        return true;
    }

    //pawns
    if (oppositeColor == white) {
    if (BlackPawnAttacks[square] & *board.getBoardOfType(Pawn, white)) return true;
    } else {
        if (WhitePawnAttacks[square] & *board.getBoardOfType(Pawn, black)) return true;
    }


    //Bishops
    uint64_t* bishopBoard = board.getBoardOfType(Bishop, oppositeColor);
    uint64_t currentBishopAttacks = getBishopAttacks(square,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
    if ((*bishopBoard & currentBishopAttacks) != 0){
        return true;
    }

    //Rooks
    uint64_t* rookBoard = board.getBoardOfType(Rook, oppositeColor);
    uint64_t currentRookAttacks = getRookAttacks(square,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
    if ((*rookBoard & currentRookAttacks) != 0){
        return true;
    }

    //Queen
    uint64_t* queenBoard = board.getBoardOfType(Queen, oppositeColor);
    uint64_t currentQueenAttacks = getRookAttacks(square,(board.getCombinedBoard(white) | board.getCombinedBoard(black)))
                                    |getBishopAttacks(square,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
    if ((*queenBoard & currentQueenAttacks) != 0){
        return true;
    }

    //King
    uint64_t* kingBoard = board.getBoardOfType(King, oppositeColor);
    if ((*kingBoard & kingAttacks[square]) != 0){
        return true;
    }
    return false;
}

void MoveGenerator::generatePseudoLegalMoves(Move (*moves)[MAX_MOVES], int& moveCount , int currentDepth) const{
    
    generateKingMoves(moves,moveCount,currentDepth);
    generateKnightMoves(moves,moveCount,currentDepth);
    generateRookMoves(moves,moveCount,currentDepth);
    generateBishopMoves(moves,moveCount,currentDepth);
    generateQueenMoves(moves,moveCount,currentDepth);
    generatePawnMoves(moves,moveCount,currentDepth);
}

void MoveGenerator::generateLegalMoves(Move (*moves)[MAX_MOVES], int& moveCount , int currentDepth){
    generatePseudoLegalMoves(moves, moveCount,currentDepth);
    int newCount = 0;
    for (int i = 0; i < moveCount; i++) {
        if (isLegal(moves[currentDepth][i])) {
            moves[currentDepth][newCount++] = moves[currentDepth][i];  // keep only legal ones
        }
    }
    moveCount = newCount;
}



bool MoveGenerator::isLegal(Move& move){
    
    
    //-----------OPTION 1

    board.makeMove(move);
    bool ans = !isSquareAttacked(board.getKingPosition(board.whiteToMove ? black : white),board.whiteToMove ? white : black);
    board.unmakeMove(move);
    return ans;

    
    }

//REMEMBER CASTLING
void MoveGenerator::generateKingMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t kingBoard = color == white ? board.whiteKing : board.blackKing;
    
    while (kingBoard){
        int targetSquare = __builtin_ctzll(kingBoard);
        kingBoard &= kingBoard -1 ;
       //Generate castling moves
        if(board.whiteToMove){
            if((board.castlingRights & (1ULL << 1)) != 0 &&
                board.getPieceTypeAtBit(1) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(2) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(3) == std::make_pair(None,white) && 
                !this->isSquareAttacked(board.getKingPosition(white),black)){
                moves[currentDepth][moveCount++] = Move(King,white,4,2);
            }
            if((board.castlingRights & (1ULL << 0)) != 0 &&
                board.getPieceTypeAtBit(5) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(6) == std::make_pair(None,white) &&
                !this->isSquareAttacked(board.getKingPosition(white),black) ){
                moves[currentDepth][moveCount++] = Move(King,white,4,6);
            }
        }else{
            if((board.castlingRights & (1ULL << 3)) != 0 &&
                board.getPieceTypeAtBit(57) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(58) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(59) == std::make_pair(None,white) &&
                !this->isSquareAttacked(board.getKingPosition(black),white)){
                moves[currentDepth][moveCount++] = Move(King,black,60,58);
            }
            if((board.castlingRights & (1ULL << 2)) != 0 &&
                board.getPieceTypeAtBit(61) == std::make_pair(None,white) &&
                board.getPieceTypeAtBit(62) == std::make_pair(None,white) &&
                !this->isSquareAttacked(board.getKingPosition(black),white)){
                moves[currentDepth][moveCount++] = Move(King,black,60,62);
            }
        }


        uint64_t currentKingAttacks = kingAttacks[targetSquare];
        //Mask for current pieces
        currentKingAttacks &= ~board.getCombinedBoard(color);
        while (currentKingAttacks){
            int targetAttack = __builtin_ctzll(currentKingAttacks);
            currentKingAttacks &= currentKingAttacks -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;

            Move moveToAdd = Move(King,color, targetSquare,targetAttack,None,pieceEatenType,false);
            moves[currentDepth][moveCount++] = moveToAdd;
        }


    }
}

void MoveGenerator::generateKnightMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t knightsBoard = color == white ? board.whiteKnights : board.blackKnights;
    
    while (knightsBoard){
        int targetSquare = __builtin_ctzll(knightsBoard);
        knightsBoard &= knightsBoard -1 ;

        uint64_t currentKnightAttack = knightAttacks[targetSquare];
        //Mask for current pieces
        currentKnightAttack &= ~board.getCombinedBoard(color);
        while (currentKnightAttack){
            int targetAttack = __builtin_ctzll(currentKnightAttack);
            currentKnightAttack &= currentKnightAttack -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;

            Move moveToAdd = Move(Knight,color, targetSquare,targetAttack,None,pieceEatenType,false);
            moves[currentDepth][moveCount++] = moveToAdd;
        }


    }
}

void MoveGenerator::generateRookMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t rookBoard = color == white ? board.whiteRooks : board.blackRooks;
    while (rookBoard){
        
        int targetSquare = __builtin_ctzll(rookBoard);
        rookBoard &= rookBoard -1 ;
        uint64_t currentRookAttacks = getRookAttacks(targetSquare,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
        currentRookAttacks &= ~board.getCombinedBoard(color);
        while (currentRookAttacks){
            int targetAttack = __builtin_ctzll(currentRookAttacks);
            currentRookAttacks &= currentRookAttacks -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;

            Move moveToAdd = Move(Rook,color, targetSquare,targetAttack,None,pieceEatenType,false);

            moves[currentDepth][moveCount++] = moveToAdd;
        }
    }
}

void MoveGenerator::generateBishopMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t bishopBoard = color == white ? board.whiteBishops : board.blackBishops;
    while (bishopBoard){
        
        int targetSquare = __builtin_ctzll(bishopBoard);
        bishopBoard &= bishopBoard -1 ;
        uint64_t currentBishopAttacks = getBishopAttacks(targetSquare,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
        currentBishopAttacks &= ~board.getCombinedBoard(color);
        while (currentBishopAttacks){
            int targetAttack = __builtin_ctzll(currentBishopAttacks);
            currentBishopAttacks &= currentBishopAttacks -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;

            Move moveToAdd = Move(Bishop,color, targetSquare,targetAttack,None,pieceEatenType,false);

            moves[currentDepth][moveCount++] = moveToAdd;
        }
    }
}

void MoveGenerator::generateQueenMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth) const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t queenBoard = color == white ? board.whiteQueens : board.blackQueens;
    while (queenBoard){
        
        int targetSquare = __builtin_ctzll(queenBoard);
        queenBoard &= queenBoard -1 ;
        uint64_t currentQueenAttacks = getBishopAttacks(targetSquare,(board.getCombinedBoard(white) | board.getCombinedBoard(black)))
                                        | getRookAttacks(targetSquare,(board.getCombinedBoard(white) | board.getCombinedBoard(black)));
        
        currentQueenAttacks &= ~board.getCombinedBoard(color);
        while (currentQueenAttacks){
            int targetAttack = __builtin_ctzll(currentQueenAttacks);
            currentQueenAttacks &= currentQueenAttacks -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;

            Move moveToAdd = Move(Queen,color, targetSquare,targetAttack,None,pieceEatenType,false);

            moves[currentDepth][moveCount++] = moveToAdd;
        }
    }
}

void MoveGenerator::generatePawnMoves(Move (*moves)[MAX_MOVES], int& moveCount, int currentDepth)  const {
    PieceColor color = board.whiteToMove == true ? white : black;
    uint64_t pawnBoard = color == white ? board.whitePawns : board.blackPawns;
    while (pawnBoard){
        
        int targetSquare = __builtin_ctzll(pawnBoard);
        pawnBoard &= pawnBoard -1 ;
        uint64_t pawnAttacks = getPawnAttacks(targetSquare, board.getCombinedBoard(color), board.getCombinedBoard(color == white? black : white));
        while (pawnAttacks){
            int targetAttack = __builtin_ctzll(pawnAttacks);
            pawnAttacks &= pawnAttacks -1;
            PieceType pieceEatenType = board.getPieceTypeAtBit(targetAttack).first;
            if ((color == white && targetAttack > 55)  | (color == black && targetAttack < 8)){
                for (int i = 1; i < 5; i++){
                    moves[currentDepth][moveCount++] = Move(Pawn,color, targetSquare,targetAttack,static_cast<PieceType>(i),pieceEatenType,targetAttack == board.enPassantSquare);
                }
            }
            else{
                if (std::abs(targetAttack - targetSquare) == 16){
                    int direction = color == white ? 8 : -8;
                    if (board.getPieceTypeAtBit(targetSquare + direction).first != None){
                        continue;
                    }
                }


                Move moveToAdd = Move(Pawn,color, targetSquare,targetAttack,None,pieceEatenType,targetAttack == board.enPassantSquare);

                moves[currentDepth][moveCount++] = moveToAdd;
            }
        }
    }
}


uint64_t MoveGenerator::knightAttacks[64] = {0};
uint64_t MoveGenerator::kingAttacks[64] = {0};

uint64_t MoveGenerator::RookAttackTable[64][4096] = {0};
uint64_t MoveGenerator::BishopAttackTable[64][512] = {0};


int MoveGenerator::RookRelevantBits[64]=
{
	52, 53, 53, 53, 53, 53, 53, 52,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 53, 53, 53, 53, 53
};

uint64_t MoveGenerator::RookMagics[64]=
{
	0x0080001020400080, 0x0040001000200040, 0x0080081000200080, 0x0080040800100080,
	0x0080020400080080, 0x0080010200040080, 0x0080008001000200, 0x0080002040800100,
	0x0000800020400080, 0x0000400020005000, 0x0000801000200080, 0x0000800800100080,
	0x0000800400080080, 0x0000800200040080, 0x0000800100020080, 0x0000800040800100,
	0x0000208000400080, 0x0000404000201000, 0x0000808010002000, 0x0000808008001000,
	0x0000808004000800, 0x0000808002000400, 0x0000010100020004, 0x0000020000408104,
	0x0000208080004000, 0x0000200040005000, 0x0000100080200080, 0x0000080080100080,
	0x0000040080080080, 0x0000020080040080, 0x0000010080800200, 0x0000800080004100,
	0x0000204000800080, 0x0000200040401000, 0x0000100080802000, 0x0000080080801000,
	0x0000040080800800, 0x0000020080800400, 0x0000020001010004, 0x0000800040800100,
	0x0000204000808000, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	0x0000040008008080, 0x0000020004008080, 0x0000010002008080, 0x0000004081020004,
	0x0000204000800080, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	0x0000040008008080, 0x0000020004008080, 0x0000800100020080, 0x0000800041000080,
	0x00FFFCDDFCED714A, 0x007FFCDDFCED714A, 0x003FFFCDFFD88096, 0x0000040810002101,
	0x0001000204080011, 0x0001000204000801, 0x0001000082000401, 0x0001FFFAABFAD1A2
};
uint64_t MoveGenerator::RookMasks[64]=
{	
	0x000101010101017E, 0x000202020202027C, 0x000404040404047A, 0x0008080808080876,
	0x001010101010106E, 0x002020202020205E, 0x004040404040403E, 0x008080808080807E,
	0x0001010101017E00, 0x0002020202027C00, 0x0004040404047A00, 0x0008080808087600,
	0x0010101010106E00, 0x0020202020205E00, 0x0040404040403E00, 0x0080808080807E00,
	0x00010101017E0100, 0x00020202027C0200, 0x00040404047A0400, 0x0008080808760800,
	0x00101010106E1000, 0x00202020205E2000, 0x00404040403E4000, 0x00808080807E8000,
	0x000101017E010100, 0x000202027C020200, 0x000404047A040400, 0x0008080876080800,
	0x001010106E101000, 0x002020205E202000, 0x004040403E404000, 0x008080807E808000,
	0x0001017E01010100, 0x0002027C02020200, 0x0004047A04040400, 0x0008087608080800,
	0x0010106E10101000, 0x0020205E20202000, 0x0040403E40404000, 0x0080807E80808000,
	0x00017E0101010100, 0x00027C0202020200, 0x00047A0404040400, 0x0008760808080800,
	0x00106E1010101000, 0x00205E2020202000, 0x00403E4040404000, 0x00807E8080808000,
	0x007E010101010100, 0x007C020202020200, 0x007A040404040400, 0x0076080808080800,
	0x006E101010101000, 0x005E202020202000, 0x003E404040404000, 0x007E808080808000,
	0x7E01010101010100, 0x7C02020202020200, 0x7A04040404040400, 0x7608080808080800,
	0x6E10101010101000, 0x5E20202020202000, 0x3E40404040404000, 0x7E80808080808000
};

//my original tables for bishops
int MoveGenerator::BishopRelevantBits[64]=
{
	58, 59, 59, 59, 59, 59, 59, 58,
	59, 59, 59, 59, 59, 59, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 59, 59, 59, 59, 59, 59,
	58, 59, 59, 59, 59, 59, 59, 58
};

uint64_t MoveGenerator::BishopMagics[64]=
{
	0x0002020202020200, 0x0002020202020000, 0x0004010202000000, 0x0004040080000000,
	0x0001104000000000, 0x0000821040000000, 0x0000410410400000, 0x0000104104104000,
	0x0000040404040400, 0x0000020202020200, 0x0000040102020000, 0x0000040400800000,
	0x0000011040000000, 0x0000008210400000, 0x0000004104104000, 0x0000002082082000,
	0x0004000808080800, 0x0002000404040400, 0x0001000202020200, 0x0000800802004000,
	0x0000800400A00000, 0x0000200100884000, 0x0000400082082000, 0x0000200041041000,
	0x0002080010101000, 0x0001040008080800, 0x0000208004010400, 0x0000404004010200,
	0x0000840000802000, 0x0000404002011000, 0x0000808001041000, 0x0000404000820800,
	0x0001041000202000, 0x0000820800101000, 0x0000104400080800, 0x0000020080080080,
	0x0000404040040100, 0x0000808100020100, 0x0001010100020800, 0x0000808080010400,
	0x0000820820004000, 0x0000410410002000, 0x0000082088001000, 0x0000002011000800,
	0x0000080100400400, 0x0001010101000200, 0x0002020202000400, 0x0001010101000200,
	0x0000410410400000, 0x0000208208200000, 0x0000002084100000, 0x0000000020880000,
	0x0000001002020000, 0x0000040408020000, 0x0004040404040000, 0x0002020202020000,
	0x0000104104104000, 0x0000002082082000, 0x0000000020841000, 0x0000000000208800,
	0x0000000010020200, 0x0000000404080200, 0x0000040404040400, 0x0002020202020200
};


uint64_t MoveGenerator::BishopMasks[64]=
{
	0x0040201008040200, 0x0000402010080400, 0x0000004020100A00, 0x0000000040221400,
	0x0000000002442800, 0x0000000204085000, 0x0000020408102000, 0x0002040810204000,
	0x0020100804020000, 0x0040201008040000, 0x00004020100A0000, 0x0000004022140000,
	0x0000000244280000, 0x0000020408500000, 0x0002040810200000, 0x0004081020400000,
	0x0010080402000200, 0x0020100804000400, 0x004020100A000A00, 0x0000402214001400,
	0x0000024428002800, 0x0002040850005000, 0x0004081020002000, 0x0008102040004000,
	0x0008040200020400, 0x0010080400040800, 0x0020100A000A1000, 0x0040221400142200,
	0x0002442800284400, 0x0004085000500800, 0x0008102000201000, 0x0010204000402000,
	0x0004020002040800, 0x0008040004081000, 0x00100A000A102000, 0x0022140014224000,
	0x0044280028440200, 0x0008500050080400, 0x0010200020100800, 0x0020400040201000,
	0x0002000204081000, 0x0004000408102000, 0x000A000A10204000, 0x0014001422400000,
	0x0028002844020000, 0x0050005008040200, 0x0020002010080400, 0x0040004020100800,
	0x0000020408102000, 0x0000040810204000, 0x00000A1020400000, 0x0000142240000000,
	0x0000284402000000, 0x0000500804020000, 0x0000201008040200, 0x0000402010080400,
	0x0002040810204000, 0x0004081020400000, 0x000A102040000000, 0x0014224000000000,
	0x0028440200000000, 0x0050080402000000, 0x0020100804020000, 0x0040201008040200
};



void MoveGenerator::initKnightAttacks(){
    for(int sq=0;sq<64;sq++){
        int r = sq/8, f = sq%8;
        uint64_t attacks = 0;
        // all 8 possible jumps
        int dr[8] = {2,1,-1,-2,-2,-1,1,2};
        int df[8] = {1,2,2,1,-1,-2,-2,-1};
        for(int i=0;i<8;i++){
            int nr=r+dr[i], nf=f+df[i];
            if(nr>=0 && nr<8 && nf>=0 && nf<8) attacks |= 1ULL<<(nr*8+nf);
        }
        knightAttacks[sq] = attacks;
    }
}

void MoveGenerator::initKingAttacks(){
    for(int sq=0;sq<64;sq++){
        int r = sq/8, f = sq%8;
        uint64_t attacks = 0;
        // all 8 possible jumps
        int dr[8] = { 1,  1,  0, -1, -1, -1,  0,  1 };
        int df[8] = { 0,  1,  1,  1,  0, -1, -1, -1 };

        for(int i=0;i<8;i++){
            int nr=r+dr[i], nf=f+df[i];
            if(nr>=0 && nr<8 && nf>=0 && nf<8) attacks |= 1ULL<<(nr*8+nf);
        }
        kingAttacks[sq] = attacks;
    }
}

void MoveGenerator::initSlidingAttacks(){

    for(int sq=0; sq<64; sq++) {
        
        // Masks
        RookMasks[sq] = generateRookMask(sq);
        BishopMasks[sq] = generateBishopMask(sq);
        

        // Rook occupancy
        auto rookOccs = generateOccupancies(RookMasks[sq]);
        for(auto occ : rookOccs) {
            int index = (int)(((occ & RookMasks[sq]) * RookMagics[sq]) >> (RookRelevantBits[sq]));
            if (index >= 4096){std::cerr << index << std::endl;}
            RookAttackTable[sq][index] = rookAttacksOnTheFly(sq, occ);
        }
        
        // Bishop occupancy
        auto bishopOccs = generateOccupancies(BishopMasks[sq]);
        for(auto occ : bishopOccs) {
            int index = (int)(((occ & BishopMasks[sq]) * BishopMagics[sq]) >> (BishopRelevantBits[sq]));
            BishopAttackTable[sq][index] = bishopAttacksOnTheFly(sq, occ);
        }
    }
}

uint64_t MoveGenerator::WhitePawnPush[64] = {};    // single-square push
uint64_t MoveGenerator::WhitePawnDouble[64] = {};  // double push (only from rank 2)
uint64_t MoveGenerator::WhitePawnAttacks[64] = {};

uint64_t MoveGenerator::BlackPawnPush[64] = {};    // single-square push
uint64_t MoveGenerator::BlackPawnDouble[64] = {};  // double push (only from rank 2)
uint64_t MoveGenerator::BlackPawnAttacks[64] = {};

void MoveGenerator::initPawnAttacks(){
    for (int row = 0; row < 8; row++){
        for (int col = 0; col < 8; col++){
            int bitIndex = (7 - row) * 8 + col;

            uint64_t whitePush = 0ULL;
            uint64_t whiteDouble = 0ULL;
            uint64_t whiteAttack = 0ULL;

            // Single push (one rank up)
            if (row > 0) {
                whitePush |= 1ULL << (bitIndex + 8);
            }

            // Double push from rank 2 (row = 6)
            if (row == 6) {
                whiteDouble |= 1ULL << (bitIndex + 16);
            }

            // Diagonal captures
            if (row > 0 && col > 0) {
                whiteAttack |= 1ULL << (bitIndex + 7);  // left capture
            }
            if (row > 0 && col < 7) {
                whiteAttack |= 1ULL << (bitIndex + 9);  // right capture
            }

            uint64_t blackPush = 0ULL;
            uint64_t blackDouble = 0ULL;
            uint64_t blackAttack = 0ULL;

            // Single push (one rank up)
            if (row < 7) {
                blackPush |= 1ULL << (bitIndex - 8);
            }

            // Double push from rank 2 (row = 6)
            if (row == 1) {
                blackDouble |= 1ULL << (bitIndex - 16);
            }

            // Diagonal captures
            if (row < 7  && col < 7) {
                blackAttack |= 1ULL << (bitIndex - 7);  // left capture
            }
            if (row < 7 && col > 0 ) {
                blackAttack |= 1ULL << (bitIndex - 9);  // right capture
            }

            WhitePawnPush[bitIndex] = whitePush;
            WhitePawnDouble[bitIndex] = whiteDouble;  // double push (only from rank 2)
            WhitePawnAttacks[bitIndex] = whiteAttack;

            BlackPawnPush[bitIndex] = blackPush;    // single-square push
            BlackPawnDouble[bitIndex] = blackDouble;  // double push (only from rank 2)
            BlackPawnAttacks[bitIndex] = blackAttack;
            
        }
    }

}

uint64_t MoveGenerator::generateRookMask(int square) {
    uint64_t attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    // horizontal
    for (int f = file + 1; f <= 6; f++) attacks |= 1ULL << (rank*8 + f);
    for (int f = file - 1; f >= 1; f--) attacks |= 1ULL << (rank*8 + f);

    // vertical
    for (int r = rank + 1; r <= 6; r++) attacks |= 1ULL << (r*8 + file);
    for (int r = rank - 1; r >= 1; r--) attacks |= 1ULL << (r*8 + file);
    return attacks;
}

uint64_t MoveGenerator::generateBishopMask(int square) {
    uint64_t attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    // NE
    for (int r=rank+1,f=file+1; r<=6 && f<=6; r++, f++) attacks |= 1ULL << (r*8+f);
    // NW
    for (int r=rank+1,f=file-1; r<=6 && f>=1; r++, f--) attacks |= 1ULL << (r*8+f);
    // SE
    for (int r=rank-1,f=file+1; r>=1 && f<=6; r--, f++) attacks |= 1ULL << (r*8+f);
    // SW
    for (int r=rank-1,f=file-1; r>=1 && f>=1; r--, f--) attacks |= 1ULL << (r*8+f);

    return attacks;
}

std::vector<uint64_t> MoveGenerator::generateOccupancies(uint64_t mask){
    std::vector<uint64_t> occupancies;
    int bits[12]; // maximum relevant squares
    int numBits = 0;

    for (int i=0; i<64; i++)
        if (mask & (1ULL<<i)) bits[numBits++] = i;

    int count = 1 << numBits; // 2^numBits
    for (int index=0; index<count; index++) {
        uint64_t occ = 0;
        for (int j=0; j<numBits; j++)
            if (index & (1<<j)) occ |= 1ULL << bits[j];
        occupancies.push_back(occ);
    }
    return occupancies;
}

uint64_t MoveGenerator::rookAttacksOnTheFly(int square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    // rays
    for (int r = rank+1; r < 8; r++) { int sq = r*8+file; attacks |= 1ULL<<sq; if (blockers & (1ULL<<sq)) break; }
    for (int r = rank-1; r >= 0; r--) { int sq = r*8+file; attacks |= 1ULL<<sq; if (blockers & (1ULL<<sq)) break; }
    for (int f = file+1; f < 8; f++) { int sq = rank*8+f; attacks |= 1ULL<<sq; if (blockers & (1ULL<<sq)) break; }
    for (int f = file-1; f >= 0; f--) { int sq = rank*8+f; attacks |= 1ULL<<sq; if (blockers & (1ULL<<sq)) break; }

    return attacks;
}

uint64_t MoveGenerator::bishopAttacksOnTheFly(int sq, uint64_t blockers) {
    uint64_t attacks=0ULL;
    int rank=sq/8, file=sq%8;

    for(int r=rank+1,f=file+1; r<8 && f<8; r++,f++){ attacks|=1ULL<<(r*8+f); if(blockers & (1ULL<<(r*8+f))) break; }
    for(int r=rank+1,f=file-1; r<8 && f>=0; r++,f--){ attacks|=1ULL<<(r*8+f); if(blockers & (1ULL<<(r*8+f))) break; }
    for(int r=rank-1,f=file+1; r>=0 && f<8; r--,f++){ attacks|=1ULL<<(r*8+f); if(blockers & (1ULL<<(r*8+f))) break; }
    for(int r=rank-1,f=file-1; r>=0 && f>=0; r--,f--){ attacks|=1ULL<<(r*8+f); if(blockers & (1ULL<<(r*8+f))) break; }

    return attacks;
}

//IMPORTANT
//-------------------------
//STILL HAVE TO MASK OUT OWN PIECES attacks & ~ownPieces
uint64_t MoveGenerator::getRookAttacks(int square, uint64_t occupancy) {
    uint64_t blockers = occupancy & RookMasks[square];
    uint64_t index = (blockers * RookMagics[square]) >> (RookRelevantBits[square]);

    return RookAttackTable[square][index];

}

uint64_t MoveGenerator::getBishopAttacks(int square, uint64_t occupancy) {
    uint64_t blockers = occupancy & BishopMasks[square];
    uint64_t index = (blockers * BishopMagics[square]) >> (BishopRelevantBits[square]);

    return BishopAttackTable[square][index];
}

uint64_t MoveGenerator::getQueenAttacks(int square, uint64_t occupancy) {
    return getBishopAttacks(square,occupancy) | getRookAttacks(square,occupancy);
}

uint64_t MoveGenerator::getPawnAttacks(int square, uint64_t combinedSame, uint64_t combinedOpposite) const{
    
    if (board.whiteToMove){
        uint64_t ans = 0ULL;
        if (board.enPassantSquare != -1 && (WhitePawnAttacks[square] & (1ULL << board.enPassantSquare))) {
            ans |= 1ULL << board.enPassantSquare;
        }
        
        return ans | ((WhitePawnPush[square] | WhitePawnDouble[square]) &  ~ (combinedSame | combinedOpposite) )|
                (WhitePawnAttacks[square] & (combinedOpposite));
    }
    else{
        uint64_t ans = 0ULL;
        if (board.enPassantSquare != -1 && (BlackPawnAttacks[square] & (1ULL << board.enPassantSquare))) {
            ans |= 1ULL << board.enPassantSquare;
        }
        
        return ans | ((BlackPawnPush[square] | BlackPawnDouble[square]) &  ~ (combinedSame | combinedOpposite) )|
                (BlackPawnAttacks[square] & (combinedOpposite));
    }
}
