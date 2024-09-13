#include <iostream>
#include <deque>
#include <chrono>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <vector>

struct Node
{
    int value;
    int parent_id;

    Node(int val, int parent_id) : value(val), parent_id(parent_id) {}
};

void printDequeAsPath(const std::deque<int> &deq)
{
    for (size_t i = 0; i < deq.size(); ++i)
    {
        std::cout << deq[i];
        if (i != deq.size() - 1)
        {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}

std::deque<int> getPath(const std::deque<Node> &nodes, int index)
{
    std::deque<int> path;

    if (index < 0 || index >= nodes.size())
    {
        std::cout << "Invalid index" << std::endl;
        return path;
    }

    while (true)
    {
        if (index != -1)
        {
            path.emplace_back(nodes[index].value);
            index = nodes[index].parent_id;
        }
        else
        {
            break;
        }
    }
    return path;
}

// Two integers are given, as well as two operations – "add 3" and "multiply by 2". Find the minimum sequence of operations that allows you to get the second number from the first.
std::deque<int> task1(int start_value = 2, int end_value = 10000)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

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

    auto result = getPath(nodes, nodes.size() - 1);
    reverse(result.begin(), result.end());
    return result;
}

// Two integers are given, as well as two operations – "add 3", "multiply by 2" and "subtract 2". Find the minimum sequence of operations that allows you to get the second number from the first.
std::deque<int> task2(int start_value = 2, int end_value = 10000)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

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
    auto result = getPath(nodes, nodes.size() - 1);
    reverse(result.begin(), result.end());
    return result;
}

// Two integers are given, as well as two operations – "add 3" and "multiply by 2". Find the minimum sequence of operations that allows you to get the second number from the first (backwards).
std::deque<int> task3(int start_value = 10000, int end_value = 2)
{
    std::unordered_set<int> passed_values;
    std::deque<Node> nodes;

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

    auto result = getPath(nodes, nodes.size() - 1);
    reverse(result.begin(), result.end());
    return result;
}

template <typename T>
bool unorderedSetHasIntersection(const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
{
    return std::any_of(set1.begin(), set1.end(), [&](const T &elem)
                       { return set2.find(elem) != set2.end(); });
}

template <typename T>
int unorderedSetIntersection(const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
{
    auto it = std::find_if(set1.begin(), set1.end(), [&set2](const T &elem)
                           { return set2.find(elem) != set2.end(); });

    return *it;
}

int indexByValue(const std::deque<Node> &nodes, int value)
{
    int index;
    for (int i = nodes.size() - 1; i >= 0; i--)
    {
        if (nodes[i].value == value)
        {
            index = i;
            break;
        }
    }
}

std::deque<int> mergeNodesQeque(const std::deque<Node> &left_nodes, const std::deque<Node> &right_nodes, int intersection)
{
    int left_index = indexByValue(left_nodes, intersection);
    int right_index = indexByValue(right_nodes, intersection);

    auto left_path = getPath(left_nodes, left_index);
    reverse(left_path.begin(), left_path.end());
    auto right_path = getPath(right_nodes, right_nodes[right_index].parent_id);

    left_path.insert(left_path.end(), right_path.begin(), right_path.end());
    return left_path;
}

// Two integers are given, as well as two operations – "add 3" and "multiply by 2". Find the minimum sequence of operations that allows you to get the second number from the first (bidirectional search).
std::deque<int> task4(int start_value = 2, int end_value = 10000)
{

    std::unordered_set<int> left_passed_values;
    std::unordered_set<int> right_passed_values;

    std::deque<Node> left_nodes;
    std::deque<Node> right_nodes;

    left_nodes.emplace_back(start_value, -1);
    right_nodes.emplace_back(end_value, -1);

    int left_index = 0;
    int right_index = 0;

    std::deque<int> result;

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

        if (unorderedSetHasIntersection(right_passed_values, left_passed_values))
        {
            int intersection = unorderedSetIntersection(right_passed_values, left_passed_values);
            result = mergeNodesQeque(left_nodes, right_nodes, intersection);
            break;
        }

        left_index += 1;
        right_index += 1;
    }

    return result;
}

void executeFunction(int start_value, int end_value, std::deque<int> (*function)(int, int))
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = function(start_value, end_value);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Executed time: " << duration.count() << " milliseconds" << std::endl;
    printDequeAsPath(result);
    std::cout << "Operations: " << result.size() - 1 << std::endl;
}

int main()
{
    int start_value = 1;
    int end_value = 10000001;

    std::cout << "\nTask 1: " << start_value << " -> " << end_value << std::endl;
    executeFunction(start_value, end_value, task1);

    std::cout << "\nTask 2: " << start_value << " -> " << end_value << std::endl;
    executeFunction(start_value, end_value, task2);

    std::cout << "\nTask 3: " << start_value << " -> " << end_value << std::endl;
    executeFunction(end_value, start_value, task3);

    std::cout << "\nTask 4: " << start_value << " -> " << end_value << std::endl;
    executeFunction(start_value, end_value, task4);

    return 0;
}

// std::unordered_set<int> expand_left_front(std::deque<Node> &nodes, std::unordered_set<int> &passed_values, int front_begin, int front_end)
// {
//     int count = 0;
//     for (int i = front_begin; i <= front_end; i++)
//     {
//         int val = nodes[i].value + 3;
//         if (passed_values.find(val) == passed_values.end())
//         {
//             nodes.emplace_back(val, i);
//             passed_values.insert(val);
//         }
//         val = nodes[i].value * 2;
//         if (passed_values.find(val) == passed_values.end())
//         {
//             nodes.emplace_back(val, i);
//             passed_values.insert(val);
//         }
//     }
// }
// std::deque<int> task4_remastered(int start_value = 2, int end_value = 10000)
// {
//     std::unordered_set<int> left_passed_values;
//     std::unordered_set<int> right_passed_values;

//     std::deque<Node> left_nodes;
//     std::deque<Node> right_nodes;

//     left_nodes.emplace_back(start_value, -1);
//     right_nodes.emplace_back(end_value, -1);

//     int left_front_begin = 1;
//     int left_front_end = 1;

//     int right_front_begin = 1;
//     int right_front_end = 1;
// }