#include <iostream>
#include <deque>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <set>

struct Node
{
    int value;
    int parent_id;

    Node(int val, int parent_id) : value(val), parent_id(parent_id) {}
};

void printSequenceLength(const std::deque<Node> &nodes, int index)
{
    if (index < 0 || index >= nodes.size())
    {
        std::cout << "Invalid index" << std::endl;
        return;
    }

    int nodes_count = 0;
    while (index != -1)
    {
        nodes_count += 1;
        index = nodes[index].parent_id;
    }
    int operations = nodes_count - 1;
    std::cout << "Count: " << operations << std::endl;
}

void printPathToRoot(const std::deque<Node> &nodes, int index)
{
    if (index < 0 || index >= nodes.size())
    {
        std::cout << "Invalid index" << std::endl;
        return;
    }

    while (true)
    {
        if (index != 0)
        {
            std::cout << nodes[index].value << " -> ";
            index = nodes[index].parent_id;
        }
        else
        {
            std::cout << nodes[index].value << std::endl;
            break;
        }
    }
}

// Two integers are given, as well as two operations – "add 3" and "multiply by 2". Find the minimum sequence of operations that allows you to get the second number from the first.
void task1(int start_value = 2, int end_value = 10000)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

    auto start = std::chrono::high_resolution_clock::now();

    nodes.emplace_back(start_value, -1);
    int index = 0;
    while (true)
    {
        int val = nodes[index].value + 3;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        val = nodes[index].value * 2;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        index += 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Executed time: " << duration.count() << " microseconds" << std::endl;
    printPathToRoot(nodes, nodes.size() - 1);
    printSequenceLength(nodes, nodes.size() - 1);
}

// Two integers are given, as well as two operations – "add 3", "multiply by 2" and "subtract 2". Find the minimum sequence of operations that allows you to get the second number from the first.
void task2(int start_value = 2, int end_value = 10000)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

    auto start = std::chrono::high_resolution_clock::now();

    nodes.emplace_back(start_value, -1);
    int index = 0;
    while (true)
    {
        int val = nodes[index].value + 3;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        val = nodes[index].value * 2;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        val = nodes[index].value - 2;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        index += 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Executed time: " << duration.count() << " microseconds" << std::endl;
    printPathToRoot(nodes, nodes.size() - 1);
    printSequenceLength(nodes, nodes.size() - 1);
}

// Two integers are given, as well as two operations – "add 3" and "multiply by 2". Find the minimum sequence of operations that allows you to get the second number from the first (backwards).
void task3(int start_value = 10000, int end_value = 2)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

    auto start = std::chrono::high_resolution_clock::now();

    nodes.emplace_back(start_value, -1);
    int index = 0;
    while (true)
    {
        int val = nodes[index].value - 3;
        if (passed_values.find(val) == passed_values.end())
        {
            nodes.emplace_back(val, index);
            passed_values.insert(val);
            if (nodes[nodes.size() - 1].value == end_value)
            {
                break;
            }
        }

        if (nodes[index].value % 2 == 0)
        {
            val = nodes[index].value / 2;
            if (passed_values.find(val) == passed_values.end())
            {
                nodes.emplace_back(val, index);
                passed_values.insert(val);
                if (nodes[nodes.size() - 1].value == end_value)
                {
                    break;
                }
            }
        }

        index += 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Executed time: " << duration.count() << " microseconds" << std::endl;
    printPathToRoot(nodes, nodes.size() - 1);
    printSequenceLength(nodes, nodes.size() - 1);
}

template <typename T>
bool hasIntersection(const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
{
    return std::any_of(set1.begin(), set1.end(), [&](const T &elem)
                       { return set2.find(elem) != set2.end(); });
}

template <typename T>
bool hasIntersectionAndPrint(const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
{
    auto it = std::find_if(set1.begin(), set1.end(), [&set2](const T &elem)
                           { return set2.find(elem) != set2.end(); });

    if (it != set1.end())
    {
        std::cout << "first Intersection: " << *it << std::endl;
        return true;
    }
    else
    {
        std::cout << "no Intersection" << std::endl;
        return false;
    }
}

void task4(int start_value = 2, int end_value = 10000)
{

    std::unordered_set<int> left_passed_values;
    std::unordered_set<int> right_passed_values;

    std::deque<Node> left_nodes;
    std::deque<Node> right_nodes;

    auto start = std::chrono::high_resolution_clock::now();

    left_nodes.emplace_back(start_value, -1);
    right_nodes.emplace_back(end_value, -1);

    int left_index = 0;
    int right_index = 0;

    while (true)
    {
        int val = left_nodes[left_index].value + 3;
        if (left_passed_values.find(val) == left_passed_values.end())
        {
            left_nodes.emplace_back(val, left_index);
            left_passed_values.insert(val);
        }
        val = left_nodes[left_index].value * 2;
        if (left_passed_values.find(val) == left_passed_values.end())
        {
            left_nodes.emplace_back(val, left_index);
            left_passed_values.insert(val);
        }

        val = right_nodes[right_index].value - 3;
        if (right_passed_values.find(val) == right_passed_values.end())
        {
            right_nodes.emplace_back(val, right_index);
            right_passed_values.insert(val);
        }

        if (right_nodes[right_index].value % 2 == 0)
        {
            val = right_nodes[right_index].value / 2;
            if (right_passed_values.find(val) == right_passed_values.end())
            {
                right_nodes.emplace_back(val, right_index);
                right_passed_values.insert(val);
            }
        }

        if (hasIntersection(right_passed_values, left_passed_values))
        {
            hasIntersectionAndPrint(right_passed_values, left_passed_values);
            break;
        }

        left_index += 1;
        right_index += 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Executed time: " << duration.count() << " microseconds" << std::endl;
}

int main()
{
    int start_value = 2;
    int end_value = 1000001;
    std::cout << "\nTask 1: " << start_value << " -> " << end_value << std::endl;
    task1(start_value, end_value);
    std::cout << "\nTask 2: " << start_value << " -> " << end_value << std::endl;
    task2(start_value, end_value);
    std::cout << "\nTask 3: " << end_value << " -> " << start_value << std::endl;
    task3(end_value, start_value);
    std::cout << "\nTask 4: " << start_value << " -> " << end_value << std::endl;
    task4(start_value, end_value);

    return 0;
}
