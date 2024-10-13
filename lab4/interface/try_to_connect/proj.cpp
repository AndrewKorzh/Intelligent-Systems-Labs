#include <iostream>
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
