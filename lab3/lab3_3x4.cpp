#include <iostream>
#include <vector>
#include <cstdint>
#include <array>
#include <cstdlib>
#include <windows.h>
#include <queue>
#include <cstring>
#include <utility>
#include <memory>
#include <unordered_set>

#include <memory>
#include <chrono>
#include <sstream>
#include <string>
void draw(const std::array<std::uint8_t, 16> values);

struct ArrayHash
{
    std::size_t operator()(const std::array<std::uint8_t, 16> &arr) const
    {
        std::size_t hash = 0;
        for (auto byte : arr)
        {
            hash = hash * 31 + byte;
        }
        return hash;
    }
};

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
    std::shared_ptr<Node> parent;

    Node(const std::array<std::uint8_t, 16> &pos, int g_value, const uint8_t (&LowerDist)[8][8], const uint8_t (&UpperDist)[8][8], std::shared_ptr<Node> parent_node = nullptr)
        : position(pos), g(g_value), parent(parent_node)
    {
        heuristic = manhattanDistance(position, LowerDist, UpperDist);
    }

    std::shared_ptr<Node> swapBits(int index1, int bit1, int index2, int bit2, const uint8_t (&LowerDist)[8][8], const uint8_t (&UpperDist)[8][8]) const
    {
        std::array<std::uint8_t, 16> new_position = position;

        new_position[index1] ^= (1 << bit1);
        new_position[index2] ^= (1 << bit2);

        return std::make_shared<Node>(new_position, g + 1, LowerDist, UpperDist, std::make_shared<Node>(*this));
    }
};

bool areEqual(const std::array<std::uint8_t, 16> &a, const std::array<std::uint8_t, 16> &b)
{
    return std::memcmp(a.data(), b.data(), 16) == 0;
}
struct Compare
{
    bool operator()(const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) const
    {
        return (n1->heuristic + n1->g) > (n2->heuristic + n2->g);
    }
};

void generateNodesAndInsert(
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Compare> &pq,
    const uint8_t (&LowerDist)[8][8],
    const uint8_t (&UpperDist)[8][8],
    const std::vector<std::pair<int, int>> &combinations,
    bool turn,
    const std::shared_ptr<Node> &current)
{
    // Ход верхних
    if (turn)
    {
        for (int i = 0; i < 8; ++i)
        {
            for (int bit_pos = 0; bit_pos <= 7; ++bit_pos)
            {
                if (isBitSet(current->position[i], bit_pos))
                {
                    for (const auto &combination : combinations)
                    {
                        int jump_i_1 = i + combination.first;
                        int jump_bit_pos_1 = bit_pos + combination.second;

                        if (0 <= jump_i_1 && jump_i_1 < 8 && 0 <= jump_bit_pos_1 && jump_bit_pos_1 < 8)
                        {
                            if (isBitNotSet(current->position[jump_i_1], jump_bit_pos_1) && isBitNotSet(current->position[jump_i_1 + 8], jump_bit_pos_1))
                            {
                                auto newNode = current->swapBits(i, bit_pos, jump_i_1, jump_bit_pos_1, LowerDist, UpperDist);
                                pq.push(newNode);
                            }
                            else
                            {
                                int jump_i_2 = jump_i_1 + combination.first;
                                int jump_bit_pos_2 = jump_bit_pos_1 + combination.second;

                                if (0 <= jump_i_2 && jump_i_2 < 8 && 0 <= jump_bit_pos_2 && jump_bit_pos_2 < 8)
                                {
                                    if (isBitNotSet(current->position[jump_i_2], jump_bit_pos_2) && isBitNotSet(current->position[jump_i_2 + 8], jump_bit_pos_2))
                                    {
                                        auto newNode = current->swapBits(i, bit_pos, jump_i_2, jump_bit_pos_2, LowerDist, UpperDist);
                                        pq.push(newNode);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // Ход нижних
    else
    {
        for (int i = 8; i < 16; ++i)
        {
            for (int bit_pos = 0; bit_pos <= 7; ++bit_pos)
            {
                if (isBitSet(current->position[i], bit_pos))
                {
                    for (const auto &combination : combinations)
                    {
                        int jump_i_1 = i + combination.first;
                        int jump_bit_pos_1 = bit_pos + combination.second;

                        if (8 <= jump_i_1 && jump_i_1 < 16 && 0 <= jump_bit_pos_1 && jump_bit_pos_1 < 8)
                        {
                            if (isBitNotSet(current->position[jump_i_1], jump_bit_pos_1) && isBitNotSet(current->position[jump_i_1 - 8], jump_bit_pos_1))
                            {
                                auto newNode = current->swapBits(i, bit_pos, jump_i_1, jump_bit_pos_1, LowerDist, UpperDist);
                                pq.push(newNode);
                            }
                            else
                            {
                                int jump_i_2 = jump_i_1 + combination.first;
                                int jump_bit_pos_2 = jump_bit_pos_1 + combination.second;

                                if (8 <= jump_i_2 && jump_i_2 < 16 && 0 <= jump_bit_pos_2 && jump_bit_pos_2 < 8)
                                {
                                    if (isBitNotSet(current->position[jump_i_2], jump_bit_pos_2) && isBitNotSet(current->position[jump_i_2 - 8], jump_bit_pos_2))
                                    {
                                        auto newNode = current->swapBits(i, bit_pos, jump_i_2, jump_bit_pos_2, LowerDist, UpperDist);
                                        pq.push(newNode);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void printToRoot(const std::shared_ptr<Node> &node, int sleep_time, std::string time_info)
{
    system("cls");
    std::vector<std::shared_ptr<Node>> path;
    auto current = node;
    while (current != nullptr)
    {
        path.push_back(current);
        current = current->parent;
    }

    auto len = path.size();

    for (auto it = path.rbegin(); it != path.rend(); ++it)
    {
        // std::cout << "Top Node g: " << (*it)->g << ", h: " << (*it)->heuristic << std::endl;
        std::cout << time_info << ", len: " << len << std::endl;
        draw((*it)->position);
        Sleep(sleep_time);
        if (std::next(it) != path.rend())
        {
            system("cls");
        }
    }
}

// Начальный узел уже внутри
std::shared_ptr<Node> AStar(
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Compare> &pq,
    const uint8_t (&LowerDist)[8][8],
    const uint8_t (&UpperDist)[8][8],
    const std::vector<std::pair<int, int>> &combinations,
    bool turn,
    const std::array<std::uint8_t, 16> &target_position)
{
    std::unordered_set<std::array<std::uint8_t, 16>, ArrayHash> closedSet;

    while (!pq.empty())
    {
        std::shared_ptr<Node> current = pq.top();
        pq.pop();

        if (areEqual(current->position, target_position))
        {
            return current;
        }
        if (closedSet.find(current->position) != closedSet.end())
        {
            continue;
        }

        closedSet.insert(current->position);

        generateNodesAndInsert(pq, LowerDist, UpperDist, combinations, turn, current);

        turn = !turn;
    }

    return nullptr;
}

int main()
{
    uint8_t LowerDist[8][8] = {
        {0, 0, 0, 0, 1, 2, 3, 4},
        {0, 0, 0, 0, 1, 2, 3, 4},
        {0, 0, 0, 0, 1, 2, 3, 4},
        {1, 1, 1, 1, 2, 3, 4, 5},
        {2, 2, 2, 2, 3, 4, 5, 6},
        {3, 3, 3, 3, 4, 5, 6, 7},
        {4, 4, 4, 4, 5, 6, 7, 8},
        {5, 5, 5, 5, 6, 7, 8, 9}};

    uint8_t UpperDist[8][8] = {
        {9, 8, 7, 6, 5, 5, 5, 5},
        {8, 7, 6, 5, 4, 4, 4, 4},
        {7, 6, 5, 4, 3, 3, 3, 3},
        {6, 5, 4, 3, 2, 2, 2, 2},
        {5, 4, 3, 2, 1, 1, 1, 1},
        {4, 3, 2, 1, 0, 0, 0, 0},
        {4, 3, 2, 1, 0, 0, 0, 0},
        {4, 3, 2, 1, 0, 0, 0, 0}};

    std::vector<std::pair<int, int>> combinations = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    std::array<std::uint8_t, 16> start_position = {0b00001111, 0b00001111, 0b00001111, 0b00000000,
                                                   0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                   0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                   0b00000000, 0b11110000, 0b11110000, 0b11110000};

    std::array<std::uint8_t, 16> target_position = {0b00000000, 0b00000000, 0b00000000, 0b00000000,
                                                    0b00000000, 0b11110000, 0b11110000, 0b11110000,
                                                    0b00001111, 0b00001111, 0b00001111, 0b00000000,
                                                    0b00000000, 0b00000000, 0b00000000, 0b00000000};

    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Compare> pq;

    auto node = std::make_shared<Node>(start_position, 0, LowerDist, UpperDist);
    pq.push(node);

    auto start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<Node> found = AStar(pq, LowerDist, UpperDist, combinations, true, target_position);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::string timeInfo = "time: " + std::to_string(duration.count()) + "s";

    printToRoot(found, 200, timeInfo);

    return 0;
}
