#include <iostream>
#include <vector>
#include <cstdint>
#include <array>
#include <cstdlib>
#include <windows.h>

void draw(const std::uint8_t values[]);

std::array<uint8_t, 16> generateRandomArray()
{
    std::array<uint8_t, 16> values;
    for (size_t i = 0; i < values.size(); ++i)
    {
        values[i] = rand() % 256; // Генерируем случайное значение от 0 до 255
    }
    return values;
}

void randomDraw(int n, int time)
{
    for (int i = 0; i <= n; ++i)
    {

        draw(generateRandomArray().data());
        Sleep(time);

        system("cls");
    }
}

bool isBitSet(int element, int bit_index)
{
    return (element & (1 << bit_index)) != 0;
}

void draw(const std::uint8_t values[])
{
    const std::string u1 = "        ";
    const std::string u2 = "  /\\/\\  ";
    const std::string u3 = "  \\__/  ";
    const std::string u4 = "________";

    const std::string l1 = "   __   ";
    const std::string l2 = "  /  \\  ";
    const std::string l3 = " /\\__/\\ ";
    const std::string l4 = "________";

    const std::string e1 = "        ";
    const std::string e2 = "        ";
    const std::string e3 = "        ";
    const std::string e4 = "________";

    const std::string sep = "|";
    const std::string line = "________";
    const std::string empty_line = "        ";

    std::vector<std::string> lines_upper = {u1, u2, u3, u4};
    std::vector<std::string> lines_lower = {l1, l2, l3, l4};
    std::vector<std::string> lines_empty = {e1, e2, e3, e4};

    std::cout << "\n\n\n";
    for (int col = 0; col < 8; ++col)
    {
        std::cout << " " << line;
    }
    std::cout << "\n";

    for (int row = 0; row < 8; ++row)
    {
        for (int ell = 0; ell <= 3; ++ell)
        {
            for (int col = 0; col < 8; ++col)
            {
                if (isBitSet(values[row], col))
                {
                    std::cout << sep << lines_upper[ell];
                }
                else if (isBitSet(values[row + 8], col))
                {
                    std::cout << sep << lines_lower[ell];
                }
                else
                {
                    std::cout << sep << lines_empty[ell];
                }
            }
            std::cout << sep << "\n";
        }
    }
    std::cout << "\n\n\n";
}

struct Node
{
    std::uint8_t position[16];
    int cost;
    int heuristic;
    Node *parent;

    // (f-cost = g-cost + h-cost)
    int totalCost() const
    {
        return cost + heuristic;
    }

    bool operator>(const Node &other) const
    {
        return totalCost() > other.totalCost();
    }
};

// Очень плохая формула - к тому же неправильная - у одинаковых состояний показывает 15
int manhattanDistance(const std::uint8_t (&current)[16], const std::uint8_t (&target)[16])
{
    int distance = 0;
    ////////////////
    return distance;
}

int main()
{
    std::uint8_t start_position[16] = {0b00000111, 0b00000111, 0b00000111, 0b00000000,
                                       0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                       0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                       0b00000000, 0b11100000, 0b11100000, 0b11100000};

    std::uint8_t target_position[16] = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                        0b00000000, 0b11100000, 0b11100000, 0b11100000,
                                        0b00000111, 0b00000111, 0b00000111, 0b00000000,
                                        0b00000000, 0b00000000, 0b00000000, 0b00000000};

    // std::uint8_t start_position[16] = {0b00000001, 0b00000000, 0b00000000, 0b00000000,
    //                                    0b00000000, 0b00000000, 0b00000000, 0b00000000,
    //                                    0b00000000, 0b00000000, 0b00000000, 0b00000000,
    //                                    0b00000000, 0b00000000, 0b00000000, 0b00000001};

    // std::uint8_t target_position[16] = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
    //                                     0b00000000, 0b00000000, 0b00000000, 0b10000000,
    //                                     0b00000000, 0b00000000, 0b10000000, 0b00000000,
    //                                     0b00000000, 0b00000000, 0b00000000, 0b00000000};

    draw(start_position);
    draw(target_position);
    return 0;
}