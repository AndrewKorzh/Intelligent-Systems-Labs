#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <windows.h>
#include <vector>
#include <stack>
#include <limits>
#include <deque>
#include <queue>
#include <utility>
#include <cmath>
#include <map>

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

float processLine(const std::vector<std::pair<int, int>> &line, const char field[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
{
    int k = 0.5;
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
            // array[i][j] = (((7.0 - abs(8 - (i + 1))) / 7.0 + (7.0 - abs(8 - (j + 1))) / 7.0)) * k;
            // h_local += centerDist[f][s];
            h_local += (((7.0 - abs(8 - (f + 1))) / 7.0 + (7.0 - abs(8 - (s + 1))) / 7.0)) * k;
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

float totalHeuristicOnePlayer(const char field[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
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
        h += processLine(horizontal, field, simbol); // Вызов заглушки
    }

    // Обработка вертикальных линий
    for (int j = 0; j < FIELD_SIZE; ++j)
    {
        std::vector<std::pair<int, int>> vertical;
        for (int row = 0; row < FIELD_SIZE; ++row)
        {
            vertical.emplace_back(row, j);
        }
        h += processLine(vertical, field, simbol); // Вызов заглушки
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
        h += processLine(mainDiagonal, field, simbol); // Вызов заглушки
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
        h += processLine(mainDiagonal, field, simbol);
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
        h += processLine(secondaryDiagonal, field, simbol); // Вызов заглушки
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
        h += processLine(secondaryDiagonal, field, simbol); // Вызов заглушки
    }
    return h;
}
float totalHeuristic(const char field[FIELD_SIZE][FIELD_SIZE], char simbol, int target_len = 5)
{
    return totalHeuristicOnePlayer(field, simbol) - totalHeuristicOnePlayer(field, flip(simbol)) * 3;
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

std::string nextStepBestH(char (&field)[FIELD_SIZE][FIELD_SIZE], char simbol)
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
                float h = totalHeuristic(field, simbol); // hForOneEll(field, i, j, simbol); //
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

std::pair<int, int> minimax(char field[FIELD_SIZE][FIELD_SIZE], char simbol, int depth, bool isMaximizing)
{
    // Проверка на выигрыш
    float currentH = totalHeuristic(field, simbol);
    if (currentH >= std::numeric_limits<float>::max())
        return {-1, 1000 - depth}; // Максимальная оценка для максимизирующего
    if (currentH <= -std::numeric_limits<float>::max())
        return {-1, depth - 1000}; // Максимальная оценка для минимизирующего

    if (depth == 0)
        return {-1, 0}; // Достигнута максимальная глубина, возврат 0

    if (isMaximizing)
    {
        float bestScore = -std::numeric_limits<float>::max();
        std::pair<int, int> bestMove = {-1, -1};

        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (field[i][j] == '-')
                {
                    field[i][j] = simbol;                                          // Сделать ход
                    float score = minimax(field, simbol, depth - 1, false).second; // Вызов рекурсии
                    field[i][j] = '-';                                             // Отменить ход

                    if (score > bestScore)
                    {
                        bestScore = score;
                        bestMove = {i, j}; // Сохранить лучший ход
                    }
                }
            }
        }
        return {bestMove.first, bestScore}; // Возвращаем лучший ход и его оценку
    }
    else
    {
        float bestScore = std::numeric_limits<float>::max();
        std::pair<int, int> bestMove = {-1, -1};
        char opponent = flip(simbol);

        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (field[i][j] == '-')
                {
                    field[i][j] = opponent;                                       // Сделать ход противника
                    float score = minimax(field, simbol, depth - 1, true).second; // Вызов рекурсии
                    field[i][j] = '-';                                            // Отменить ход

                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestMove = {i, j}; // Сохранить лучший ход
                    }
                }
            }
        }
        return {bestMove.first, bestScore}; // Возвращаем лучший ход и его оценку
    }
}

std::string findBestMove(char field[FIELD_SIZE][FIELD_SIZE], char simbol)
{
    // Получаем лучший ход с помощью minimax
    std::pair<int, float> bestMove = minimax(field, simbol, 3, true);
    return indexesToCom(bestMove.first, bestMove.second); // Возврат лучшего хода
}

void test()
{
    std::string s;

    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];
    resetField(field);

    fillCenterDistArray(centerDist, 0.5);

    char simbol = '0';
    // nextStepBestMinimax(field,  simbol);
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
        std::string ns = nextStepBestH(field, simbol); // findBestMove(field, centerDist, simbol); //

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

        std::string ns = nextStepBestH(field, simbol); // findBestMove(field, centerDist, simbol);  //

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