#include <iostream>
#include <vector>
#include <cstdint>
#include <array>
#include <cstdlib>
#include <windows.h>
#include <queue>
#include <cstring>
#include <utility>

void draw(const std::array<std::uint8_t, 16> values);

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

        draw(generateRandomArray());
        Sleep(time);

        system("cls");
    }
}

bool isBitSet(int element, int bit_index)
{
    return (element & (1 << bit_index)) != 0;
}
bool isBitNotSet(std::uint8_t element, int bit_index)
{
    return (element & (1 << bit_index)) == 0;
}

void draw(const std::array<std::uint8_t, 16> values)
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

// можно улучшить чуть-чуть
int manhattanDistance(const std::array<std::uint8_t, 16> &current, const uint8_t (&LowerDist)[8][8], const uint8_t (&UpperDist)[8][8])
{
    int distance = 0;

    for (int row = 0; row < 8; ++row)
    {

        for (int bit_pos = 0; bit_pos <= 7; ++bit_pos)
        {
            if (isBitSet(current[row], bit_pos))
            {
                distance += UpperDist[row][bit_pos];
            }
        }
    }

    for (int row = 8; row < 16; ++row)
    {
        for (int bit_pos = 0; bit_pos <= 7; ++bit_pos)
        {
            if (isBitSet(current[row], bit_pos))
            {
                distance += LowerDist[row - 8][bit_pos];
            }
        }
    }

    return distance;
}
struct Node
{
    std::array<std::uint8_t, 16> position;
    int g;
    int heuristic;
    Node *parent;

    Node(const std::array<std::uint8_t, 16> &pos, int g_value, const uint8_t (&LowerDist)[8][8], const uint8_t (&UpperDist)[8][8], Node *parent_node = nullptr)
        : g(g_value), parent(parent_node), position(pos)
    {
        heuristic = manhattanDistance(position, LowerDist, UpperDist);
    }

    Node swapBits(int index1, int bit1, int index2, int bit2, const uint8_t (&LowerDist)[8][8], const uint8_t (&UpperDist)[8][8]) const
    {
        std::array<std::uint8_t, 16> new_position;
        std::memcpy(new_position.data(), position.data(), 16 * sizeof(std::uint8_t));

        // Обмен битами
        new_position[index1] ^= (1 << bit1);
        new_position[index2] ^= (1 << bit2);

        return Node(new_position, g + 1, LowerDist, UpperDist, const_cast<Node *>(this));
    }
};

bool areEqual(const std::uint8_t a[16], const std::uint8_t b[16])
{
    return std::memcmp(a, b, 16) == 0;
}
struct Compare
{
    bool operator()(const Node &n1, const Node &n2)
    {
        return (n1.heuristic + n1.g) > (n2.heuristic + n2.g);
    }
};

// надо разделять ход
void generateNodesAndInsert(
    std::priority_queue<Node, std::vector<Node>, Compare> &pq,
    const uint8_t (&LowerDist)[8][8],
    const uint8_t (&UpperDist)[8][8],
    const std::vector<std::pair<int, int>> &combinations,
    bool turn)
{
    Node node = pq.top();

    for (int i = 0; i < 8; ++i)
    {
        for (int bit_pos = 0; bit_pos <= 7; ++bit_pos)
        {

            // Типа если 1
            if (isBitSet(node.position[i], bit_pos))
            {
                for (const auto &combination : combinations)
                {
                    int jump_i_1 = i + combination.first;
                    int jump_bit_pos_1 = bit_pos + combination.second;

                    // На айпаде схема полная
                    // Прыжок 1 на доске? Да:
                    if (0 <= jump_i_1 && jump_i_1 < 8 && 0 <= jump_bit_pos_1 && jump_bit_pos_1 < 8)
                    {
                        // Свободен прыжок 1? Да:
                        if (isBitNotSet(node.position[jump_i_1], jump_bit_pos_1))
                        {
                            // Были? Нет:
                            pq.push(node.swapBits(i, bit_pos, jump_i_1, jump_bit_pos_1, LowerDist, UpperDist));
                            // Были? Да - сделать:
                        }
                        // Свободен прыжок 2? Нет:
                        else
                        {
                            int jump_i_2 = i + combination.first * 2;
                            int jump_bit_pos_2 = bit_pos + combination.second * 2;
                            // Прыжок 2 на доске? Да:
                            if (0 <= jump_i_2 && i + jump_i_2 < 8 && 0 <= jump_bit_pos_2 && jump_bit_pos_2 < 8)
                            {
                                // Прыжок 2 свободен? Да:
                                if (isBitNotSet(node.position[jump_i_2], jump_bit_pos_2))
                                {
                                    // Были? Нет:
                                    pq.push(node.swapBits(i, bit_pos, jump_i_2, jump_bit_pos_2, LowerDist, UpperDist));
                                    // Были? Да - сделать:
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (turn)
    {
    }
}

int main()
{
    uint8_t LowerDist[8][8] = {
        {0, 0, 0, 1, 2, 3, 4, 5},
        {0, 0, 0, 1, 2, 3, 4, 5},
        {0, 0, 0, 1, 2, 3, 4, 5},
        {1, 1, 1, 2, 3, 4, 5, 6},
        {2, 2, 2, 3, 4, 5, 6, 7},
        {3, 3, 3, 4, 5, 6, 7, 8},
        {4, 4, 4, 5, 6, 7, 8, 9},
        {5, 5, 5, 6, 7, 8, 9, 10}};

    uint8_t UpperDist[8][8] = {
        {10, 9, 8, 7, 6, 5, 5, 5},
        {9, 8, 7, 6, 5, 4, 4, 4},
        {8, 7, 6, 5, 4, 3, 3, 3},
        {7, 6, 5, 4, 3, 2, 2, 2},
        {6, 5, 4, 3, 2, 1, 1, 1},
        {5, 4, 3, 2, 1, 0, 0, 0},
        {5, 4, 3, 2, 1, 0, 0, 0},
        {5, 4, 3, 2, 1, 0, 0, 0}};

    std::vector<std::pair<int, int>> combinations = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    std::array<std::uint8_t, 16> start_position = {0b00000111, 0b00000111, 0b00000111, 0b00000000,
                                                   0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                   0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                   0b00000000, 0b11100000, 0b11100000, 0b11100000};

    std::array<std::uint8_t, 16> target_position = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                    0b00000000, 0b11100000, 0b11100000, 0b11100000,
                                                    0b00000111, 0b00000111, 0b00000111, 0b00000000,
                                                    0b00000000, 0b00000000, 0b00000000, 0b00000000};

    std::priority_queue<Node, std::vector<Node>, Compare> pq;

    Node node(start_position, 5, LowerDist, UpperDist);
    pq.push(node);

    Node topNode = pq.top();
    std::cout << "Top Node g: " << topNode.g << ", h: " << topNode.heuristic << std::endl;

    generateNodesAndInsert(pq, LowerDist, UpperDist, combinations, true);

    // std::cout << pq.size() << std::endl;

    while (pq.size() > 0)
    {
        auto n = pq.top();
        pq.pop();
        draw(n.position);
        std::cout << n.heuristic << std::endl;
        Sleep(1000);
        system("cls");
    }

    return 0;
}
// auto md_s = manhattanDistance(start_position, LowerDist, UpperDist);
// // std::cout << md_s << std::endl;

// // auto md_t = manhattanDistance(target_position, LowerDist, UpperDist);
// // std::cout << md_t << std::endl;

// // std::cout << areEqual(start_position, target_position) << std::endl;

// // draw(start_position);
// // draw(target_position);

// traverse_bits(start_position);
// std::uint8_t target_position[16] = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                                     0b00000000, 0b11100000, 0b11100000, 0b11100000,
//                                     0b00000111, 0b00000111, 0b00000111, 0b00000000,
//                                     0b00000000, 0b00000000, 0b00000000, 0b00000000};

// std::uint8_t start_position[16] = {0b00000001, 0b00000000, 0b00000000, 0b00000000,
//                                    0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                                    0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                                    0b00000000, 0b00000000, 0b00000000, 0b00000001};

// std::uint8_t target_position[16] = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
//                                     0b00000000, 0b00000000, 0b00000000, 0b10000000,
//                                     0b00000000, 0b00000000, 0b10000000, 0b00000000,
//                                     0b00000000, 0b00000000, 0b00000000, 0b00000000};

// randomDraw(100, 300);

// draw(start_position);
// draw(target_position);