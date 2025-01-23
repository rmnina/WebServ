#include <iostream>
#include <cstdlib> 
#include <ctime>   

int main() 
{
    std::cout << "Content-Type: text/html\r\n\r\n";

    std::srand(std::time(0));
    int randomNumber = (std::rand() % 100) + 1;
    std::cout << randomNumber << std::endl;

    return 0;
}