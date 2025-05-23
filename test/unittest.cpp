#include <gtest/gtest.h>
#include "movegen.h"
#include "search.h"

class BoardTest : public testing::Test {
protected:
    BoardTest() {
        INITIALIZE::Mask();
	    INITIALIZE::MoveData();
        chessBoard.ReadFENString(STANDARD);
        castlingBoard.ReadFENString("r1b1kb1r/1pppqp2/p1n2n1p/1B2p1p1/4P3/2NPBN2/PPP1QPPP/R3K2R w KQkq - 0 1");
    }
    BOARD_C fenTestBoard;
    BOARD_C chessBoard;
    BOARD_C castlingBoard;

    std::string invalidFEN1 = "r1b1kb1r/p1n2n1p/1B2p1p1/4P3/2NPBN2/PPP1QPPP/R3K2R w KQkq - 0 1"; //missing one rank
    std::string invalidFEN2 = "r1b1kb1r/1pppqp2/p1n2n1p/1B2p1p1/4P3/2NPBN2/PPP1QPPP/R3K2R KQkq - 0 1"; //missing side to move
    std::string invalidFEN3 = "rb1kb1r/1pppqp2/pn2n1p/1B2p1p1/4P3/2NPBN2/PPP1QPPP/R3K2R b KQkq - 0 1"; //missing square(s)
};

class AlphaBetaTest : public testing::Test {
protected:
    AlphaBetaTest() {
        INITIALIZE::Mask();
	    INITIALIZE::MoveData();
        InitZoBrist(true);
        mateIn4.ReadFENString(_mateIn4);
        mateIn1.ReadFENString(_mateIn1);
        mateIn3.ReadFENString(_mateIn3);
        mateIn5.ReadFENString(_mateIn5);
        puzzle1.ReadFENString(_puzzle1);
    }

    BOARD_C mateIn1;
    BOARD_C mateIn3;
    BOARD_C mateIn4;
    BOARD_C mateIn5;
    BOARD_C puzzle1;

    //Mate - mate position with clear best move.
    std::string _mateIn1 = "4rb2/3qrk2/1p1p1n2/7p/P2P4/4R2P/1BQN1P2/1K4R1 w - - 3 39";
    std::string _mateIn4 = "3qr2k/1p3rbp/2p3p1/p7/P2pBNn1/1P3n2/6P1/B1Q1RR1K b - - 1 30";
    std::string _mateIn3 = "r1bqk2r/2ppb1p1/n3P2p/8/2B1nP2/4P3/1PPP3P/RNBQK1NR w KQkq - 0 10";
    std::string _mateIn5 = "4rb1k/2pqn2p/6pn/ppp3N1/P1QP2b1/1P2p3/2B3PP/B3RRK1 w - - 0 24";  //use to benchmark more likely cause it might teak a bit of time currently to search to 10 (>10s)

    //PUZZLE - tricky position with clear best move.
    std::string _puzzle1 = "rnb1k2r/pp2b1pp/2p2pn1/q3P2Q/5p2/PB6/1BPP2PP/RN2K1NR w KQkq - 0 1";  //this one actually mate in..12 i believe
};

TEST_F(BoardTest, GenerateMove) {
    std::vector<ExtMove> moveList;
    moveList = GENERATE::AllMoves(chessBoard, WHITE);
    ASSERT_EQ(moveList.size(), 20);
}

TEST_F(BoardTest, CastlingMove){
    std::vector<ExtMove> moveList;
    moveList  = GENERATE::SpecialMoves(castlingBoard, castlingBoard.Side_to_move);
    ASSERT_EQ(moveList.size(), 2);
    ASSERT_EQ(moveList[0].getFlags(), MoveType::KING_CASTLE);
    ASSERT_EQ(moveList[1].getFlags(), MoveType::QUEEN_CASTLE);
}

TEST_F(BoardTest, ReadFenString){
    bool test1 = fenTestBoard.ReadFENString(invalidFEN1);
    bool test2 = fenTestBoard.ReadFENString(invalidFEN2);
    bool test3 = fenTestBoard.ReadFENString(invalidFEN3);
    bool test4 = fenTestBoard.ReadFENString(STANDARD);

    ASSERT_EQ(test1, false);
    ASSERT_EQ(test2, false);
    ASSERT_EQ(test3, false);
    ASSERT_EQ(test4, true);

}

TEST_F(BoardTest, Enpassant){
    //Need to fix fenstring reading to account for previous move -> enpassant condition
}

TEST_F(BoardTest, MakeMove){
    ExtMove newMove;
    newMove.move = 0x2FBC; //castling move;
    BOARD_C newBoard;
    newBoard = castlingBoard.MakeMove(newMove);
    std::string fenString = newBoard.ToFENString();
    EXPECT_TRUE(fenString == "r1b1kb1r/1pppqp2/p1n2n1p/1B2p1p1/4P3/2NPBN2/PPP1QPPP/R4RK1 b kq - 0 1");
}

//Testing Board class
TEST_F(BoardTest, Behaviours){
    std::string fenString = chessBoard.ToFENString();
    EXPECT_TRUE(fenString == STANDARD);
}

//correctness testing since, well, there can only be a fixed number of correct move in a mate situation
TEST_F(AlphaBetaTest, MateIn1){
    ExtMove result;
    Search alphaBeta;
    alphaBeta.SetTableSize(1000000);
    alphaBeta.SetPosition(mateIn1);
    result = alphaBeta.IterativeDeepening(6, true);
    ASSERT_EQ(result.move, 1458); //1458
}

TEST_F(AlphaBetaTest, MateIn3){
}

//takes too long for a test
TEST_F(AlphaBetaTest, MateIn5){
    //417d
    Search alphaBeta;
    ExtMove result;
    alphaBeta.SetTableSize(30000000);
    alphaBeta.SetPosition(mateIn5);
    result = alphaBeta.IterativeDeepening(10, true);
    ASSERT_EQ(result.move, 0x417d);
}

TEST_F(AlphaBetaTest, MateIn10){
    //0x369
    Search alphaBeta;
    ExtMove result;
    alphaBeta.SetTableSize(30000000);
    alphaBeta.SetPosition(puzzle1);
    result = alphaBeta.IterativeDeepening(10, true);
    ASSERT_EQ(result.move, 0x369);
}