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
using std::get;

const int FIELD_SIZE = 15;
const int MAX_H = 4200000;

void printCharArray(char array[FIELD_SIZE][FIELD_SIZE])
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            std::cout << array[i][j] << ' ';
        }
        std::cout << std::endl;
    }
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

// Можно доделать -
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
            // Близость к центру
            h_local += (((7.0 - abs(8 - (f + 1))) / 7.0 + (7.0 - abs(8 - (s + 1))) / 7.0)) * k;
            len += 1;
            if (collected_detail_dist >= target_len)
            {
                return MAX_H;
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
bool isGameOver(const char field[FIELD_SIZE][FIELD_SIZE], int target_len = 5)
{
    // Символы для проверки
    char symbols[] = {'0', '1'};

    // Проходим по каждому символу
    for (char simbol : symbols)
    {
        // Проверка горизонтальных линий
        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            int count = 0;
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (field[i][j] == simbol)
                {
                    count++;
                    if (count == target_len)
                        return true;
                }
                else
                {
                    count = 0;
                }
            }
        }

        // Проверка вертикальных линий
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            int count = 0;
            for (int i = 0; i < FIELD_SIZE; ++i)
            {
                if (field[i][j] == simbol)
                {
                    count++;
                    if (count == target_len)
                        return true;
                }
                else
                {
                    count = 0;
                }
            }
        }

        // Проверка главных диагоналей (↘)
        for (int start = 0; start <= FIELD_SIZE - target_len; ++start)
        {
            int count_main_diag = 0;
            int count_secondary_diag = 0;
            for (int offset = 0; start + offset < FIELD_SIZE; ++offset)
            {
                if (field[start + offset][offset] == simbol)
                {
                    count_main_diag++;
                    if (count_main_diag == target_len)
                        return true;
                }
                else
                {
                    count_main_diag = 0;
                }

                if (field[offset][start + offset] == simbol)
                {
                    count_secondary_diag++;
                    if (count_secondary_diag == target_len)
                        return true;
                }
                else
                {
                    count_secondary_diag = 0;
                }
            }
        }

        // Проверка побочных диагоналей (↙)
        for (int start = target_len - 1; start < FIELD_SIZE; ++start)
        {
            int count_main_diag = 0;
            int count_secondary_diag = 0;
            for (int offset = 0; start - offset >= 0 && offset < FIELD_SIZE; ++offset)
            {
                if (field[start - offset][offset] == simbol)
                {
                    count_main_diag++;
                    if (count_main_diag == target_len)
                        return true;
                }
                else
                {
                    count_main_diag = 0;
                }

                if (field[FIELD_SIZE - 1 - offset][start - offset] == simbol)
                {
                    count_secondary_diag++;
                    if (count_secondary_diag == target_len)
                        return true;
                }
                else
                {
                    count_secondary_diag = 0;
                }
            }
        }
    }

    return false;
}

std::tuple<int, int, float> minimax(char (&field)[FIELD_SIZE][FIELD_SIZE], char simbol, char start_simbol, int depth, float a, float b, int row, int col, bool isMaxim)
{
    if (depth <= 0 || isGameOver(field))
    {
        float total_h = totalHeuristic(field, start_simbol);
        return std::make_tuple(row, col, total_h);
    }

    if (isMaxim)
    {

        float best_score = -std::numeric_limits<float>::max();
        int best_row = -1;
        int best_col = -1;
        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (field[i][j] == '-')
                {
                    field[i][j] = simbol;
                    float score = std::get<2>(minimax(field, flip(simbol), start_simbol, depth - 1, a, b, i, j, false)); // hForOneEll(field, i, j, simbol); //
                    field[i][j] = '-';
                    if (score > best_score)
                    {
                        best_score = score;
                        best_row = i;
                        best_col = j;
                    }
                    a = std::max(a, best_score);
                    if (b <= a)
                    {
                        break;
                    }
                }
            }
        }

        return std::make_tuple(best_row, best_col, best_score);
    }
    else
    {
        float best_score = std::numeric_limits<float>::max();
        int best_row = -1;
        int best_col = -1;

        for (int i = 0; i < FIELD_SIZE; ++i)
        {
            for (int j = 0; j < FIELD_SIZE; ++j)
            {
                if (field[i][j] == '-')
                {
                    field[i][j] = simbol;
                    float score = std::get<2>(minimax(field, flip(simbol), start_simbol, depth - 1, a, b, i, j, true)); // hForOneEll(field, i, j, simbol); //
                    field[i][j] = '-';
                    if (score < best_score)
                    {
                        best_score = score;
                        best_row = i;
                        best_col = j;
                    }
                    b = std::min(b, best_score);
                    if (b <= a)
                    {
                        break;
                    }
                }
            }
        }

        return std::make_tuple(best_row, best_col, best_score);
    }
}

std::string nextStepMinimax(char (&field)[FIELD_SIZE][FIELD_SIZE], char simbol)
{
    int best_i = 0;
    int best_j = 0;
    int depth = 2;
    float a = -std::numeric_limits<float>::max();
    float b = std::numeric_limits<float>::max();
    char start_simbol = simbol;

    auto res = minimax(field, simbol, start_simbol, depth, a, b, -1, -1, true);
    best_i = std::get<0>(res);
    best_j = std::get<1>(res);

    return indexesToCom(best_i, best_j);
}

int main(int argc, char *argv[])
{
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
        std::string ns = nextStepMinimax(field, simbol); // nextStepBestH(field, simbol);

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

        std::string ns = nextStepMinimax(field, simbol); // nextStepBestH(field, simbol);

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