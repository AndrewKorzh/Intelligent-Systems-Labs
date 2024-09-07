#include <iostream>
#include <deque>

struct Chip
{
    int top;
    int down;

    Chip() : top(0), down(0) {}

    Chip(int t, int d) : top(t), down(d) {}

    void print() const
    {
        std::cout << "[" << top << ", " << down << "]";
    }
};

struct Node
{
    Chip chip;
    bool flip;
    std::deque<Chip> q;

    Node() : chip(), q(), flip(false) {}

    void print() const
    {

        std::cout << "Chip: ";
        chip.print();
        std::cout << "; ";

        std::cout << "C: " << flip << "; ";

        std::cout << " Deque: ";
        for (const auto &ch : q)
        {
            ch.print();
            std::cout << ", ";
        }
        std::cout << std::endl;
    }
};

void clearChipsAtIndex(std::deque<Node> &nodes, size_t index)
{
    if (index >= nodes.size())
    {
        std::cout << "Invalid index!" << std::endl;
        return;
    }

    nodes[index].q.clear();
}

std::deque<Chip> generateAllDominoChips()
{
    std::deque<Chip> chips;

    for (int top = 0; top <= 6; ++top)
    {
        for (int down = top; down <= 6; ++down)
        {
            chips.emplace_back(top, down);
        }
    }

    return chips;
}

bool checkLimitRools(const std::deque<Node> &nodes, int val)
{
    // Проверяем горизонтали
    for (int i = 0; i < 3; ++i)
    {
        int top_sum = 0;
        int down_sum = 0;
        for (int j = i * 6; j < (i * 6 + 6); ++j)
        {
            top_sum += nodes[j].chip.top;
            down_sum += nodes[j].chip.down;
        }
        if (top_sum > val || down_sum > val)
        {
            return false;
        }
    }

    // Проверяем по вертикали
    for (int i = 0; i < 6; ++i)
    {
        int sum = nodes[i].chip.top + nodes[i].chip.down + nodes[i + 6].chip.top + nodes[i + 6].chip.down + nodes[i + 12].chip.top + nodes[i + 12].chip.down;
        if (sum > val)
        {
            return false;
        }
    }

    // Проверяем по диаг
    int main_diagonal = nodes[0].chip.top + nodes[1].chip.down + nodes[8].chip.top + nodes[9].chip.down + nodes[16].chip.top + nodes[17].chip.down;
    int side_diagonal = nodes[12].chip.down + nodes[13].chip.top + nodes[8].chip.down + nodes[9].chip.top + nodes[4].chip.down + nodes[5].chip.top;

    if (main_diagonal > val || side_diagonal > val)
    {
        return false;
    }

    return true;
}

bool checkRools(const std::deque<Node> &nodes, int val)
{
    // Проверяем горизонтали
    for (int i = 0; i < 3; ++i)
    {
        int top_sum = 0;
        int down_sum = 0;
        for (int j = i * 6; j < (i * 6 + 6); ++j)
        {
            top_sum += nodes[j].chip.top;
            down_sum += nodes[j].chip.down;
        }
        if (top_sum != val || down_sum != val)
        {
            return false;
        }
    }

    // Проверяем по вертикали
    for (int i = 0; i < 6; ++i)
    {
        int sum = nodes[i].chip.top + nodes[i].chip.down + nodes[i + 6].chip.top + nodes[i + 6].chip.down + nodes[i + 12].chip.top + nodes[i + 12].chip.down;
        if (sum != val)
        {
            return false;
        }
    }

    // Проверяем по диаг
    int main_diagonal = nodes[0].chip.top + nodes[1].chip.down + nodes[8].chip.top + nodes[9].chip.down + nodes[16].chip.top + nodes[17].chip.down;
    int side_diagonal = nodes[12].chip.down + nodes[13].chip.top + nodes[8].chip.down + nodes[9].chip.top + nodes[4].chip.down + nodes[5].chip.top;

    if (main_diagonal != val || side_diagonal != val)
    {
        return false;
    }

    return true;
}

void printField(const std::deque<Node> &nodes)
{
    std::cout << "\n\n";
    for (int i = 0; i < 3; ++i)
    {
        for (int j = i * 6; j < (i * 6 + 6); ++j)
        {
            std::cout << nodes[j].chip.top << " ";
        }
        std::cout << std::endl;
        for (int j = i * 6; j < (i * 6 + 6); ++j)
        {
            std::cout << nodes[j].chip.down << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "\n\n";
}

// void printNodes(const std::deque<Node> &nodes)
// {
//     std::cout << "\n\n";
//     for (int i = 0; i < nodes.size(); ++i)
//     {
//         std::cout << "[" << nodes[i].chip.top << ", " << nodes[i].chip.down << "] {" << nodes[i].q.size() << "},   ";
//     }
//     std::cout << "\n\n";
// }

void printNodes(const std::deque<Node> &nodes)
{
    std::cout << "\n\n";
    for (int i = 0; i < nodes.size(); ++i)
    {
        std::cout << i + 1 << ") ";
        nodes[i].print();
    }
    std::cout << "\n\n";
}

int main()
{
    std::deque<Chip> domino_chips = generateAllDominoChips();
    std::cout << domino_chips.size() << std::endl;

    std::deque<Node> nodes;
    for (int i = 0; i < 18; ++i)
    {
        nodes.emplace_back();
    }

    nodes[0].q = domino_chips;

    printNodes(nodes);

    int index = 0;

    int count = 0;
    while (true)
    {
        count += 1;
        if (count % 10000 == 0)
        {
            printField(nodes);
        }
        // printNodes(nodes);
        if (index < 0)
        {
            std::cout << "  Impossible! No result!" << std::endl;
            break;
        }

        if (index == 18)
        {
            if (checkRools(nodes, 13))
            {
                std::cout << "Found!" << std::endl;
                break;
            }
            else
            {
                index -= 1;
            }
        }
        if (nodes[index].q.empty())
        {
            nodes[index].chip.down = 0;
            nodes[index].chip.top = 0;
            index -= 1;
        }
        nodes[index].chip.down = nodes[index].q.front().down;
        nodes[index].chip.top = nodes[index].q.front().top;
        nodes[index].q.pop_front();
        if (checkLimitRools(nodes, 13))
        {
            if (index < 17)
            {
                nodes[index + 1].q.clear();
                std::copy(nodes[index].q.begin(), nodes[index].q.end(), std::back_inserter(nodes[index + 1].q));

                // nodes[index + 1].q.assign(nodes[index].q.begin(), nodes[index].q.end());
            }
            index += 1;
        }
    }

    return 0;
}

//    std::cout << nodes.size();
// int main()
// {
//     std::deque<Chip> domino_chips = generateAllDominoChips();

//     std::deque<Node> nodes;
//     for (int i = 0; i < 18; ++i)
//     {
//         nodes.emplace_back();
//     }

//     nodes[0].q = domino_chips;

//     int index = 0;

//     std::cout << "Trying!" << std::endl;

//     int big_count = 0;
//     while (true)
//     {
//         // std::cout << "q: " << nodes[index].q.size() << std::endl;
//         // printField(nodes);
//         printNodes(nodes);
//         if (index < 0)
//         {
//             std::cout << "  Impossible! No result!" << std::endl;
//             break;
//         }

//         if (index == 18)
//         {
//             big_count += 1;
//             std::cout << "Big check - " << big_count << std::endl;
//             // big ckeck
//             if (checkRools(nodes, 13))
//             {
//                 std::cout << "Found!" << std::endl;
//                 break;
//             }
//             else
//             {
//                 index -= 1;
//             }
//         }
//         if (nodes[index].q.empty())
//         {
//             nodes[index].chip.down = 0;
//             nodes[index].chip.top = 0;
//             index -= 1;
//         }
//         nodes[index].chip.down = nodes[index].q.front().down;
//         nodes[index].chip.top = nodes[index].q.front().top;
//         nodes[index].q.pop_front();
//         if (checkLimitRools(nodes, 13))
//         {
//             if (index < 17)
//             {
//                 nodes[index + 1].q.clear();

//                 nodes[index + 1].q.assign(nodes[index].q.begin(), nodes[index].q.end());
//             }
//             index += 1;
//         }
//     }

//     // std::cout << checkLimitRools(nodes, 0);

//     return 0;
// }

// for (int i = 0; i < nodes.size(); ++i)
// {
//     std::cout << i + 1 << ") ";
//     nodes[i].print();
// }