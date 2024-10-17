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

std::string indexesToCom(int i, int j)
{
    char c = 'a' + j;
    return c + std::to_string(i + 1); // Возврат ошибки, если индекс не в диапазоне
}

std::string nextStepClosestToCenter(const char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE])
{
    // Тут некая функция эвристики - а вообще тут должно быть всё
    int best_i = 0;
    int best_j = 0;
    float best_h = 0;

    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {

            if (field[i][j] == '-')
            {
                if (centerDist[i][j] > best_h)
                {
                    best_h = centerDist[i][j];
                    best_i = i;
                    best_j = j;
                }
            }
        }
    }
    return indexesToCom(best_i, best_j);
}

int testFunctions()
{
    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];

    std::vector<std::string> steps;

    // Steps хранить

    fillCenterDistArray(centerDist, 1);

    // printFloatArray(centerDist);

    resetField(field);

    for (int index = 0; index < 10; index++)
    {
        printField(field);
        auto ns = nextStepClosestToCenter(field, centerDist);
        steps.emplace_back(ns);
        tuple<int, int> coordinates = comToIndexes(ns);
        int i = get<0>(coordinates);
        int j = get<1>(coordinates);
        field[i][j] = 'A';
        Sleep(100);
        system("cls");
    }
    printField(field);
    printFloatArray(centerDist);

    for (int index = 0; index < steps.size(); index++)
    {
        cout << steps[index] << " ";
    }
    cout << "\n";
    return 0;
}

int main(int argc, char *argv[])
{
    std::string s;

    // testFunctions();
    float centerDist[FIELD_SIZE][FIELD_SIZE];

    char field[FIELD_SIZE][FIELD_SIZE];

    std::stack<std::string> steps;

    fillCenterDistArray(centerDist, 1);

    resetField(field);

    if (argv[1][0] == '0')
    {
        std::string ns = nextStepClosestToCenter(field, centerDist);

        steps.push(ns);
        std::cout << ns << std::endl;
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

        field[i][j] = '0';

        steps.push(s);

        std::string ns = nextStepClosestToCenter(field, centerDist);

        tuple<int, int> coordinates_resp = comToIndexes(ns);
        i = get<0>(coordinates_resp);
        j = get<1>(coordinates_resp);
        field[i][j] = '1';

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