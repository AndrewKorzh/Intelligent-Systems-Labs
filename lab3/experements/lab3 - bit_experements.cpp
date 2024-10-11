#include <iostream>
#include <cstdint>

void print_binary_ones(std::uint8_t values[], size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        for (int bit_pos = 0; bit_pos < 8; ++bit_pos)
        {
            if (values[i] & (1 << bit_pos))
            {
                std::cout << "Array Index: " << i << ", Bit Index: " << bit_pos << std::endl;
            }
        }
    }
}

bool isBitSet(int element, int bit_index)
{
    return (element & (1 << bit_index)) != 0;
}

int main()
{
    std::uint8_t values[8] = {0b00000111, 0b00000111, 0b00000111, 0b00000000,
                              0b00000000, 0b00000000, 0b00000000, 0b00000000};

    print_binary_ones(values, 8);

    int element_index = 2;
    int bit_index = 1;

    if (isBitSet(values[element_index], bit_index))
    {
        std::cout << "bit " << bit_index << " is set." << std::endl;
    }
    else
    {
        std::cout << "bit " << bit_index << " is not set." << std::endl;
    }

    return 0;
}