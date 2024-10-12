#include <iostream>
#include <thread> // Для работы с потоками
#include <chrono> // Для работы с временем
#include <cctype> // Для функции toupper
#include <windows.h>
#include <string>

int main()
{
    std::string s;

    while (std::cin >> s)
    {
        s = "_" + s + "_";

        Sleep(2000);

        // Выводим результат
        std::cout << s << std::endl;
    }

    return 0;
}
