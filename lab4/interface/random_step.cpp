#include <iostream>
#include <windows.h>
#include <set>
#include <random>
#include <string>

// Функция для генерации случайной строки (буква от 'a' до 'p' и число от 1 до 15)
std::string getRandomString(const std::set<std::string> &used_values)
{
    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> letter_distr(0, 14); // Индексы для букв от 'a' до 'p'
    std::uniform_int_distribution<> number_distr(1, 15); // Числа от 1 до 15

    std::string random_value;
    do
    {
        // Генерируем случайную букву
        char letter = 'a' + letter_distr(gen);
        // Генерируем случайное число
        int number = number_distr(gen);

        // Формируем строку
        random_value = letter + std::to_string(number);

    } while (used_values.find(random_value) != used_values.end()); // Проверяем, что такого значения ещё не было

    return random_value;
}

int main()
{
    std::set<std::string> used_values; // Множество для хранения использованных строк
    std::string s;

    while (true)
    {
        std::cin >> s;

        // Проверяем, было ли это значение уже использовано
        if (used_values.find(s) != used_values.end())
        {
            std::cout << "This value has already been used. Try again: " << s << std::endl;
            continue; // Повторяем ввод
        }

        // Добавляем введенное значение в список использованных
        used_values.insert(s);

        // Генерируем случайную строку, которая ещё не была использована
        std::string random_string = getRandomString(used_values);

        // Добавляем сгенерированное значение в множество использованных
        used_values.insert(random_string);

        Sleep(500);

        // Выводим сгенерированное значение
        std::cout << random_string << std::endl;

        // Прерывание цикла если все возможные строки использованы (16 букв * 15 чисел = 240 уникальных строк)
        if (used_values.size() == 240)
        {
            std::cout << "All possible values have been used." << std::endl;
            break;
        }
    }

    return 0;
}
