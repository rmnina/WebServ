#include <stdio.h>

int	main(int argc, char **argv)
{
	int	i;

	i = 0;
	if (argc != 2)
		return (1);
	while (*argv[i])
		i += 1;
	printf("length: %d\n", i);
	return (i);
}
