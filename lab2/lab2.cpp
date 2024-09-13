#include <iostream>
#include <string>
#include <cmath>
#include <deque>
#include <cstdlib>
#include <unistd.h>
#include <list>

size_t isPerfectSquare(size_t num)
{
    if (num == 0)
        return 0;

    double root = std::sqrt(static_cast<double>(num));

    size_t intRoot = static_cast<size_t>(std::round(root));

    if (intRoot * intRoot == num)
    {
        return intRoot;
    }
    else
    {
        return 0;
    }
}

void draw_field(const std::string &str, bool er)
{
    if (er)
    {
        system("cls");
    }

    size_t len = str.length();
    size_t root = isPerfectSquare(len);
    std::string layer = std::string(root * 3, '-');

    if (root == 0)
    {
        return;
    }

    std::cout << std::endl;
    for (size_t i = 0; i < root; ++i)
    {
        std::cout << layer << std::endl;
        for (size_t j = 0; j < root; ++j)
        {
            if (str[i * root + j] != '0')
            {
                std::cout << "|" << str[i * root + j] << "|";
            }
            else
            {
                std::cout << "   ";
            }
        }
        std::cout << std::endl;
        std::cout << layer << std::endl;
    }
    std::cout << std::endl;
}

void draw_sequence(const std::list<std::string> &seq, int sleep_time, bool er)
{
    for (std::string value : seq)
    {
        draw_field(value, er);
        sleep(sleep_time);
    }
}

int main()
{
    // std::list<std::string> seq4 = {"123456789ABCDE0F", "123456789ABCDEF0", "123456789AB0DEFC"};
    // draw_sequence(seq4, 1, true);

    std::list<std::string> seq3 = {"123456780", "123456708", "123456078", "123056478", "123506478"};
    draw_sequence(seq3, 1, true);
}