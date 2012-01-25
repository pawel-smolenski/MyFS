#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "myfs_struct.h"
#include "functions.h"

void init(void)
{
	filename = getFilename();

	partitionFile = fopen(filename, "r+");

	if (partitionFile == NULL)
	{
		printf("Partycja nie istnieje\n");
		free(filename);
		return;
	}

	superblock = calloc(1, sizeof(struct SUPERBLOCK));
	fread(superblock, sizeof(struct SUPERBLOCK), 1, partitionFile);

	iNodeTableSize = calculateINodeTableSize(superblock->partitionSize, superblock->blockSize);
	iNodeTable = (struct INODE *)calloc(iNodeTableSize, sizeof(struct INODE));
	fread(iNodeTable, sizeof(struct INODE), iNodeTableSize, partitionFile);


}

int dispose(int returnCode)
{
	fseek(partitionFile, 0, SEEK_SET);
	fwrite(superblock, sizeof(struct SUPERBLOCK), 1, partitionFile);
	fwrite(iNodeTable, sizeof(struct INODE), iNodeTableSize, partitionFile);

	free(filename);
	fclose(partitionFile);
	free(superblock);
	free(iNodeTable);


	return returnCode;
}

int print_help(void)
{
	printf("My File System\n");
	printf("  create SIZE BLOCKSIZE - Tworzy nowa partycje o rozmiarze SIZE bajtow\n");
	printf("  delete - Usuwa partycje\n");
	printf("  map - Wyswietla mape partycji\n");
	printf("  ls - Wyswietla zawartość katalogu glownego\n");
	printf("  cpto SOURCE FILENAME - Kopiuje plik z lokalnego dysku na partycję\n");
	printf("  cpfrom FILENAME DEST - Kopiuje plik z partycji na lokalny dysk\n");
	printf("  rm FILENAME - Usuwa plik z partycji\n");

	return 0;
}

char *getFilename(void)
{
	char *filename = calloc(strlen(MYFS_FILENAME) + 3, sizeof(char));

	strcpy(filename, "./");

	strcat(filename, MYFS_FILENAME);

	return filename;
}

int calculateINodeTableSize(int size, int blockSize)
{
	return size / (blockSize + sizeof(struct INODE));
}

int filenameExists(char* filename)
{
	int fileAddress;

	for(fileAddress = superblock->firstFile; fileAddress >= 0; fileAddress = iNodeTable[fileAddress].nextFile)
	{
		if(strcmp(iNodeTable[fileAddress].fileName, filename) == 0)
		{
			return 0;
		}
	}

	return -1;
}

int hasEnoughFreeSpace(int size)
{
	int freeBlockAddr;
	int freeSpace = 0;

	for(freeBlockAddr=superblock->firstFree; freeSpace < size; freeBlockAddr = iNodeTable[freeBlockAddr].nextBlock)
	{
		if(freeBlockAddr == -1)
		{
			return -1;
		}

		freeSpace += superblock->blockSize;
	}

	return 0;
}

int format(int size, int blockSize)
{
	int i;
	char buffer[1];
	buffer[0] = '\0';

	superblock = calloc(1, sizeof(struct SUPERBLOCK));

	superblock->partitionSize = size;
	superblock->blockSize = blockSize;
	superblock->firstFile = -1;
	superblock->lastFile = -1;
	superblock->firstFree = 0;

	iNodeTableSize = calculateINodeTableSize(size, blockSize);
	superblock->lastFree = iNodeTableSize-1;
	superblock->dataBlockOffset = sizeof(struct SUPERBLOCK) + sizeof(struct INODE)*iNodeTableSize;
	iNodeTable = (struct INODE*) calloc(iNodeTableSize, sizeof(struct INODE));

	for (i = 0; i < iNodeTableSize; i++)
	{
		iNodeTable[i].fileName[0] = '\0';
		iNodeTable[i].fileSize = 0;
		iNodeTable[i].blockAddres = i;
		iNodeTable[i].nextFile = -1;

		if(i+1 < iNodeTableSize)
		{
			iNodeTable[i].nextBlock = i+1;
		}
		else
		{
			iNodeTable[i].nextBlock = -1;
		}
	}

	fseek(partitionFile, 0, SEEK_SET);
	fwrite(superblock, sizeof(struct SUPERBLOCK), 1, partitionFile);
	fwrite(iNodeTable, sizeof(struct INODE), iNodeTableSize, partitionFile);
	fseek(partitionFile, size, SEEK_SET);

	fwrite(buffer, sizeof(char), 1, partitionFile);

	free(iNodeTable);
	return 0;
}

int create(int size, int blockSize)
{
	filename = getFilename();


	if (size < (sizeof(struct SUPERBLOCK) + sizeof(struct INODE) + 10 * blockSize))
	{
		printf("Rozmiar partycji jest nieprawidlowy\n");
		return 1;
	}

	partitionFile = fopen(filename, "wb");

	format(size, blockSize);

	fclose(partitionFile);

	free(filename);

	return 0;
}

int delete(void)
{
	char *command;
	char decision;

	filename = getFilename();

	partitionFile = fopen(filename, "r");
	if (partitionFile == NULL)
	{
		printf("Partycja nie istnieje\n");
		free(filename);
		return 1;
	}
	fclose(partitionFile);

	printf("Czy na pewno usunac partycje? y/n: ");
	scanf("%c", &decision);

	if (decision == 'n')
	{
		printf("Usuniecie partycji anulowane\n");
		return 0;
	}

	command = calloc(strlen(filename) + 4, sizeof(char));
	strcpy(command, "rm ");
	strcat(command, filename);

	system(command);

	printf("Partycja zostala usunieta\n");

	free(command);
	free(filename);

	return 0;
}

int map(void)
{
	int i;

	printf("Size: %d  Block: %d  DataOffset: %d FirstFile: %d  LastFile: %d FirstFree: %d LastFree: %d\n", superblock->partitionSize, superblock->blockSize, superblock->dataBlockOffset, superblock->firstFile, superblock->lastFile, superblock->firstFree, superblock->lastFree);

	printf("ID\tFilename\tFileSize\tNextFile\tNextBlock\n" );


	for(i = 0; i < iNodeTableSize; i++)
	{
		printf("%3d", i);
		if(strlen(iNodeTable[i].fileName) > 0)
		{
			printf("\t%15s", iNodeTable[i].fileName);
			printf("\t%5d", iNodeTable[i].fileSize);
		}
		else
		{
			printf("\tEMPTY BLOCK    ");
			printf("\t  -  ");
		}
		printf("\t%d", iNodeTable[i].nextFile);
		printf("\t%d", iNodeTable[i].nextBlock);
		printf("\n");
	}

	return 0;
}

int ls(void)
{
	int fileAddr;

	if(superblock->firstFile == -1)
	{
		printf("Brak plikow na dysku\n");
	}
	else
	{
		for(fileAddr = superblock->firstFile; fileAddr != -1; fileAddr = iNodeTable[fileAddr].nextFile)
		{
			printf("%15s   %-5d\n", iNodeTable[fileAddr].fileName, iNodeTable[fileAddr].fileSize);
		}
	}

	return EXIT_SUCCESS;
}

int cpto(char *source, char *filename)
{
	FILE* sourceFile;
	int sourcefileSize;
	int numberOfBlocks;
	int blockAddr, firstBlockAddr;
	struct INODE* currentBlock = NULL;
	struct INODE* previousBlock = NULL;
	int i, read;
	char *buffer;


	sourceFile = fopen(source, "rb");

	if(sourceFile == NULL)
	{
		printf("Plik zrodlowy nieczytelny\n");
		return EXIT_FAILURE;
	}

	if(strlen(filename) > 15)
	{
		printf("Zbyt dluga nazwa pliku\n");
		fclose(sourceFile);
		return EXIT_FAILURE;
	}


	if(filenameExists(filename) == 0)
	{
		printf("Plik o podanej nazwie juz istnieje\n");
		fclose(sourceFile);
		return EXIT_FAILURE;
	}


	/*Obliczanie liczby potrzebnych blokow*/
	fseek(sourceFile, 0L, SEEK_END);
	sourcefileSize = ftell(sourceFile);
	numberOfBlocks = ceil((double)sourcefileSize/(double)superblock->blockSize);
	fseek(sourceFile, 0L, SEEK_SET);


	/*Sprawdzanie dostepnego na dysku miejsca*/
	if(superblock->firstFree == -1 || hasEnoughFreeSpace(sourcefileSize) == -1)
	{
		printf("Brak wsytarczajace ilosci miejca na dysku\n");
		fclose(sourceFile);
		return EXIT_FAILURE;
	}

	buffer = (char *)calloc(superblock->blockSize, sizeof(char));

	/*Kopiowanie pliku*/
	for(i=0, blockAddr = superblock->firstFree; i < numberOfBlocks; i++)
	{
		currentBlock = &(iNodeTable[blockAddr]);

		if(i == 0)
		{
			firstBlockAddr = blockAddr;
			if(superblock->firstFile == -1)
			{
				superblock->firstFile = blockAddr;
			}
		}


		superblock->firstFree = currentBlock->nextBlock;

		if(superblock->lastFree == blockAddr)
		{
			superblock->lastFree = -1;
		}

		strcpy(currentBlock->fileName, filename);
		currentBlock->fileSize = sourcefileSize;
		currentBlock->nextFile = -1;

		if(previousBlock != NULL)
		{
			previousBlock->nextBlock = blockAddr;
		}


		read = fread(buffer, sizeof(char), superblock->blockSize, sourceFile);

		fseek(partitionFile, blockAddr*superblock->blockSize+superblock->dataBlockOffset, SEEK_SET);
		fwrite(buffer, sizeof(char), read, partitionFile);

		previousBlock = currentBlock;
		blockAddr = currentBlock->nextBlock;
	}


	currentBlock->nextFile = -1;
	currentBlock->nextBlock = -1;
	iNodeTable[superblock->lastFile].nextFile = firstBlockAddr;
	superblock->lastFile = firstBlockAddr;


	free(buffer);

	return EXIT_SUCCESS;
}

int cpfrom(char *filename, char *destination)
{
	int blockAddr;
	struct INODE* currentFile;
	FILE* destinationFile;
	int bytesToCopy;
	char* buffer;

	/*Wyszukuje plik o podanej nazwie*/
	for(blockAddr = superblock->firstFile; blockAddr != -1; blockAddr = currentFile->nextFile)
	{
		currentFile = &iNodeTable[blockAddr];
		if(strcmp(currentFile->fileName, filename) == 0)
		{
			break;
		}

	}

	if(blockAddr == -1)
	{
		printf("Plik o podanej nazwie nie zostal znaleziony\n");
		return EXIT_FAILURE;
	}

	destinationFile = fopen(destination, "wb");

	if(destinationFile == NULL)
	{
		printf("Nie mozna utworzyc pliku docelowego\n");
		return EXIT_FAILURE;
	}

	bytesToCopy = currentFile->fileSize;

	buffer = (char *)calloc(superblock->blockSize, sizeof(char));

	while(bytesToCopy >= superblock->blockSize)
	{
		fseek(partitionFile, blockAddr*superblock->blockSize+superblock->dataBlockOffset, SEEK_SET);
		fread(buffer, sizeof(char), superblock->blockSize, partitionFile);

		fwrite(buffer, sizeof(char), superblock->blockSize, destinationFile);

		bytesToCopy -= superblock->blockSize;
		blockAddr = iNodeTable[blockAddr].nextBlock;
	}

	if(bytesToCopy != 0)
	{
		fseek(partitionFile, blockAddr*superblock->blockSize+superblock->dataBlockOffset, SEEK_SET);
		fread(buffer, sizeof(char), bytesToCopy, partitionFile);

		fwrite(buffer, sizeof(char), bytesToCopy, destinationFile);
	}

	free(buffer);

	fclose(destinationFile);

	return EXIT_SUCCESS;
}

int rm(char *filename)
{
	int blockAddr, prevBlockAddr = -1;
	struct INODE* currentFile;

	/*Wyszukuje plik o podanej nazwie*/
	for(blockAddr = superblock->firstFile; blockAddr != -1; blockAddr = currentFile->nextFile)
	{
		currentFile = &iNodeTable[blockAddr];
		if(strcmp(currentFile->fileName, filename) == 0)
		{
			break;
		}
		prevBlockAddr = blockAddr;

	}

	if(blockAddr == -1)
	{
		printf("Plik o podanej nazwie nie zostal znaleziony\n");
		return EXIT_FAILURE;
	}


	if(blockAddr == superblock->firstFile)
	{
		superblock->firstFile = currentFile->nextFile;
	}

	if(blockAddr == superblock->lastFile)
	{
		superblock->lastFile = prevBlockAddr;
	}

	if(prevBlockAddr != -1)
	{
		iNodeTable[prevBlockAddr].nextFile = currentFile->nextFile;
	}

	while(blockAddr != -1)
	{
		currentFile->fileName[0] = '\0';
		currentFile->fileSize = 0;
		currentFile->nextFile = -1;

		iNodeTable[superblock->lastFree].nextBlock = blockAddr;
		superblock->lastFree = blockAddr;

		if(superblock->firstFree == -1)
		{
			superblock->firstFree = blockAddr;
		}

		blockAddr = currentFile->nextBlock;
		currentFile->nextBlock = -1;
		currentFile = &iNodeTable[blockAddr];
	}

	return EXIT_SUCCESS;
}
