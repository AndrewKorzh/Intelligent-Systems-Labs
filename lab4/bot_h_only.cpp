#include <iostream>
#include <iomanip>
#include <string>
#include <tuple>
#include <windows.h>
#include <vector>
#include <stack>

using namespace std;

const int FIELD_SIZE = 15;

void fillCenterDistArray(float array[FIELD_SIZE][FIELD_SIZE], float k)
{
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            array[i][j] = ((7.0 - abs(8 - (i + 1))) / 7.0 + (7.0 - abs(8 - (j + 1))) / 7.0) * k;
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

void printField(const char (&field)[FIELD_SIZE][FIELD_SIZE])
{

    cout << "\n\n\n";
    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            cout << field[i][j] << ' ';
        }
        cout << endl;
    }
    cout << "\n\n\n";
}
tuple<int, int> comToIndexes(std::string com)
{
    char c = com[0];
    int j = c - 'a';

    string substring = com.substr(1);
    int i = stoi(substring);

    return make_tuple(i - 1, j);
}

float hForOneEll(const char field[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE], int i, int j, char simbol, int target_len = 5)
{
    float h_general = 0;
    float h_local = 0;
    int len = 0;

    for (int col = 0; col < FIELD_SIZE; ++col)
    {
        if (field[i][col] == '-')
        {
            len += 1;
        }
        else if (field[i][col] == simbol)
        {
            h_local += 1;
            h_local += centerDist[i][col];
            len += 1;
        }
        else
        {
            if (len >= target_len)
            {
                h_general += h_local;
            }
            h_local = 0;
            len = 0;
        }
    }

    h_general += h_local;
    h_local = 0;
    len = 0;

    for (int row = 0; row < FIELD_SIZE; ++row)
    {
        if (field[row][j] == '-')
        {
            len += 1;
        }
        else if (field[row][j] == simbol)
        {
            h_local += 1;
            h_local += centerDist[row][j];
            len += 1;
        }
        else
        {
            if (len >= target_len)
            {
                h_general += h_local;
            }
            h_local = 0;
            len = 0;
        }
    }

    h_general += h_local;
    h_local = 0;
    len = 0;

    int diag_i = i, diag_j = j;
    while (diag_i > 0 && diag_j > 0)
    {
        diag_i--;
        diag_j--;
    }
    while (diag_i < FIELD_SIZE && diag_j < FIELD_SIZE)
    {
        if (field[diag_i][diag_j] == '-')
        {
            len += 1;
        }
        else if (field[diag_i][diag_j] == simbol)
        {
            h_local += 1;
            h_local += centerDist[diag_i][diag_j];
            len += 1;
        }
        else
        {
            if (len >= target_len)
            {
                h_general += h_local;
            }
            h_local = 0;
            len = 0;
        }
        diag_i++;
        diag_j++;
    }

    h_general += h_local;
    h_local = 0;
    len = 0;

    diag_i = i, diag_j = j;
    while (diag_i > 0 && diag_j < FIELD_SIZE - 1)
    {
        diag_i--;
        diag_j++;
    }

    while (diag_i < FIELD_SIZE && diag_j >= 0)
    {
        if (field[diag_i][diag_j] == '-')
        {
            len += 1;
        }
        else if (field[diag_i][diag_j] == simbol)
        {
            h_local += 1;
            h_local += centerDist[diag_i][diag_j];
            len += 1;
        }
        else
        {
            if (len >= target_len)
            {
                h_general += h_local;
            }
            h_local = 0;
            len = 0;
        }
        diag_i++;
        diag_j--;
    }

    h_general += h_local;
    return h_general;
}
std::string indexesToCom(int i, int j)
{
    char c = 'a' + j;
    return c + std::to_string(i + 1); // Возврат ошибки, если индекс не в диапазоне
}
std::string nextStepBestH(char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE])
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
                field[i][j] = '0';
                float h = hForOneEll(field, centerDist, i, j, '0');
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
            cout << fixed << setprecision(2) << array[i][j] << ' '; // Точно 2 знака после запятой
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    std::string s;

    // testFunctions();
    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];

    std::stack<std::string> steps;

    fillCenterDistArray(centerDist, 1);

    // printFloatArray(centerDist);

    resetField(field);

    // Естественно назначать значения ячеек в зависимоти от того начинало ли

    if (argv[1][0] == '0')
    {
        std::string ns = nextStepBestH(field, centerDist);

        steps.push(ns);
        std::cout << ns << std::endl;
        tuple<int, int> coordinates = comToIndexes(ns);
        int i = get<0>(coordinates);
        int j = get<1>(coordinates);
        field[i][j] = '0';
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

        field[i][j] = '1';

        steps.push(s);

        std::string ns = nextStepBestH(field, centerDist);

        tuple<int, int> coordinates_resp = comToIndexes(ns);
        i = get<0>(coordinates_resp);
        j = get<1>(coordinates_resp);
        field[i][j] = '0';

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