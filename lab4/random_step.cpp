#include <iostream>
#include <windows.h>
#include <set>
#include <stack>
#include <random>
#include <string>
#include <chrono>

// Функция для генерации случайной строки (буква от 'a' до 'p' и число от 1 до 15)
std::string getRandomString(const std::set<std::string> &used_values, std::mt19937 &gen)
{
    // Диапазоны для генерации букв и чисел
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

int main(int argc, char *argv[])
{
    // Генерация seed на основе текущего времени
    auto time_now = std::chrono::system_clock::now().time_since_epoch().count(); // Используем текущее время
    std::mt19937 gen(static_cast<unsigned int>(time_now));                       // Инициализация генератора случайных чисел с seed на основе времени

    std::set<std::string> used_values;  // Множество для хранения использованных строк
    std::stack<std::string> move_stack; // Стек для хранения ходов
    std::string s;

    if (argv[1][0] == '0')
    {
        std::string random_string = getRandomString(used_values, gen);

        used_values.insert(random_string);
        move_stack.push(random_string);
        std::cout << random_string << std::endl;
    }

    while (true)
    {
        std::cin >> s;

        // Если введено "u1", убираем последний ход
        if (s == "u1")
        {
            if (!move_stack.empty())
            {
                std::string last_move = move_stack.top(); // Получаем последний ход
                move_stack.pop();                         // Убираем его из стека
                used_values.erase(last_move);             // Убираем ход из множества использованных значений
                std::cout << "Last move undone: " << last_move << std::endl;
            }
            else
            {
                std::cout << "No moves to undo!" << std::endl;
            }
            continue; // Возвращаемся в начало цикла для нового ввода
        }

        // Проверяем, было ли это значение уже использовано
        if (used_values.find(s) != used_values.end())
        {
            std::cout << "This value has already been used. Try again: " << s << std::endl;
            continue; // Повторяем ввод
        }

        // Добавляем введенное значение в список использованных
        used_values.insert(s);
        move_stack.push(s); // Сохраняем ход в стек

        // Генерируем случайную строку, которая ещё не была использована
        std::string random_string = getRandomString(used_values, gen);

        used_values.insert(random_string);
        move_stack.push(random_string);

        Sleep(1);

        std::cout << random_string << std::endl;

        if (used_values.size() == 225)
        {
            std::cout << "All possible values have been used." << std::endl;
            break;
        }
    }

    return 0;
}
