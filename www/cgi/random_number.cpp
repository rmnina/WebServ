#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <sstream>

int main() 
{
    
    std::srand(std::time(NULL));
    int randomNumber = (std::rand() % 100) + 1;

    std::ostringstream html;
    html << "<html><head><title>Random Number</title></head>"
        << "<body><h1>Your random number is: " << randomNumber << "</h1></body></html>";

    // std::cout << "Content-Type: html/text\r\n\r\n"; 
    std::cout << html.str();
}