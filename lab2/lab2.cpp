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

int rand(void);
struct Node
{
    std::string condition;
    int last_position;
    int empty_index;
    int g;

    Node(std::string cond, int lp, int ei, int steps) : condition(cond), last_position(lp), empty_index(ei), g(steps) {}
};

int manhattan_distance4x4(const std::string &state)
{
    int grid_size = 4;
    int distance = 0;

    for (int i = 0; i < state.size(); ++i)
    {
        char tile = state[i];
        if (tile != '0') // Пропускаем пустую плитку
        {
            int value = (tile <= '9') ? tile - '1' : tile - 'A' + 9; // Преобразование плитки в целое значение
            int current_x = i % grid_size;
            int current_y = i / grid_size;
            int target_x = value % grid_size;
            int target_y = value / grid_size;

            distance += std::abs(current_x - target_x) + std::abs(current_y - target_y);
        }
    }
    return distance;
}

struct Compare
{
    bool operator()(const Node &n1, const Node &n2)
    {
        return n1.g + manhattan_distance4x4(n1.condition) > n2.g + manhattan_distance4x4(n2.condition); // тут элемент с меньшим приоритетом будет наверху
    }
};

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

void draw_sequence(const std::list<std::string> &seq, int sleep_time, bool er)
{
    for (std::string value : seq)
    {
        draw_field(value, er);
        Sleep(sleep_time);
    }
}
void draw_nodes_sequence(const std::deque<Node> &nodes, int sleep_time = 300, bool er = true)
{
    for (int i = 0; i < nodes.size(); ++i)
    {
        draw_field(nodes[i].condition, er);
        Sleep(sleep_time);
    }
}

std::string swap_positions(const std::string &s, int i1, int i2)
{
    std::string res = s;
    res[i1] = s[i2];
    res[i2] = s[i1];
    return res;
}

std::deque<Node> next_nodes(const Node &node, std::map<int, std::deque<int>> &moves)
{
    // Condition, last position, empty index
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

Node create_start_node(std::string c)
{
    Node n(c, -1, c.find('0'), 0);
    return n;
}

std::deque<std::string> solve_game_A_star(Node start_node, std::map<int, std::deque<int>> &moves)
{
    std::string start_condition = start_node.condition;
    std::string target_condition = "123456789ABCDEF0";
    std::map<std::string, std::string> parent_map;

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

    // auto condition = "5134207896ACDEBF";
    // auto condition = "0FEDCBA987654321";
    // auto condition = "123456789ABCDE0F";
    // auto condition = "123456789ABCDEF0";

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
        {45, "75AB2C416D389F0E"},
        {48, "04582E1DF79BCA36"},
        {52, "FE169B4C0A73D852"},
        {55, "D79F2E8A45106C3B"},
        {58, "DBE87A2C91F65034"},
        {61, "BAC0F478E19623D5"},

    };

    auto condition = conditions[35];
    auto node = create_start_node(condition);
    int draw_time_sleep = 300;
    bool erace = true;

    if (isSolvable4x4(node.condition))
    {

        std::cout << "Can Solve" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        auto res = solve_game_A_star(node, moves);
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
        std::cout << "Cant Solve" << std::endl;
    }
}
//
//
//
//
//
//
//
///////////////////        not used    ///////////////////////
// std::deque<Node> random_node_seq(Node node, std::map<int, std::deque<int>> &moves, int length = 10)
// {
//     std::deque<Node> nodes;
//     nodes.emplace_back(node);

//     int g_start = node.g;

//     for (int i = 1; i < length; ++i)
//     {
//         auto next_steps_deque = next_steps(nodes.back(), moves);
//         auto next_random_node = next_steps_deque[rand() % next_steps_deque.size()];
//         nodes.emplace_back(std::get<0>(next_random_node), std::get<1>(next_random_node), std::get<2>(next_random_node), g_start + i);
//     }

//     return nodes;
// }

// std::deque<std::tuple<std::string, int, int>> next_steps(const Node &node, std::map<int, std::deque<int>> &moves)
// {
//     // Condition, last position, empty index
//     std::deque<std::tuple<std::string, int, int>> ns;
//     for (int i = 0; i < moves[node.empty_index].size(); ++i)
//     {
//         if (moves[node.empty_index][i] != node.last_position)
//         {
//             ns.emplace_back(swap_positions(node.condition, moves[node.empty_index][i], node.empty_index), node.empty_index, moves[node.empty_index][i]);
//         }
//     }
//     return ns;
// }
///////////////////////////////////////////////////////////////////////

// draw_nodes_sequence(random_node_seq(node, moves, 20), 500, true);
//  std::deque<Node> nodes;

// nodes.emplace_back("123456789ABCDEF0", 0, 14, 15);

// int index = 0;
// while (true)
// {
//     index += 1;
//     auto next_steps_val = next_steps(nodes.back(), moves);

//     auto next_step = next_steps_val[rand() % next_steps_val.size()];

//     nodes.emplace_back(std::get<0>(next_step), 0, std::get<1>(next_step), std::get<2>(next_step)); // неправильно из кортнжа
//     if (index > 100)
//     {
//         break;
//     }
// }

// for (int i = 0; i < nodes.size(); ++i)
// {
//     draw_field(nodes[i].condition, true);
//     Sleep(800);
// }
// auto ns = next_steps(n, moves);

// for

// draw_field(n.condition);

// draw_field(ns[0]);

// for (int i = 0; i < ns.size(); ++i)
// {
//     std::cout << ns[i] << std::endl;
// }

// Node n("123456789ABCDEF0", -1, 14, 15);

// next_steps(n, moves);

// std::string s = "123123321";

// s[0] = '3';

// std::cout << s << std::endl;
// std::list<std::string> seq4 = {"123456789ABCDE0F", "123456789ABCDEF0", "123456789AB0DEFC"};
// draw_sequence(seq4, 600, true);

// std::list<std::string> seq3 = {"123456780", "123456708", "123456078", "123056478", "123506478"};
// draw_sequence(seq3, 1, true);