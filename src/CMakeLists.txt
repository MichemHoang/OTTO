set(srcs
    main.cpp
    bitboard.h
    bitboard.cpp
    board.h
    board.cpp
    evaluation.cpp
    evaluation.h
    movegen.cpp
    movegen.h
    mt64.cpp
    mt64.h
    openbook.h
    openbook.cpp
    search.cpp
    search.h
    game.cpp
    game.h
    transposition.h
    typedefs.h
    zobristhash.h
    zobristhash.cpp
)
add_executable( OTTO ${srcs})
target_link_libraries( OTTO ${OpenCV_LIBS} pthread)
