set(srcs
    Main.cpp
    BitBoard.h
    BitBoard.cpp
    evaluation.cpp
    evaluation.h
    MoveGen.cpp
    MoveGen.h
    mt64.cpp
    mt64.h
    OpenBook.h
    OpenBook.cpp
    search.cpp
    search.h
    Thread.cpp
    Thread.h
    Transposition.h
    Transposition.cpp
    typedefs.h
    ZobristHash.h
    ZobristHash.cpp
)
add_executable( OTTO ${srcs})
target_link_libraries( OTTO ${OpenCV_LIBS} )
