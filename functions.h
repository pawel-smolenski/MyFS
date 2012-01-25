
/**
 * nazwa pliku z partycją
 */
#define MYFS_FILENAME 			"partition.mfs"

/**
 * Rozmiar bloku danych
 */
#define MYFS_BLOCK_SIZE 		1024

char* filename;					/*nazwa pliku z partycją*/
FILE* partitionFile;			/*uchwyt do pliku z partycja*/
struct SUPERBLOCK* superblock;	/*metablok*/
struct INODE* iNodeTable;		/*tablica deskryptorow*/
int iNodeTableSize;				/*rozmiar tablicy deskryptorow*/

/**
 * Zwraca nazwę pliku
 */
char* getFilename(void);

/**
 * Oblicza rozmiar tablicy selektorow
 */
int calculateINodeTableSize(int, int);

/**
 * Sprawdza czy na dysku jest wystarczajaco duzo miejsca
 */
int hasEnoughFreeSpace(int);

/**
 * Inicjalizuje zmienne globalne
 */
void init(void);

/**
 * Zwalnia pamiec
 */
int dispose(int);

/**
 * Wyswietla pomoc
 */
int print_help(void);

/**
 * Formatuje partycje(tworzy metablok i tablice dekryptorow)
 */
int format(int, int);

/**
 * Tworzy partycje
 */
int create(int, int);

/**
 * Usuwa partycje
 */
int delete(void);

/**
 * Wyswietla zawartosc partycji
 */
int map(void);

/**
 * Wyswietla liste plikow
 */
int ls(void);

/**
 * Kopiuje plik z lokalnego dysku na partycje
 */
int cpto(char*, char*);

/**
 * Kopiuje plik z partycji na dysk lokalny
 */
int cpfrom(char*, char*);

/**
 * Usuwa plik
 */
int rm(char *);

/**
 * Sprawdza czy plik o podanej nazwie istnieje
 */
int filenameExists(char *);
