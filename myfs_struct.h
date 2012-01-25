/*
 * myfs_structures.h
 *
 *  Created on: 19-01-2012
 *      Author: psmolenski
 */

#ifndef MYFS_STRUCTURES_H_
#define MYFS_STRUCTURES_H_

/**
 * Struktura deskryptora bloku
 */
struct INODE
{
	char fileName[16];			/*nazwa pliku, jezeli pusta, to blok jest wolny*/
	int fileSize;				/*rozmiar pliku*/
	int blockAddres;			/*adres bloku*/
	int nextBlock;				/*nastepny blok*/
	int nextFile;				/*nastepny plik*/
};


/**
 * Struktura metabloku partycji
 */
struct SUPERBLOCK
{
		int partitionSize;
		int blockSize;
		int dataBlockOffset;
		int firstFile;
		int lastFile;
		int firstFree;
		int lastFree;
};


#endif /* MYFS_STRUCTURES_H_ */
