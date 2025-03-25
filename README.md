Simple chess engine in c++
doesnt support UCI interface (yet)
Require c++ 14 and up (for gtest)
goal is 2k5 elo (for now)

## Desired feature (in the work)

- **Parallelization Support:**
  - **Move Generation and Alpha-Beta Searching:** generate legal moves and explore multiple branches of the game tree in parallel to improve search depth and speed. Using openMP.

- **Monte-Carlo Tree Search (MCTS):**
  - Integrate MCTS to handle positions with a high branching factor that traditional search methods might struggle with.

- **Thorough Testing and Benchmarking:**
  - Late game need more testing for sure

- **Handling opening book:** 
  - not in immediate future 







