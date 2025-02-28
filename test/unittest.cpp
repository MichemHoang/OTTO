#include <gtest/gtest.h>
#include "movegen.h"

class BoardTest : public testing::Test {
protected:
    BoardTest() {
        INITIALIZE::Mask();
	    INITIALIZE::MoveData();
        chessBoard.readFENString(STANDARD);
    }
    BOARD_C chessBoard;

};

class AlphaBetaTest : public testing::Test {
protected:
    AlphaBetaTest() {

    }

    BOARD_C mateIn8;
    BOARD_C mateIn9;
    BOARD_C mateIn2;

    std::string _mateIn8;
    std::string _mateIn9;
    std::string _mateIn10;
};

TEST_F(BoardTest, StandardBoard) {
    std::vector<ExtMove> moveList;
    std::string fenString = chessBoard.toFENString();
    moveList = GENERATE::AllMoves(chessBoard, WHITE);
    ASSERT_EQ(moveList.size(), 20);

}

TEST_F(BoardTest, CastlingMove){
}

TEST_F(BoardTest, Enpassant){
}

//Testing Board class
TEST_F(BoardTest, Behaviours){
    std::string fenString = chessBoard.toFENString();
    EXPECT_TRUE(fenString == STANDARD) ;
}

//correctness testing since, well, there can only be a fixed number of correct move in a mate situation
TEST_F(AlphaBetaTest, MateIn4){
}

TEST_F(AlphaBetaTest, MateIn5){
}

TEST_F(AlphaBetaTest, MateIn6){
}

//世界が終わるまでは