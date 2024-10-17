#include <iostream>
using namespace std;

const int FIELD_SIZE = 15;

// Капец как код дуюлирую
// Потом переписать
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
std::string nextStepClosestToCenter(const char (&field)[FIELD_SIZE][FIELD_SIZE], const float (&centerDist)[FIELD_SIZE][FIELD_SIZE])
{

    int best_i = 0;
    int best_j = 0;
    float best_h = 0;

    for (int i = 0; i < FIELD_SIZE; ++i)
    {
        for (int j = 0; j < FIELD_SIZE; ++j)
        {
            float h = hForOneEll(field, centerDist, i, j, '0');

            if (field[i][j] == '-')
            {
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

int main()
{

    float centerDist[FIELD_SIZE][FIELD_SIZE];

    fillCenterDistArray(centerDist, 1);

    char field[FIELD_SIZE][FIELD_SIZE] = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-'}};

    int i = 3, j = 4;

    hForOneEll(field, centerDist, i, j, 'a');

    return 0;
}
