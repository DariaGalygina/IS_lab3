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

const int BOARD_SIZE_BFS = 5;
const int TOTAL_CELLS_BFS = BOARD_SIZE_BFS * BOARD_SIZE_BFS;

const int BOARD_SIZE_IDS = 5;
const int TOTAL_CELLS_IDS = BOARD_SIZE_IDS * BOARD_SIZE_IDS;

const int BOARD_SIZE_LARGE = 8;
const int TOTAL_CELLS_LARGE = BOARD_SIZE_LARGE * BOARD_SIZE_LARGE;

// Возможные ходы коня
const pair<int, int> knightMoves[8] = {
    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
    {1, -2}, {1, 2}, {2, -1}, {2, 1}};

using BoardBFS = array<int, TOTAL_CELLS_BFS>;
using BoardIDS = array<int, TOTAL_CELLS_IDS>;
using BoardLarge = array<int, TOTAL_CELLS_LARGE>;

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
    const int MAX_STATES = 10000000;

    auto start_time = chrono::high_resolution_clock::now();

    while (!q.empty() && states_explored < MAX_STATES) {
        BFSState current = q.front();    // первое состояние в очереди
        q.pop();
        states_explored++;

        if (current.move_count == TOTAL_CELLS_BFS - 1) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "BFS 5x5: решение найдено" << endl;
            cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;
            return current.board;
        }

        int x = current.pos % BOARD_SIZE_BFS;
        int y = current.pos / BOARD_SIZE_BFS;

        for (int i = 0; i < 8; i++) {
            int nx = x + knightMoves[i].first;
            int ny = y + knightMoves[i].second;

            if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE_BFS && ny < BOARD_SIZE_BFS) {
                int next_pos = ny * BOARD_SIZE_BFS + nx;    // перевод координат в номер строки

                if (current.board[next_pos] == -1) {        // О
                    BoardBFS new_board = current.board;
                    new_board[next_pos] = current.move_count + 1;

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

bool ids_5x5(BoardIDS& board, int pos, int moveNumber, int max_depth) {
    if (moveNumber == TOTAL_CELLS_IDS - 1) return true;
    if (moveNumber >= max_depth) return false;

    int x = pos % BOARD_SIZE_IDS;
    int y = pos / BOARD_SIZE_IDS;

    for (int i = 0; i < 8; i++) {    // 8 ходов коня
        int nx = x + knightMoves[i].first;
        int ny = y + knightMoves[i].second;

        if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE_IDS && ny < BOARD_SIZE_IDS) {   // границы
            int next = ny * BOARD_SIZE_IDS + nx;
            if (board[next] == -1) {
                board[next] = moveNumber + 1;
                if (ids_5x5(board, next, moveNumber + 1, max_depth)) return true;
                board[next] = -1;
            }
        }
    }
    return false;
}

BoardIDS idsKnightTour5x5(int startPos) {
    BoardIDS board;
    board.fill(-1);
    board[startPos] = 0;

    auto start_time = chrono::high_resolution_clock::now();

    for (int d = 1; d <= TOTAL_CELLS_IDS; ++d) {
        BoardIDS current_board = board;
        if (ids_5x5(current_board, startPos, 0, d)) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "IDS 5x5: решение найдено" << endl;
            cout << "Глубина: " << d << ", время: " << duration.count() << " мс" << endl;
            return current_board;
        }
    }

    BoardIDS empty;
    empty.fill(-1);
    cout << "IDS 5x5: решение не найдено" << endl;
    return empty;
}

bool dfs_8x8(BoardLarge& board, int pos, int moveNumber) {
    if (moveNumber == TOTAL_CELLS_LARGE - 1) return true;

    int x = pos % BOARD_SIZE_LARGE;
    int y = pos / BOARD_SIZE_LARGE;

    for (int i = 0; i < 8; i++) {
        int nx = x + knightMoves[i].first;
        int ny = y + knightMoves[i].second;

        if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE_LARGE && ny < BOARD_SIZE_LARGE) {
            int next = ny * BOARD_SIZE_LARGE + nx;
            if (board[next] == -1) {
                board[next] = moveNumber + 1;
                if (dfs_8x8(board, next, moveNumber + 1)) return true;
                board[next] = -1;
            }
        }
    }
    return false;
}

BoardLarge dfsKnightTour8x8(int startPos) {
    BoardLarge board;
    board.fill(-1);
    board[startPos] = 0;

    auto start = chrono::high_resolution_clock::now();
    bool found = dfs_8x8(board, startPos, 0);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "DFS 8x8: " << (found ? "решение найдено" : "решение не найдено");
    cout << " за " << duration.count() << " мс" << endl;

    if (!found) board.fill(-1);
    return board;
}

struct AStarState {
    BoardLarge board;
    int pos;
    int move_count;
    int g;
    int h;

    AStarState(BoardLarge b, int p, int m, int g_val, int h_val)
        : board(b), pos(p), move_count(m), g(g_val), h(h_val) {}

    friend bool operator<(const AStarState& n1, const AStarState& n2) {
        return (n1.g + n1.h) > (n2.g + n2.h);
    }
};

int remaining_cells_heuristic(const BoardLarge& board, int move_count) {
    return TOTAL_CELLS_LARGE - move_count - 1;
}

string board_large_to_hash(const BoardLarge& board) {
    string hash;
    for (int cell : board) {
        hash += to_string(cell) + "|";
    }
    return hash;
}

BoardLarge astarKnightTour8x8(int startPos) {
    BoardLarge initial_board;
    initial_board.fill(-1);
    initial_board[startPos] = 0;

    priority_queue<AStarState> q;
    unordered_set<string> visited;

    int start_heuristic = remaining_cells_heuristic(initial_board, 0);
    q.push(AStarState(initial_board, startPos, 0, 0, start_heuristic));

    int states_explored = 0;
    const int MAX_STATES = 500000;

    auto start_time = chrono::high_resolution_clock::now();

    while (!q.empty() && states_explored < MAX_STATES) {
        AStarState current = q.top();
        q.pop();
        states_explored++;

        if (current.move_count == TOTAL_CELLS_LARGE - 1) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
            cout << "A* 8x8: решение найдено" << endl;
            cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;
            return current.board;
        }

        string state_hash = board_large_to_hash(current.board) + to_string(current.pos);

        if (visited.find(state_hash) != visited.end()) continue;
        visited.insert(state_hash);

        int x = current.pos % BOARD_SIZE_LARGE;
        int y = current.pos / BOARD_SIZE_LARGE;

        for (int i = 0; i < 8; i++) {
            int nx = x + knightMoves[i].first;
            int ny = y + knightMoves[i].second;

            if (nx >= 0 && ny >= 0 && nx < BOARD_SIZE_LARGE && ny < BOARD_SIZE_LARGE) {
                int next_pos = ny * BOARD_SIZE_LARGE + nx;
                if (current.board[next_pos] == -1) {
                    BoardLarge new_board = current.board;
                    new_board[next_pos] = current.move_count + 1;

                    int new_heuristic = remaining_cells_heuristic(new_board, current.move_count + 1);
                    AStarState new_state(new_board, next_pos, current.move_count + 1,
                        current.g + 1, new_heuristic);

                    string new_hash = board_large_to_hash(new_board) + to_string(next_pos);

                    if (visited.find(new_hash) == visited.end()) {
                        q.push(new_state);
                    }
                }
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "A* 8x8: решение не найдено" << endl;
    cout << "Состояний: " << states_explored << ", время: " << duration.count() << " мс" << endl;

    BoardLarge empty;
    empty.fill(-1);
    return empty;
}

template<typename BoardType, int Size>
bool validate_solution(const BoardType& board, int board_size) {
    vector<bool> visited(Size, false);

    for (int i = 0; i < Size; ++i) {
        if (board[i] == -1) return false;             // все клетки заполнены
        if (board[i] < 0 || board[i] >= Size) return false;
        if (visited[board[i]]) return false;
        visited[board[i]] = true;
    }

    vector<int> positions(Size);
    for (int i = 0; i < Size; ++i) {
        positions[board[i]] = i;
    }

    for (int i = 0; i < Size - 1; ++i) {
        int pos1 = positions[i];
        int pos2 = positions[i + 1];
        int x1 = pos1 % board_size, y1 = pos1 / board_size;
        int x2 = pos2 % board_size, y2 = pos2 / board_size;
        int dx = abs(x1 - x2), dy = abs(y1 - y2);
        if (!((dx == 1 && dy == 2) || (dx == 2 && dy == 1))) return false;
    }

    return true;
}

template<typename BoardType, int Size>
void print_solution(const BoardType& board, const string& algorithm_name, int board_size) {
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

template<typename BoardType, int Size>
bool is_board_empty(const BoardType& board) {
    for (int i = 0; i < Size; i++) {
        if (board[i] != -1) return false;
    }
    return true;
}

int main() {
    setlocale(LC_ALL, "ru_RU");

    cout << "АНАЛИЗ АЛГОРИТМОВ" << endl;

    int startPos5x5 = 0;
    int startPos8x8 = 27;

    cout << "BFS для доски 5x5..." << endl;
    auto bfs_result = bfsKnightTour5x5(startPos5x5);
    bool bfs_valid = !is_board_empty<BoardBFS, TOTAL_CELLS_BFS>(bfs_result) &&
        validate_solution<BoardBFS, TOTAL_CELLS_BFS>(bfs_result, BOARD_SIZE_BFS);

    cout << "IDS для доски 5x5..." << endl;
    auto ids_result = idsKnightTour5x5(startPos5x5);
    bool ids_valid = !is_board_empty<BoardIDS, TOTAL_CELLS_IDS>(ids_result) &&
        validate_solution<BoardIDS, TOTAL_CELLS_IDS>(ids_result, BOARD_SIZE_IDS);

    cout << "DFS для доски 8x8..." << endl;
    auto dfs_result = dfsKnightTour8x8(startPos8x8);
    bool dfs_valid = !is_board_empty<BoardLarge, TOTAL_CELLS_LARGE>(dfs_result) &&
        validate_solution<BoardLarge, TOTAL_CELLS_LARGE>(dfs_result, BOARD_SIZE_LARGE);

    cout << "A* для доски 8x8..." << endl;
    auto astar_result = astarKnightTour8x8(startPos8x8);
    bool astar_valid = !is_board_empty<BoardLarge, TOTAL_CELLS_LARGE>(astar_result) &&
        validate_solution<BoardLarge, TOTAL_CELLS_LARGE>(astar_result, BOARD_SIZE_LARGE);

    cout << "НАЙДЕННЫЕ РЕШЕНИЯ:" << endl;

    if (bfs_valid) print_solution<BoardBFS, TOTAL_CELLS_BFS>(bfs_result, "BFS для доски 5x5", BOARD_SIZE_BFS);
    if (ids_valid) print_solution<BoardIDS, TOTAL_CELLS_IDS>(ids_result, "IDS для доски 5x5", BOARD_SIZE_IDS);
    if (dfs_valid) print_solution<BoardLarge, TOTAL_CELLS_LARGE>(dfs_result, "DFS для доски 8x8", BOARD_SIZE_LARGE);
    if (astar_valid) print_solution<BoardLarge, TOTAL_CELLS_LARGE>(astar_result, "A* для доски 8x8", BOARD_SIZE_LARGE);

    return 0;
}