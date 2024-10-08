#include <iostream>
#include <string>
#include <cmath>
#include <deque>
#include <cstdlib>
#include <windows.h>
#include <list>
#include <map>
#include <tuple>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <chrono>
#include <unordered_set>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <climits>
#include <algorithm>
#include <random>
#include <string>
#include <ctime>

int manhattan_distance_with_linear_conflict_and_corner(const std::string &state)
{
    int grid_size = 4;
    int distance = 0;
    int linear_conflict = 0;
    int corner_trap_penalty = 0;

    // Манхэттенское расстояние с линейными конфликтами
    for (int i = 0; i < state.size(); ++i)
    {
        char tile = state[i];
        if (tile != '0')
        {
            int value = (tile <= '9') ? tile - '1' : tile - 'A' + 9;
            int current_x = i % grid_size;
            int current_y = i / grid_size;
            int target_x = value % grid_size;
            int target_y = value / grid_size;

            // Манхэттенское расстояние
            distance += std::abs(current_x - target_x) + std::abs(current_y - target_y);

            // Линейный конфликт по строкам
            if (current_y == target_y)
            {
                for (int k = current_x + 1; k < grid_size; ++k)
                {
                    char other_tile = state[current_y * grid_size + k];
                    if (other_tile != '0')
                    {
                        int other_value = (other_tile <= '9') ? other_tile - '1' : other_tile - 'A' + 9;
                        int other_target_x = other_value % grid_size;
                        if (other_target_x < target_x) // Линейный конфликт
                        {
                            linear_conflict += 2;
                        }
                    }
                }
            }

            // Линейный конфликт по столбцам
            if (current_x == target_x)
            {
                for (int k = current_y + 1; k < grid_size; ++k)
                {
                    char other_tile = state[k * grid_size + current_x];
                    if (other_tile != '0')
                    {
                        int other_value = (other_tile <= '9') ? other_tile - '1' : other_tile - 'A' + 9;
                        int other_target_y = other_value / grid_size;
                        if (other_target_y < target_y) // Линейный конфликт
                        {
                            linear_conflict += 2;
                        }
                    }
                }
            }
        }
    }

    // "Пробки углов" (corner trapping)
    if (state[0] != '1' && (state[1] == '2' || state[4] == '5'))
    {
        corner_trap_penalty += 2; // Пробка в левом верхнем углу
    }
    if (state[3] != '4' && (state[2] == '3' || state[7] == '8'))
    {
        corner_trap_penalty += 2; // Пробка в правом верхнем углу
    }
    if (state[12] != 'D' && (state[8] == '9' || state[13] == 'E'))
    {
        corner_trap_penalty += 2; // Пробка в левом нижнем углу
    }
    if (state[15] != 'F' && (state[11] == 'B' || state[14] == 'E'))
    {
        corner_trap_penalty += 2; // Пробка в правом нижнем углу
    }

    return distance + linear_conflict + corner_trap_penalty;
}

int manhattan_distance_with_linear_conflict(const std::string &state)
{
    int grid_size = 4;
    int distance = 0;
    int linear_conflict = 0;

    for (int i = 0; i < state.size(); ++i)
    {
        char tile = state[i];
        if (tile != '0')
        {
            int value = (tile <= '9') ? tile - '1' : tile - 'A' + 9;
            int current_x = i % grid_size;
            int current_y = i / grid_size;
            int target_x = value % grid_size;
            int target_y = value / grid_size;

            // Манхэттенское расстояние
            distance += std::abs(current_x - target_x) + std::abs(current_y - target_y);

            // Проверка на линейный конфликт в строке
            if (current_y == target_y)
            {
                for (int k = current_x + 1; k < grid_size; ++k)
                {
                    char other_tile = state[current_y * grid_size + k];
                    if (other_tile != '0')
                    {
                        int other_value = (other_tile <= '9') ? other_tile - '1' : other_tile - 'A' + 9;
                        int other_target_x = other_value % grid_size;
                        if (other_target_x < target_x) // Линейный конфликт: плитка мешает
                        {
                            linear_conflict += 2;
                        }
                    }
                }
            }

            // Проверка на линейный конфликт в столбце
            if (current_x == target_x)
            {
                for (int k = current_y + 1; k < grid_size; ++k)
                {
                    char other_tile = state[k * grid_size + current_x];
                    if (other_tile != '0')
                    {
                        int other_value = (other_tile <= '9') ? other_tile - '1' : other_tile - 'A' + 9;
                        int other_target_y = other_value / grid_size;
                        if (other_target_y < target_y) // Линейный конфликт: плитка мешает
                        {
                            linear_conflict += 2;
                        }
                    }
                }
            }
        }
    }

    return distance + linear_conflict;
}

int manhattan_distance4x4(const std::string &state)
{
    int grid_size = 4;
    int distance = 0;

    for (int i = 0; i < state.size(); ++i)
    {
        char tile = state[i];
        if (tile != '0')
        {
            int value = (tile <= '9') ? tile - '1' : tile - 'A' + 9;
            int current_x = i % grid_size;
            int current_y = i / grid_size;
            int target_x = value % grid_size;
            int target_y = value / grid_size;

            distance += std::abs(current_x - target_x) + std::abs(current_y - target_y);
        }
    }
    return distance;
}
struct Node
{
    std::string condition;
    int last_position;
    int empty_index;
    int g;
    int f;

    Node(std::string cond, int lp, int ei, int steps) : condition(cond), last_position(lp), empty_index(ei), g(steps), f(steps + manhattan_distance4x4(cond)) {}
};

struct Compare
{
    bool operator()(const Node &n1, const Node &n2)
    {
        return n1.f > n2.f;
    }
};

std::string swap_positions(const std::string &s, int i1, int i2)
{
    std::string res = s;
    res[i1] = s[i2];
    res[i2] = s[i1];
    return res;
}

std::deque<Node> next_nodes(const Node &node, std::map<int, std::deque<int>> &moves)
{
    std::deque<Node> nodes;
    for (int i = 0; i < moves[node.empty_index].size(); ++i)
    {
        if (moves[node.empty_index][i] != node.last_position)
        {
            nodes.emplace_back(swap_positions(node.condition, moves[node.empty_index][i], node.empty_index), node.empty_index, moves[node.empty_index][i], node.g + 1);
        }
    }
    return nodes;
}

bool isSolvable4x4(const std::string &puzzle)
{
    int inversions = 0;
    int emptyRow = 0;

    std::vector<int> tiles;
    for (char c : puzzle)
    {
        if (c != '0')
        {
            tiles.push_back(c <= '9' ? c - '0' : c - 'A' + 10);
        }
    }

    for (size_t i = 0; i < tiles.size(); ++i)
    {
        for (size_t j = i + 1; j < tiles.size(); ++j)
        {
            if (tiles[i] > tiles[j])
            {
                ++inversions;
            }
        }
    }

    int emptyIndex = puzzle.find('0');
    emptyRow = 4 - (emptyIndex / 4);

    if (inversions % 2 == 0 && emptyRow % 2 != 0)
    {
        return true;
    }
    else if (inversions % 2 != 0 && emptyRow % 2 == 0)
    {
        return true;
    }

    return false;
}

size_t isPerfectSquare(size_t num)
{
    if (num == 0)
        return 0;

    double root = std::sqrt(static_cast<double>(num));

    size_t intRoot = static_cast<size_t>(std::round(root));

    if (intRoot * intRoot == num)
    {
        return intRoot;
    }
    else
    {
        return 0;
    }
}
void draw_field(const std::string &str, bool er = false, std::string info = "")
{

    if (er)
    {
        system("cls");
    }
    std::cout << info;
    size_t len = str.length();
    size_t root = isPerfectSquare(len);
    std::string layer = std::string(root * 3, '-');

    if (root == 0)
    {
        return;
    }

    std::cout << std::endl;
    for (size_t i = 0; i < root; ++i)
    {
        std::cout << layer << std::endl;
        for (size_t j = 0; j < root; ++j)
        {
            if (str[i * root + j] != '0')
            {
                std::cout << "|" << str[i * root + j] << "|";
            }
            else
            {
                std::cout << "   ";
            }
        }
        std::cout << std::endl;
        std::cout << layer << std::endl;
    }
    std::cout << std::endl;
}

std::string generate_random_puzzle()
{
    std::string puzzle = "ABCDEF0";
    std::mt19937 g(static_cast<unsigned int>(std::time(nullptr)));
    std::shuffle(puzzle.begin(), puzzle.end(), g);

    return "213456789" + puzzle;
}

Node create_start_node(std::string c)
{
    Node n(c, -1, c.find('0'), 0);
    return n;
}

std::deque<std::string> solve_game_A_star(Node start_node, std::map<int, std::deque<int>> &moves)
{
    std::string start_condition = start_node.condition;
    std::string target_condition = "123456789ABCDEF0";
    std::unordered_map<std::string, std::string> parent_map;

    std::deque<std::string> result;

    std::unordered_set<std::string> passed_values;
    std::priority_queue<Node, std::vector<Node>, Compare> pq;
    pq.push(start_node);

    while (true)
    {
        Node node = pq.top();
        pq.pop();
        passed_values.insert(node.condition);
        if (node.condition != target_condition)
        {
            auto nv = next_nodes(node, moves);
            for (const auto &n : nv)
            {
                if (passed_values.find(n.condition) == passed_values.end())
                {
                    parent_map[n.condition] = node.condition;
                    pq.push(n);
                }
            }
        }
        else
        {
            auto current_condition = node.condition;
            while (current_condition != start_condition)
            {
                result.push_front(current_condition);
                current_condition = parent_map[current_condition];
            }
            std::cout << result.size() << std::endl;
            break;
        }
    }

    return result;
}

std::deque<std::string> solve_game_IDA_star(Node start_node, std::map<int, std::deque<int>> &moves)
{
    std::string start_condition = start_node.condition;
    std::string target_condition = "123456789ABCDEF0";
    std::unordered_map<std::string, std::string> parent_map;
    std::deque<std::string> result;

    std::unordered_set<std::string> passed_values;

    std::function<int(const Node &, int)> search = [&](const Node &node, int bound)
    {
        int f = node.g + manhattan_distance4x4(node.condition);
        if (f > bound)
        {
            return f;
        }
        if (node.condition == target_condition)
        {
            return -1; // Решение найдено
        }

        int min_threshold = INT_MAX;

        auto next_nodes_list = next_nodes(node, moves);
        for (const auto &next_node : next_nodes_list)
        {
            if (passed_values.find(next_node.condition) == passed_values.end())
            {
                passed_values.insert(next_node.condition);
                parent_map[next_node.condition] = node.condition;
                int t = search(next_node, bound);
                if (t == -1)
                {
                    return -1;
                }
                if (t < min_threshold)
                {
                    min_threshold = t;
                }
                passed_values.erase(next_node.condition);
            }
        }
        return min_threshold;
    };

    int bound = manhattan_distance4x4(start_node.condition);
    while (true)
    {
        int t = search(start_node, bound);
        if (t == -1)
        {
            std::string current_condition = target_condition;
            while (current_condition != start_condition)
            {
                result.push_front(current_condition);
                current_condition = parent_map[current_condition];
            }
            std::cout << result.size() << std::endl;
            return result;
        }
        if (t == INT_MAX)
        {
            break; // Решение не найдено
        }
        bound = t;
    }
    return result;
}

int main()
{
    std::map<int, std::deque<int>> moves{
        {0, {1, 4}},
        {1, {0, 2, 5}},
        {2, {1, 3, 6}},
        {3, {2, 7}},
        {4, {0, 5, 8}},
        {5, {1, 4, 6, 9}},
        {6, {2, 5, 7, 10}},
        {7, {3, 6, 11}},
        {8, {4, 9, 12}},
        {9, {5, 8, 10, 13}},
        {10, {6, 9, 11, 14}},
        {11, {7, 10, 15}},
        {12, {8, 13}},
        {13, {9, 12, 14}},
        {14, {10, 13, 15}},
        {15, {11, 14}}};

    std::map<int, std::string> conditions{
        {-2, "123456789AFB0EDC"},
        {-1, "F2345678A0BE91CD"},
        {0, "123456789ABCDEF0"},
        {5, "1234067859ACDEBF"},
        {8, "5134207896ACDEBF"},
        {10, "16245A3709C8DEBF"},
        {13, "1723068459ACDEBF"},
        {19, "12345678A0BE9FCD"},
        {27, "51247308A6BE9FCD"},
        {33, "F2345678A0BE91DC"},
        {35, "75123804A6BE9FCD"},
        {40, "12A34BC57689DEF0"},
        {42, "A10598B2764C3DEF"},
        {48, "04582E1DF79BCA36"},
    };

    auto condition = conditions[42]; // generate_random_puzzle(); //"1A095B82F64C3DE7"; //

    auto node = create_start_node(condition);
    int draw_time_sleep = 200;
    bool erace = true;

    if (isSolvable4x4(node.condition))
    {

        std::cout << "Possible to Solve\n"
                  << "manhattan_distance4x4: " << manhattan_distance4x4(node.condition) << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        /////////////////////

        // std::cout << "IDA*" << std::endl;
        // auto res = solve_game_IDA_star(node, moves);

        std::cout << "A*" << std::endl;
        auto res = solve_game_A_star(node, moves);

        /////////////////
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Executed time: " << duration.count() << " milliseconds" << std::endl;

        std::stringstream ss;
        ss << "Length: " << res.size() << " Time: " << duration.count() << " milliseconds" << "\n";

        std::string info = ss.str();
        Sleep(2000);
        for (int i = 0; i < res.size(); ++i)
        {
            draw_field(res[i], erace, info);
            Sleep(draw_time_sleep);
        }
    }
    else
    {
        std::cout << "Impossible to Solve" << std::endl;
    }
    std::cout << condition << std::endl;
}