#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "functions.h"

/**
 * Parsowanie argumentow i przekazanie sterowania do odpowiedniej funkcji
 */
int main(int argc, char **argv)
{
	int returnCode;

	if(argc < 2 || strcmp("help", argv[1]) == 0)
	{
		print_help();
		return EXIT_FAILURE;
	}

	if(strcmp("create", argv[1]) == 0)
	{
		if(argc < 4)
		{
			printf("Nie podano rozmiaru partycji\n");
			return EXIT_FAILURE;
		}

		return create(atoi(argv[2]), atoi(argv[3]));
	}
	else if(strcmp("delete", argv[1]) == 0)
	{
		return delete();
	}

	init();

	if(strcmp("map", argv[1]) == 0)
	{
		returnCode = map();
	}
	else if(strcmp("ls", argv[1]) == 0)
	{
		returnCode = ls();
	}
	else if(strcmp("cpto", argv[1]) == 0)
	{
		if(argc < 4)
		{
			printf("Nie podano nazwy pliku docelowego\n");
			return dispose(EXIT_FAILURE);
		}

		returnCode = cpto(argv[2], argv[3]);
	}
	else if(strcmp("cpfrom", argv[1]) == 0)
	{
		if(argc < 4)
		{
			printf("Nie podano nazwy pliku docelowego\n");
			return dispose(EXIT_FAILURE);
		}

		returnCode = cpfrom(argv[2], argv[3]);
	}
	else if(strcmp("rm", argv[1]) == 0)
	{
		if(argc < 3)
		{
			printf("Nie podano nazwy pliku do usunieca\n");
			return dispose(EXIT_FAILURE);
		}

		returnCode = rm(argv[2]);
	}
	else
	{
		printf("%s %s : Bledne polecenie!\n\n", argv[0], argv[1]);
		print_help();
		return dispose(EXIT_FAILURE);
	}


	return dispose(returnCode);
}
