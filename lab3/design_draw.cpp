#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>   // Для system
#include <windows.h> // Для Sleep()

// Функция для чтения содержимого файла и сохранения строк в вектор
std::vector<std::string> readFileLines(const std::string &filename)
{
    std::ifstream inputFile(filename);
    if (!inputFile)
    {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return {};
    }

    std::vector<std::string> lines;
    std::string line;
    // Читаем файл построчно
    while (std::getline(inputFile, line))
    {
        lines.push_back(line); // Сохраняем каждую строку в вектор
    }

    inputFile.close(); // Закрываем файл
    return lines;
}

// Функция для вывода всех строк из файла, чередуя их со строкой "hello"
void printAllLinesWithHello(const std::string &filename, int waitTime)
{
    while (true)
    {
        auto lines = readFileLines(filename);

        // Стираем предыдущий вывод
        system("cls"); // Используйте "clear" для Unix-подобных систем

        // Цикл для вывода всех строк
        for (const auto &line : lines)
        {
            std::cout << line << std::endl; // Выводим строку из файла
        }

        Sleep(waitTime); // Задержка в миллисекундах

        system("cls");

        // Выводим строку "hello"
        std::cout << "hello" << std::endl;
        Sleep(1); // Задержка в миллисекундах
                  // Ожидание после вывода всех строк
    }
}

int main()
{
    // Замените "design2.txt" на имя вашего файла
    printAllLinesWithHello("design2.txt", 200); // Задержка 2000 мс
    return 0;
}
