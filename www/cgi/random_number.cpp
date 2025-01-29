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
        << "<body><p>Your sacred random number is:</p>" << "<p id=\"random-nb-result\">" << randomNumber << "</p></body></html>";

    // std::cout << "Content-Type: html/text\r\n\r\n"; 
    std::cout << html.str();
}
