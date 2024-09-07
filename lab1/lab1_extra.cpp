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
    std::deque<Chip> q;

    Node() : chip(), q() {}

    void print() const
    {

        std::cout << "Chip: ";
        chip.print();
        std::cout << std::endl;

        std::cout << "Deque of chips: ";
        for (const auto &ch : q)
        {
            std::cout << std::endl;
            ch.print();
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
        for (int down = 0; down <= 6; ++down)
        {
            chips.emplace_back(top, down);
            chips.emplace_back(down, top);
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

int main()
{
    std::deque<Chip> domino_chips = generateAllDominoChips();

    std::deque<Node> nodes;
    for (int i = 0; i < 18; ++i)
    {
        nodes.emplace_back();
    }

    nodes[0].q = domino_chips;

    int index = 0;

    while (true)
    {
        if (index < 0)
        {
            std::cout << "Impossible! No result!";
            break;
        }
        if (index == 18)
        {
            // big ckeck
        }
        if (nodes[index].q.empty())
        {
            // Пускаем сигнал назад типа
            index -= 1;
        }
        nodes[index].chip = nodes[index].q.front();
        nodes[index].q.pop_front();
    }

    // for (int i = 0; i < nodes.size(); ++i)
    // {
    //     std::cout << i + 1 << ") ";
    //     nodes[i].print();
    // }

    std::cout << checkLimitRools(nodes, 0);

    return 0;
}
