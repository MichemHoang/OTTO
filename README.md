Simple chess engine in c++
doesnt support UCI interface (yet)
Require c++ 14 and up (for gtest)
goal is 2k5 elo (for now)

## Desired feature (in the work)

- **Parallelization Support:**
  - **Move Generation** generate legal moves and quick evaluate for move ordering [X]
  - **Alpha-Beta Searching:** evaluate multiple branches of the game tree in parallel to improve search depth and speed.

- **Monte-Carlo Tree Search (MCTS):**
  - Integrate MCTS to handle positions with a high branching factor.

- **Handling opening book:** 
  - not in immediate future 







