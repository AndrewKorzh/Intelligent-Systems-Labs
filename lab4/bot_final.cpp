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

std::string nextStepBestH(char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], char simbol)
{

    int best_i = 0;
    int best_j = 0;
    float best_h = 0;

    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            if (field[i][j] == '-')
            {
                field[i][j] = simbol;
                float h = hForOneEll(field, centerDist, i, j, simbol);
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

int main(int argc, char *argv[])
{
    std::string s;

    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];

    std::stack<std::string> steps;

    fillCenterDistArray(centerDist, 100);

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
        Sleep(1);
        std::cout << ns << std::endl;

        if (steps.size() == 225)
        {
            std::cout << "All possible values have been used." << std::endl;
            break;
        }
    }

    return 0;
}