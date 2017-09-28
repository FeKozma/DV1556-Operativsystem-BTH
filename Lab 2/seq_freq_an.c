#define FALSE 0
#define TRUE 1
#define NOT_FOUND -1

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>

struct Word {
	char word[256];
	int count;
	int used;
};

struct FreqTable {
	int nrOfWords;
	int nrOfUniqueWords;
	int arrayCapacity;
	struct Word *wordArray;
};
struct Parameters {
	char *fileContent;
	int fileSize;
	int threadNr;
};


void frequencyAnalysis(struct Parameters* info);
void printStats(struct FreqTable table);	//prints some statistics from a frequency table
int searchWord(struct FreqTable* table, char* searchWord);
struct FreqTable* initFreqTable(size_t size);
void deleteFreqTable(struct FreqTable* table);
void sortFreqTable(struct FreqTable* table);
void mergeFreqTable(struct FreqTable** result, struct FreqTable* src);	//merge a frequency table into another
size_t hash(char* word);		//hash function
void rehash(struct FreqTable** table, size_t numberOfWords);	//expands hashtable
size_t insert(struct FreqTable** table, char* word);	//inserts word in hashtble
int find(struct FreqTable* table, char* word);	//find a certain entry in a table
void userInteraction();

unsigned int nThreads = 1;
struct FreqTable* result;

int main(int argc, char ** argv)
{
	char fileName[256];
	unsigned int nThreads = 1;

	struct timeval startTime, endTime;
	double time;

	result = initFreqTable(1000);

	printf("Input file: ");
	scanf("%s", fileName);
	getchar();

	/* READ FILE */
	FILE* filePointer = fopen(fileName, "r");
	char* fileContent;
	size_t fileSize;
	fseek(filePointer, 0, SEEK_END);
	fileSize = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	fileContent = malloc((sizeof(char) * fileSize));
	fread(fileContent, sizeof(char), fileSize, filePointer);
	fclose(filePointer);


	/* START THREADS */
	gettimeofday(&startTime, NULL);
	struct Parameters* params = (struct Parameters*)malloc(nThreads * sizeof(struct Parameters));
	for(int i=0 ; i<nThreads ; i++)
	{
		params[i].threadNr = i;
		params[i].fileContent = fileContent;
		params[i].fileSize = fileSize;
	}
	frequencyAnalysis(params);
	gettimeofday(&endTime, NULL);
	userInteraction();
	time = endTime.tv_sec - startTime.tv_sec + (endTime.tv_usec - startTime.tv_usec)/1000000.0;
	printf("Analysis took %.3f seconds\n\n", time);
	printf("Now compiling results before presentation...\n");
	sortFreqTable(result);
	printStats(*result);
	deleteFreqTable(result);

	return 0;
}


void frequencyAnalysis(struct Parameters* info) 
{
	char *wordFromFile, *fileContent;
	int found;
	size_t fileSize = info->fileSize;

	int offset = info->threadNr * fileSize / nThreads;
	int readBytes;
	if (info->threadNr != nThreads - 1)
		readBytes = fileSize / nThreads;
	else
		readBytes = fileSize / nThreads + fileSize % nThreads;

	fileContent = malloc((sizeof(char) * readBytes+1));
	strncpy(fileContent,(info->fileContent)+offset,readBytes+1);
	fileContent[readBytes] = '\0';
	for (int i = 0; i < readBytes; i++)
		if (!isalpha(fileContent[i]) && fileContent[i] != 0x27)
			fileContent[i] = ' ';
	char* savePtr;
	wordFromFile = strtok_r(fileContent, " ", &savePtr);

	struct FreqTable* table;
	table = initFreqTable(1000);
	while (wordFromFile != NULL)
	{
		int j = 0;
		while(wordFromFile[j] != '\0') {
			wordFromFile[j] = tolower(wordFromFile[j]);
			j++;
		}
		table->nrOfWords++;
		size_t index = find(table, wordFromFile);
		if( index != -1 )
			table->wordArray[index].count++;
		else
		{
			insert(&table,wordFromFile);
		}
		wordFromFile = strtok_r(NULL, " ", &savePtr);
	}

	mergeFreqTable(&result,table);

	free(fileContent);
	deleteFreqTable(table);

}


void printStats(struct FreqTable table)
{
	printf("The 10 most frequent words are: \n\n");
	int stop;
	if (table.nrOfUniqueWords < 10) 
		stop = table.nrOfUniqueWords;
	else
		stop = 10;
	for (int i = 0 ; i < stop; i++)
		printf("%-25s%5d\n", table.wordArray[i].word, table.wordArray[i].count);
	printf("\n\n");
	
	printf("There are %d words in the file. \n", table.nrOfWords);
	printf("There are %d unique words in the file.\n\n", table.nrOfUniqueWords);

	int theIndex = searchWord(&table, "the");
	int theCount = theIndex == -1 ? 0 : table.wordArray[theIndex].count;
	printf("The word 'the' occurred %d times.\n\n", table.wordArray[theIndex].count);

	int aIndex = searchWord(&table, "a");
	int aCount = aIndex == -1 ? 0 : table.wordArray[aIndex].count;
	printf("The word 'a' occurred %d times\n\n.", aCount);
	int checkSum = 0;
	for (int i = 0; i < table.nrOfUniqueWords; i++)
		checkSum += table.wordArray[i].count;
	printf("Checksum: %d\n\n", checkSum);
}


int searchWord(struct FreqTable* table, char* searchWord)
{
	int found = FALSE;
	int index = -1;
	for (int i = 0; i < table->nrOfUniqueWords && !found; i++)
		if (strcmp(searchWord, table->wordArray[i].word) == 0)
		{
			found = TRUE;
			index = i;
		}
	return index;
}


struct FreqTable* initFreqTable(size_t size)
{
	struct FreqTable* table;
	table = malloc(sizeof(struct FreqTable));
	table->arrayCapacity = size;
	table->wordArray = malloc(sizeof(struct Word) * table->arrayCapacity);
	for(int i = 0 ; i<table->arrayCapacity ; i++)
	{
		table->wordArray[i].used = 0;
	}
	table->nrOfUniqueWords = 0;
	table->nrOfWords = 0;
	return table;
}


void deleteFreqTable(struct FreqTable* table)
{
	free(table->wordArray);
	free(table);
}


void sortFreqTable(struct FreqTable* table)
{
	int currentCount, j;
	struct Word temp;
	for (int i = 1; i < table->arrayCapacity ; i++)
	{
		j = i;
		while (j > 0 && table->wordArray[j-1].count < table->wordArray[j].count)
		{
			temp = table->wordArray[j-1];
			table->wordArray[j-1] = table->wordArray[j];
			table->wordArray[j] = temp;
			j--;
		}
	}
}


void mergeFreqTable(struct FreqTable** result, struct FreqTable* src)
{
	(*result)->nrOfWords += src->nrOfWords;
	for (int i=0 ; i<src->arrayCapacity ; i++)
	{
		if(src->wordArray[i].used != 0)
		{
			size_t index = find((*result),src->wordArray[i].word);
			if(index != -1)
			{
				(*result)->wordArray[index].count += src->wordArray[i].count;
			}
			else
			{
				index = insert(result, src->wordArray[i].word);
				if(index != -1)
				{
					(*result)->wordArray[index].count = src->wordArray[i].count;
				}
			}
		}
	}
}


size_t hash(char* word)
{
	size_t hash = 0;
	for (int i=0 ; i<strlen(word) ; ++i)
	{
		hash += word[i]*pow(39,i);
	}
	return hash;
}


void rehash(struct FreqTable** table, size_t numberOfWords)
{
	struct FreqTable** tmp;
	tmp = (struct FreqTable**)malloc(sizeof(struct FreqTable*));
	(*tmp) = initFreqTable(numberOfWords);
	for( int i = 0 ; i < (*table)->arrayCapacity ; i++)
	{
		if((*table)->wordArray[i].used != 0)
		{
			size_t index = insert(tmp, (*table)->wordArray[i].word);
			(*tmp)->wordArray[index].count = (*table)->wordArray[i].count;
		}
	}
	(*tmp)->nrOfWords = (*table)->nrOfWords;
	deleteFreqTable(*table);
	(*table) = (*tmp);
}


size_t insert(struct FreqTable** table, char* word)
{
	if ( (*table)->arrayCapacity/2 <= (*table)->nrOfUniqueWords)
	{
		rehash(table,2*(*table)->arrayCapacity);
	}
	int index = hash(word) % (*table)->arrayCapacity;
	int i = index;
	int retValue = 0;
	int finished = FALSE;
	while(!finished)
	{
		if((*table)->wordArray[i].used == 0){
			strcpy((*table)->wordArray[i].word, word);
			(*table)->wordArray[i].count = 1;
			(*table)->wordArray[i].used = 1;
			(*table)->nrOfUniqueWords++;
			finished = TRUE;
			retValue = i;
		}
		i = (i+1) % (*table)->arrayCapacity;
		if ( i == index )
		{
			finished = TRUE;
			retValue = -1;
		}
	}
	return retValue;
}


int find(struct FreqTable* table, char* word)
{
	int index = hash(word) % table->arrayCapacity;
	int i = index;
	int retValue = 0;
	int finished = FALSE;
	while(!finished)
	{
		if(strcmp(table->wordArray[i].word,word) == 0)
		{
			finished = TRUE;
			retValue = i;
		}
		if(table->wordArray[i].used == 0)
		{
			finished = TRUE;
			retValue = -1;
		}
		i = (i+1) % table->arrayCapacity;
		if ( i == index )
		{
			finished = TRUE;
			retValue = -1;
		}
	}
	return retValue;
}


void userInteraction()
{
	char userInput = '\0';

	printf("Enter a character (q = quit): ");
	while (userInput != 'q')
	{
		userInput = getchar();
		while (getchar() != '\n');
		printf("You typed %c\n", userInput);
		if (userInput != 'q')
			printf ("Enter another character: ");
	}
}
