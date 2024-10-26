#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <windows.h>
#include <vector>
#include <stack>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <cmath>
#include <map>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

const int FIELD_SIZE = 15;

void fillCenterDistArray(float array[FIELD_SIZE][FIELD_SIZE], float k)
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            array[i][j] = (((7.0 - abs(8 - (i + 1))) / 7.0 + (7.0 - abs(8 - (j + 1))) / 7.0)) * k;
        }
    }
}

void printCharArray(char array[FIELD_SIZE][FIELD_SIZE])
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            std::cout << array[i][j] << ' '; // Выводим каждый символ с пробелом
        }
        std::cout << std::endl; // Переход на новую строку
    }
}

void resetField(char (&field)[FIELD_SIZE][FIELD_SIZE])
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            field[i][j] = '-';
        }
    }
}

tuple<int, int> comToIndexes(std::string com)
{
    char c = com[0];
    int j = c - 'a';

    string substring = com.substr(1);
    int i = stoi(substring);

    return make_tuple(i - 1, j);
}
std::string indexesToCom(int i, int j)
{
    char c = 'a' + j;
    return c + std::to_string(i + 1); // Возврат ошибки, если индекс не в диапазоне
}
char flip(char c)
{
    return (c == '0') ? '1' : '0';
}

float hForOneEll(const char field[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], int i, int j, char simbol, int target_len = 5)
{

    std::vector<std::pair<int, int>>
        vertical;
    std::vector<std::pair<int, int>> horizontal;
    std::vector<std::pair<int, int>> mainDiagonal;
    std::vector<std::pair<int, int>> secondaryDiagonal;

    for (int col = 0; col < FIELD_SIZE; ++col)
    {
        horizontal.emplace_back(i, col);
    }

    for (int row = 0; row < FIELD_SIZE; ++row)
    {
        vertical.emplace_back(row, j);
    }

    int diag_i = i, diag_j = j;
    while (diag_i > 0 && diag_j > 0)
    {
        diag_i--;
        diag_j--;
    }
    while (diag_i < FIELD_SIZE && diag_j < FIELD_SIZE)
    {
        mainDiagonal.emplace_back(diag_i, diag_j);
        diag_i++;
        diag_j++;
    }

    diag_i = i, diag_j = j;
    while (diag_i > 0 && diag_j < FIELD_SIZE - 1)
    {
        diag_i--;
        diag_j++;
    }

    while (diag_i < FIELD_SIZE && diag_j >= 0)
    {
        secondaryDiagonal.emplace_back(diag_i, diag_j);
        diag_i++;
        diag_j--;
    }

    std::array<std::vector<std::pair<int, int>>, 4> directions = {vertical, horizontal, mainDiagonal, secondaryDiagonal};

    float h_general = 0;
    float h_local = 0;
    int len = 0;
    int collected_detail_dist = 0;

    // добавить штуку которая добавляет определённые коэф-ты для расстояний
    for (size_t idx = 0; idx < directions.size(); ++idx)
    {
        len = 0;
        h_local = 0;
        // std::cout << "Direction " << idx + 1 << ":\n";
        for (const auto &coord : directions[idx])
        {
            // std::cout << "(" << coord.first << ", " << coord.second << ") ";
            int f = coord.first;
            int s = coord.second;
            if (field[f][s] == '-')
            {
                len += 1;
                collected_detail_dist = 0;
            }
            else if (field[f][s] == simbol)
            {
                collected_detail_dist += 1;
                h_local += (1 * collected_detail_dist * collected_detail_dist);
                h_local += centerDist[f][s];
                len += 1;
                if (collected_detail_dist >= target_len)
                {
                    return std::numeric_limits<float>::max();
                }
            }
            else
            {
                collected_detail_dist = 0;
                if (len >= target_len)
                {
                    h_general += h_local;
                }

                h_local = 0;
                len = 0;
            }
            if (len >= target_len)
            {
                h_general += h_local;
            }
        }
    }
    return h_general;
}

float processLine(const std::vector<std::pair<int, int>> &line, const char field[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
{
    float h_general = 0;
    float h_local = 0;
    int len = 0;
    int collected_detail_dist = 0;

    for (const auto &coord : line)
    {
        int f = coord.first;
        int s = coord.second;
        if (field[f][s] == '-')
        {
            len += 1;
            collected_detail_dist = 0;
        }
        else if (field[f][s] == simbol)
        {
            collected_detail_dist += 1;
            h_local += (1 * collected_detail_dist * collected_detail_dist * collected_detail_dist);
            h_local += centerDist[f][s];
            len += 1;
            if (collected_detail_dist >= target_len)
            {
                return 420000; // std::numeric_limits<float>::max();
            }
        }
        else
        {
            collected_detail_dist = 0;
            if (len >= target_len)
            {
                h_general += h_local;
            }

            h_local = 0;
            len = 0;
        }
        if (len >= target_len)
        {
            h_general += h_local;
        }
    }
    return h_general;
    // std::cout << std::endl;
}

float totalHeuristicOnePlayer(const char field[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
{
    float h = 0;
    // Обработка горизонтальных линий
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        std::vector<std::pair<int, int>> horizontal;
        for (int col = 0; col < FIELD_SIZE; ++col)
        {
            horizontal.emplace_back(i, col);
        }
        h += processLine(horizontal, field, centerDist, simbol); // Вызов заглушки
    }

    // Обработка вертикальных линий
    for (int j = 0; j < FIELD_SIZE; ++j)
    {
        std::vector<std::pair<int, int>> vertical;
        for (int row = 0; row < FIELD_SIZE; ++row)
        {
            vertical.emplace_back(row, j);
        }
        h += processLine(vertical, field, centerDist, simbol); // Вызов заглушки
    }

    // Обработка главных диагоналей
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        std::vector<std::pair<int, int>> mainDiagonal;
        int diag_i = i, diag_j = 0;
        while (diag_i < FIELD_SIZE && diag_j < FIELD_SIZE)
        {
            mainDiagonal.emplace_back(diag_i, diag_j);
            diag_i++;
            diag_j++;
        }
        h += processLine(mainDiagonal, field, centerDist, simbol); // Вызов заглушки
    }
    for (int j = 1; j < FIELD_SIZE; ++j)
    {
        std::vector<std::pair<int, int>> mainDiagonal;
        int diag_i = 0, diag_j = j;
        while (diag_i < FIELD_SIZE && diag_j < FIELD_SIZE)
        {
            mainDiagonal.emplace_back(diag_i, diag_j);
            diag_i++;
            diag_j++;
        }
        h += processLine(mainDiagonal, field, centerDist, simbol);
        ; // Вызов заглушки
    }

    // Обработка побочных диагоналей
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        std::vector<std::pair<int, int>> secondaryDiagonal;
        int diag_i = i, diag_j = FIELD_SIZE - 1;
        while (diag_i < FIELD_SIZE && diag_j >= 0)
        {
            secondaryDiagonal.emplace_back(diag_i, diag_j);
            diag_i++;
            diag_j--;
        }
        h += processLine(secondaryDiagonal, field, centerDist, simbol); // Вызов заглушки
    }
    for (int j = FIELD_SIZE - 2; j >= 0; --j)
    {
        std::vector<std::pair<int, int>> secondaryDiagonal;
        int diag_i = 0, diag_j = j;
        while (diag_i < FIELD_SIZE && diag_j >= 0)
        {
            secondaryDiagonal.emplace_back(diag_i, diag_j);
            diag_i++;
            diag_j--;
        }
        h += processLine(secondaryDiagonal, field, centerDist, simbol); // Вызов заглушки
    }
    return h;
}
float totalHeuristic(const char field[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
{
    return totalHeuristicOnePlayer(field, centerDist, simbol) - totalHeuristicOnePlayer(field, centerDist, flip(simbol)) * 3;
}
struct Node
{
    char field[FIELD_SIZE][FIELD_SIZE];
    int parent_id;
    float h;
    int depth;
    int i;
    int j;

    Node(char (&field)[FIELD_SIZE][FIELD_SIZE], int parent_id, int h, int depth, int i, int j) : parent_id(parent_id), h(h), depth(depth), i(i), j(j)
    {
        std::copy(&field[0][0], &field[0][0] + FIELD_SIZE * FIELD_SIZE, &this->field[0][0]);
    }
};
struct CompareNode
{
    bool operator()(const Node &n1, const Node &n2)
    {
        return n1.h < n2.h; // Большие значения h будут иметь больший приоритет
    }
};
std::string nextStepBestMinimax(char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol)
{
    int max_depth = 5;
    int wight_search = 1;
    auto start = simbol;
    std::deque<Node> nodes;
    nodes.emplace_back(field, -1, 0, 0, -1, 1);
    // cout << nodes.size() << endl;
    // cout << nodes.front().field[0][0] << endl;
    // printCharArray(nodes.front().field);

    int index_current = 0;

    while (true)
    {
        if (index_current > nodes.size() - 1)
        {
            break;
        }
        auto n = nodes[index_current];
        if (n.depth >= max_depth)
        {
            index_current++;
            continue;
        }

        if (n.depth % 2 == 0)
        {

            simbol = start; //'0'; //
        }
        else
        {
            simbol = flip(start); // ;
        }

        // cout << simbol << "\n";

        std::priority_queue<Node, std::vector<Node>, CompareNode> new_nodes_pq;
        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (n.field[i][j] == '-')
                {
                    n.field[i][j] = simbol;
                    auto h = totalHeuristic(n.field, centerDist, simbol); // push(Node(1, 5))
                    new_nodes_pq.push(Node(n.field, index_current, h, n.depth + 1, i, j));
                    n.field[i][j] = '-';
                }
            }
        }
        int count = 0;
        while (!new_nodes_pq.empty())
        {
            if (count < wight_search)
            {
                nodes.push_back(new_nodes_pq.top());
            }
            new_nodes_pq.pop(); // Удаляем из priority_queue
            ++count;
        }
        index_current++;
    }

    for (std::deque<Node>::iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        printCharArray(it->field);
        cout << "\n\n";
    }
    cout << nodes.size() << "\n";
    return "EEEEE";
}

std::string nextStepBestH(char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol)
{

    int best_i = 0;
    int best_j = 0;
    float best_h = -1000000;

    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            if (field[i][j] == '-')
            {
                field[i][j] = simbol;
                float h = totalHeuristic(field, centerDist, simbol); // hForOneEll(field, centerDist, i, j, simbol); //
                field[i][j] = '-';
                if (h > best_h)
                {
                    best_h = h;
                    best_i = i;
                    best_j = j;
                }
            }
        }
    }
    return indexesToCom(best_i, best_j);
}

void printFloatArray(float array[FIELD_SIZE][FIELD_SIZE])
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            cout << fixed << setprecision(2) << array[i][j] << ' ';
        }
        cout << endl;
    }
}

void test()
{
    std::string s;

    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];
    resetField(field);

    fillCenterDistArray(centerDist, 0.5);

    char simbol = '0';
    nextStepBestMinimax(field, centerDist, simbol);
    exit;
}

int main(int argc, char *argv[])
{
    test();
    std::string s;

    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];

    std::stack<std::string> steps;

    fillCenterDistArray(centerDist, 0.5);

    char simbol;

    resetField(field);

    if (argv[1][0] == '0')
    {
        simbol = '0';
        std::string ns = nextStepBestH(field, centerDist, simbol);

        steps.push(ns);
        std::cout << ns << std::endl;
        tuple<int, int> coordinates = comToIndexes(ns);
        int i = get<0>(coordinates);
        int j = get<1>(coordinates);
        field[i][j] = simbol;
    }
    else
    {
        simbol = '1';
    }

    while (true)
    {
        cin >> s;

        if (s == "u1")
        {
            if (!steps.empty())
            {
                std::string last_move = steps.top();
                steps.pop();
                tuple<int, int> coordinates = comToIndexes(last_move);
                int i = get<0>(coordinates);
                int j = get<1>(coordinates);
                field[i][j] = '-';
                std::cout << "Last move undone: " << last_move << std::endl;
            }
            else
            {
                std::cout << "No moves to undo!" << std::endl;
            }

            continue;
        }
        tuple<int, int> coordinates = comToIndexes(s);
        int i = get<0>(coordinates);
        int j = get<1>(coordinates);

        if (field[i][j] != '-')
        {
            std::cout << "This value has already been used. Try again: " << s << std::endl;
            continue;
        }

        field[i][j] = flip(simbol);

        steps.push(s);

        std::string ns = nextStepBestH(field, centerDist, simbol);

        tuple<int, int> coordinates_resp = comToIndexes(ns);
        i = get<0>(coordinates_resp);
        j = get<1>(coordinates_resp);
        field[i][j] = simbol;

        steps.push(ns);
        std::cout << ns << std::endl;

        if (steps.size() == 225)
        {
            std::cout << "All possible values have been used." << std::endl;
            break;
        }
    }

    return 0;
}