#include <unistd.h>

int     main(void)
{
        while (1)
                write(2, "I'm an infinite loop\n", 21);
}
