#include <unistd.h>

int     main(void)
{
        while (1)
                write(2, "Infinity generated\n", 19);
}
