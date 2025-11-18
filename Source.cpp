#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <string>
#include <chrono>
#include <cmath>
#include <climits>
#include <array>
#include <iomanip>

using namespace std;

const int BOARD_SIZE_MAIN = 8;
const int BOARD_SIZE_BFS = 5;

const int TOTAL_CELLS_MAIN = BOARD_SIZE_MAIN * BOARD_SIZE_MAIN;
const int TOTAL_CELLS_BFS = BOARD_SIZE_BFS * BOARD_SIZE_BFS;

const pair<int, int> knightMoves[8] = {
    {-2, -1}, {-2,  1}, {-1, -2}, {-1, 2},
    { 1, -2}, { 1,  2}, { 2, -1}, { 2, 1}
};

using BoardMain = array<int, TOTAL_CELLS_MAIN>;
using BoardBFS = array<int, TOTAL_CELLS_BFS>;

template<int BoardSize>
bool isValidPosition(int x, int y) {
    return x >= 0 && y >= 0 && x < BoardSize && y < BoardSize;
}

template<int BoardSize>
int positionToIndex(int x, int y) {
    return y * BoardSize + x;
}

template<int BoardSize>
void indexToPosition(int index, int& x, int& y) {
    x = index % BoardSize;
    y = index / BoardSize;
}

template<typename BoardType>
string boardToHash(const BoardType& board) {
    string hash;
    for (int cell : board) {
        hash += to_string(cell) + "|";
    }
    return hash;
}

template<typename BoardType, int Size>
bool isBoardEmpty(const BoardType& board) {
    for (int i = 0; i < Size; i++) {
        if (board[i] != -1) return false;
    }
    return true;
}

template<typename BoardType, int Size>
bool validateSolution(const BoardType& board, int board_size) {
    vector<bool> visited(Size, false);

    for (int i = 0; i < Size; ++i) {
        if (board[i] == -1) return false;
        if (board[i] < 0 || board[i] >= Size) return false;
        if (visited[board[i]]) return false; // уникальность
        visited[board[i]] = true;
    }

    // обратное отображение от номера хода к позиции на доске
    vector<int> positions(Size);
    for (int i = 0; i < Size; ++i) {
        positions[board[i]] = i;
    }

    // Г
    for (int i = 0; i < Size - 1; ++i) {
        int x1, y1, x2, y2;
        indexToPosition<BOARD_SIZE_MAIN>(positions[i], x1, y1);
        indexToPosition<BOARD_SIZE_MAIN>(positions[i + 1], x2, y2);

        int dx = abs(x1 - x2), dy = abs(y1 - y2);
        if (!((dx == 1 && dy == 2) || (dx == 2 && dy == 1))) {
            return false;
        }
    }

    return true;
}

template<typename BoardType, int Size>
void printSolution(const BoardType& board, const string& algorithm_name, int board_size) {
    cout << endl << algorithm_name << ":" << endl;
    cout << "Шахматная доска " << board_size << "x" << board_size << " (номера ходов):" << endl;

    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            int idx = i * board_size + j;
            cout << setw(3) << board[idx] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

template<int TotalCells>
int simpleHeuristic(const array<int, TotalCells>& board, int move_count) {
    return TotalCells - move_count - 1;
}

template<int TotalCells>
int myHeuristic(const array<int, TotalCells>& board, int move_count) {
    int base = TotalCells - move_count - 1;

    // Приоритет состояниям, где посещены углы
    // нет штрафов
    int bonus = 0;
    if (board[0] != -1) bonus -= 0.1;  // Посещен левый верхний 
    if (board[BOARD_SIZE_MAIN - 1] != -1) bonus -= 0.1;  // Посещен правый верхний
    if (board[BOARD_SIZE_MAIN * (BOARD_SIZE_MAIN - 1)] != -1) bonus -= 0.1;  // Посещен левый нижний  
    if (board[TotalCells - 1] != -1) bonus -= 0.1;  // Посещен правый нижний

    return base + bonus;
}

// Функция для применения правила Варнсдорфа 
template<typename BoardType, int BoardSize>
int countAvailableMoves(const BoardType& board, int pos) {
    int x, y;
    indexToPosition<BoardSize>(pos, x, y);
    int count = 0;

    for (const auto& move : knightMoves) {
        int nx = x + move.first;
        int ny = y + move.second;
        if (isValidPosition<BoardSize>(nx, ny)) {
            int next_pos = positionToIndex<BoardSize>(nx, ny);
            if (board[next_pos] == -1) {
                count++;
            }
        }
    }
    return count;
}

// BFS 
BoardBFS bfsKnightTour5x5(int startPos) {
    struct BFSState {
        BoardBFS board;
        int pos;
        int move_count;
        BFSState(BoardBFS b, int p, int m) : board(b), pos(p), move_count(m) {}
    };

    BoardBFS initial;
    initial.fill(-1);
    initial[startPos] = 0;

    queue<BFSState> q;
    unordered_set<string> visited;

    q.push(BFSState(initial, startPos, 0));

    string initial_hash;
    for (int cell : initial) initial_hash += to_string(cell) + "|";
    visited.insert(initial_hash);

    int states_explored = 0;

    auto start_time = chrono::high_resolution_clock::now();

    while (!q.empty()) {
        BFSState current = q.front();
        q.pop();
        states_explored++;

        if (current.move_count == TOTAL_CELLS_BFS - 1) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "BFS 5x5: решение найдено" << endl;
            cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;

            printSolution<BoardBFS, TOTAL_CELLS_BFS>(current.board, "BFS РЕЗУЛЬТАТ", BOARD_SIZE_BFS);
            return current.board;
        }

        int x = current.pos % BOARD_SIZE_BFS;
        int y = current.pos / BOARD_SIZE_BFS;

        for (int i = 0; i < 8; i++) {
            int nx = x + knightMoves[i].first;
            int ny = y + knightMoves[i].second;

            if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE_BFS && ny < BOARD_SIZE_BFS) {
                int next_pos = ny * BOARD_SIZE_BFS + nx;

                if (current.board[next_pos] == -1) {
                    BoardBFS new_board = current.board;
                    new_board[next_pos] = current.move_count + 1;  // Запись номера хода

                    string new_hash;
                    for (int cell : new_board) new_hash += to_string(cell) + "|";

                    if (visited.find(new_hash) == visited.end()) {
                        visited.insert(new_hash);
                        q.push(BFSState(new_board, next_pos, current.move_count + 1));
                    }
                }
            }
        }
    }
    BoardBFS empty;
    empty.fill(-1);
    cout << "BFS 5x5: решение не найдено (состояний: " << states_explored << ")" << endl;
    return empty;
}

// IDS
bool idsDLSWithWarnsdorff(BoardMain& board, int pos, int moveNumber, int max_depth) {
    if (moveNumber == TOTAL_CELLS_MAIN - 1) return true;
    if (moveNumber >= max_depth) return false;

    int x, y;
    indexToPosition<BOARD_SIZE_MAIN>(pos, x, y);

    vector<pair<int, int>> possible_moves;
    for (const auto& move : knightMoves) {
        int nx = x + move.first;
        int ny = y + move.second;
        if (isValidPosition<BOARD_SIZE_MAIN>(nx, ny)) {
            int next_pos = positionToIndex<BOARD_SIZE_MAIN>(nx, ny);
            if (board[next_pos] == -1) {
                int accessibility = countAvailableMoves<BoardMain, BOARD_SIZE_MAIN>(board, next_pos);
                possible_moves.push_back({ accessibility, next_pos });
            }
        }
    }

    sort(possible_moves.begin(), possible_moves.end());

    for (const auto& move : possible_moves) {
        int next_pos = move.second;
        board[next_pos] = moveNumber + 1;
        if (idsDLSWithWarnsdorff(board, next_pos, moveNumber + 1, max_depth)) {
            return true;
        }
        board[next_pos] = -1;
    }

    return false;
}

BoardMain idsKnightTour(int startPos) {
    BoardMain board;
    board.fill(-1);
    board[startPos] = 0;

    auto start_time = chrono::high_resolution_clock::now();

    BoardMain current_board = board;
    cout << "IDS: проверка полной глубины " << TOTAL_CELLS_MAIN << "..." << endl;

    if (idsDLSWithWarnsdorff(current_board, startPos, 0, TOTAL_CELLS_MAIN)) {
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        cout << "IDS: решение найдено" << endl;
        cout << "Время: " << duration.count() << " мс" << endl;

        printSolution<BoardMain, TOTAL_CELLS_MAIN>(current_board, "IDS РЕЗУЛЬТАТ", BOARD_SIZE_MAIN);
        return current_board;
    }

    BoardMain empty;
    empty.fill(-1);
    cout << "IDS: решение не найдено" << endl;
    return empty;
}

// DFS 
bool dfsWithWarnsdorff(BoardMain& board, int pos, int moveNumber) {
    if (moveNumber == TOTAL_CELLS_MAIN - 1) return true;

    int x, y;
    indexToPosition<BOARD_SIZE_MAIN>(pos, x, y);

    vector<pair<int, int>> possible_moves;
    for (const auto& move : knightMoves) {
        int nx = x + move.first;
        int ny = y + move.second;
        if (isValidPosition<BOARD_SIZE_MAIN>(nx, ny)) {
            int next_pos = positionToIndex<BOARD_SIZE_MAIN>(nx, ny);
            if (board[next_pos] == -1) {
                int accessibility = countAvailableMoves<BoardMain, BOARD_SIZE_MAIN>(board, next_pos);
                possible_moves.push_back({ accessibility, next_pos });
            }
        }
    }

    sort(possible_moves.begin(), possible_moves.end());

    for (const auto& move : possible_moves) {
        int next_pos = move.second;
        board[next_pos] = moveNumber + 1;
        if (dfsWithWarnsdorff(board, next_pos, moveNumber + 1)) {
            return true;
        }
        board[next_pos] = -1;
    }

    return false;
}

BoardMain dfsKnightTour(int startPos) {
    BoardMain board;
    board.fill(-1);
    board[startPos] = 0;

    auto start_time = chrono::high_resolution_clock::now();
    bool found = dfsWithWarnsdorff(board, startPos, 0);
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "DFS: " << (found ? "решение найдено" : "решение не найдено");
    cout << " за " << duration.count() << " мс" << endl;

    if (found) {
        printSolution<BoardMain, TOTAL_CELLS_MAIN>(board, "DFS РЕЗУЛЬТАТ", BOARD_SIZE_MAIN);
    }
    else {
        board.fill(-1);
    }

    return board;
}

// A*
struct AStarState {
    BoardMain board;
    int pos;
    int move_count;
    int g;
    int h;

    AStarState(const BoardMain& b, int p, int m, int g_val, int h_val)
        : board(b), pos(p), move_count(m), g(g_val), h(h_val) {}

    friend bool operator<(const AStarState& n1, const AStarState& n2) {
        return (n1.g + n1.h) > (n2.g + n2.h);
    }
};

BoardMain astarKnightTour(int startPos) {
    BoardMain initial_board;
    initial_board.fill(-1);
    initial_board[startPos] = 0;

    priority_queue<AStarState> open_set;
    unordered_set<string> visited;

    int start_heuristic = myHeuristic<TOTAL_CELLS_MAIN>(initial_board, 0);
    open_set.push(AStarState(initial_board, startPos, 0, 0, start_heuristic));

    int states_explored = 0;

    auto start_time = chrono::high_resolution_clock::now();

    while (!open_set.empty()) {
        AStarState current = open_set.top();
        open_set.pop();
        states_explored++;

        if (current.move_count == TOTAL_CELLS_MAIN - 1) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "A*: решение найдено" << endl;
            cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;

            printSolution<BoardMain, TOTAL_CELLS_MAIN>(current.board, "A* РЕЗУЛЬТАТ", BOARD_SIZE_MAIN);
            return current.board;
        }

        string state_hash = boardToHash(current.board) + to_string(current.pos);
        if (visited.find(state_hash) != visited.end()) continue;
        visited.insert(state_hash);

        int x, y;
        indexToPosition<BOARD_SIZE_MAIN>(current.pos, x, y);

        for (const auto& move : knightMoves) {
            int nx = x + move.first;
            int ny = y + move.second;

            if (isValidPosition<BOARD_SIZE_MAIN>(nx, ny)) {
                int next_pos = positionToIndex<BOARD_SIZE_MAIN>(nx, ny);

                if (current.board[next_pos] == -1) {
                    BoardMain new_board = current.board;
                    new_board[next_pos] = current.move_count + 1;

                    int new_heuristic = myHeuristic<TOTAL_CELLS_MAIN>(new_board, current.move_count + 1);
                    AStarState new_state(new_board, next_pos, current.move_count + 1,
                        current.g + 1, new_heuristic);

                    string new_hash = boardToHash(new_board) + to_string(next_pos);

                    if (visited.find(new_hash) == visited.end()) {
                        open_set.push(new_state);
                    }
                }
            }
        }

        if (states_explored % 10000 == 0) {
            cout << "A* прогресс: " << states_explored << " состояний..." << endl;
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "A*: решение не найдено" << endl;
    cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;

    BoardMain empty;
    empty.fill(-1);
    return empty;
}

int main() {
    setlocale(LC_ALL, "ru_RU");

    cout << "АНАЛИЗ АЛГОРИТМОВ ПОИСКА МАРШРУТА КОНЯ" << endl;
    cout << "=======================================" << endl;
    cout << "Всего клеток: " << TOTAL_CELLS_MAIN << endl;

    const int startPos8x8 = 27; // Центральная позиция для 8x8
    int startX8, startY8;
    indexToPosition<BOARD_SIZE_MAIN>(startPos8x8, startX8, startY8);
    cout << "Стартовая позиция: (" << startX8 << ", " << startY8 << ")" << endl << endl;

    cout << "1. Запуск DFS..." << endl;
    auto dfs_result = dfsKnightTour(startPos8x8);

    cout << endl << "2. Запуск A*..." << endl;
    auto astar_result = astarKnightTour(startPos8x8);

    cout << endl << "3. Запуск IDS..." << endl;
    auto ids_result = idsKnightTour(startPos8x8);

    cout << endl << "АЛГОРИТМ BFS:" << endl;
    cout << "=============================" << endl;
    cout << "Всего клеток: " << TOTAL_CELLS_BFS << endl;

    const int startPos5x5 = 0; // Левая верхняя позиция для 5x5 (0, 0)
    int startX5, startY5;
    indexToPosition<BOARD_SIZE_BFS>(startPos5x5, startX5, startY5);
    cout << "Стартовая позиция: (" << startX5 << ", " << startY5 << ")" << endl << endl;

    cout << "4. Запуск BFS..." << endl;
    auto bfs_result = bfsKnightTour5x5(startPos5x5);

    return 0;
}
