#include <stdio.h>
#include <pthread.h>

void	*example_function(void *name)
{
	printf("This is a print");
	return (NULL);
}

int main(int argc, char **argv)
{
	pthread_t *thread = NULL;
	if(pthread_create(thread, NULL, &example_function, NULL))
	{
		perror("Error al crear el hilo");
		return (1);
	}
	printf("Hijos de puta\n");
	printf("%d\n", argc);
	printf("%s\n", argv[1]);
}