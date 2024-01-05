#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_NAME_C       "main.c"
#define FILE_NAME_CPP     "main.cpp"
#define FILE_NAME_JAVA    "Main.java"
#define FILE_NAME_C_SHARP "Main.cs"
#define FILE_NAME_PYTHON  "Main.py"

#define TEXT_PREFIX_NORMAL_TIME      "Tiempo="
#define TEXT_PREFIX_TRANSLATION_TIME "TiempoTraduccion="
#define TEXT_PREFIX_ENGLISH_TIME     "Time="


#define MAX_CHARACTERS_DIRECTORY 512
#define MAX_CHARACTERS_FILENAME  2048

#define MAX_CHARACTERS_FILETYPE 128

#define MAX_CHARACTERS_PER_LINE 128

//Name_Prefix.csv
#define OUTPUT_FILENAME "%s_%s.csv"


#define C_0 0
#define C_1 1
#define C_2 2

#define FAILURE_EXIT -1
#define SUCCESS_EXIT  1

typedef enum {false, true} bool;

typedef enum {PREFIX_NORMAL, PREFIX_TRANSLATION, PREFIX_ENGLISH, TOTAL_PREFIXES} PREFIX_TYPE;
typedef enum {TYPE_C, TYPE_CPP, TYPE_JAVA, TYPE_CSHARP, TYPE_PYTHON, TOTAL_TYPES} FILE_TYPE;

const char *listPrefix[TOTAL_PREFIXES] = {TEXT_PREFIX_NORMAL_TIME,
                                          TEXT_PREFIX_TRANSLATION_TIME,
                                          TEXT_PREFIX_ENGLISH_TIME
                                         };

const char *listTypes[TOTAL_TYPES] = {FILE_NAME_C, FILE_NAME_CPP, FILE_NAME_JAVA,
                                      FILE_NAME_C_SHARP, FILE_NAME_PYTHON
                                     };



typedef struct
{
    int hours, minutes;
} timeCounter;


char directoryPath[MAX_CHARACTERS_DIRECTORY+C_2];
char reportName[MAX_CHARACTERS_FILENAME+C_2];

void searchSubdirectories(char* path);
int fstatat (int __fd, const char *__restrict __file,
             struct stat *__restrict __buf, int __flag);

FILE_TYPE getFileType(char* name);
void searchPrefixInFile(char* path);
timeCounter extractValues (char* textLine, PREFIX_TYPE prefixType);
void writeToDocument(timeCounter timeToSave, PREFIX_TYPE type);


int main()
{
    printf("Enter a report name: ");
    scanf("%s", &reportName);
    getchar();

    printf("Enter a directory for search: ");

    fgets(directoryPath, MAX_CHARACTERS_DIRECTORY, stdin);
    printf("%s\n", directoryPath);

    directoryPath[strlen(directoryPath) - C_1] = '\0';
    searchSubdirectories(directoryPath);





    return SUCCESS_EXIT;
}


void searchSubdirectories(char* path)
{
    struct dirent* rootFolderSubdirectory;
    DIR* rootFolder;

    rootFolder = opendir(path);

    if (rootFolder == NULL)
    {
        perror("That path doesn't exist!");
    }
    else
    {
        while ((rootFolderSubdirectory = readdir(rootFolder)) != NULL)
        {
            struct stat currentObject;
            char subPath[MAX_CHARACTERS_FILENAME+C_2];
            char* subDirectoryName = rootFolderSubdirectory->d_name;

            if (strcmp(".", subDirectoryName) == C_0 || strcmp(subDirectoryName, "..") == C_0)
            {
                continue;
            }
            else
            {
                snprintf(subPath, MAX_CHARACTERS_FILENAME, "%s/%s", path, subDirectoryName);

                fstatat(dirfd(rootFolderSubdirectory), subPath, &currentObject, C_0);

                if (S_ISDIR(currentObject.st_mode))
                {
                    searchSubdirectories(subPath);
                }
                else
                {
                    if (getFileType(subDirectoryName) >= C_0)
                    {
                        searchPrefixInFile(subPath);
                    }
                }
            }
        }
        closedir(rootFolder);
    }
}

//Function returns -1, if the file isn't in the enum FILE_TYPE otherwise returns
//the FILE_TYPE for the file

FILE_TYPE getFileType(char* name)
{
    int filetype, i;

    filetype = -C_1;

    for (i = C_0; i < TOTAL_TYPES; i++)
    {
        if (strcmp(name, listTypes[i]) == C_0)
        {
            filetype = i;
            printf("Found type: %s\n\n", listTypes[filetype]);
            break;
        }
    }
    return filetype;
}

void searchPrefixInFile(char* path)
{
    bool fileExists;
    char actualLine[MAX_CHARACTERS_PER_LINE+C_2];
    FILE *actualFile, *prefix;

    timeCounter timeFromFile;


    int actualPrefixId;

    actualFile = fopen(path, "r");

    fileExists = actualFile != NULL;
    prefix = NULL;
    if (fileExists)
    {
        do
        {
            fgets(actualLine, MAX_CHARACTERS_PER_LINE, actualFile);

            for (actualPrefixId = C_0; actualPrefixId < TOTAL_PREFIXES; actualPrefixId++)
            {
                prefix = (FILE*)strstr(actualLine, listPrefix[actualPrefixId]);
                //Check if the prefix is a substring of the line.
                if (prefix != NULL)
                {
                    timeFromFile = extractValues((char *)prefix, actualPrefixId);
                    writeToDocument(timeFromFile, actualPrefixId);
                    break;
                }
            }
        }
        while (!feof(actualFile));
        fclose(actualFile);
    }
    else
    {
        printf("File doesn't exists!. . .\n\n");
    }
}

timeCounter extractValues (char* textLine, PREFIX_TYPE prefixType)
{
    timeCounter timeC;

    char prefixSearched[MAX_CHARACTERS_FILETYPE+C_2];

    strcpy(prefixSearched, listPrefix[prefixType]);

    strcat(prefixSearched, "%d:%d");
    printf("Scanning: %s\n", textLine);

    sscanf(textLine, prefixSearched,  &timeC.hours, &timeC.minutes);
    printf("Horas:%d Minutos: %d\n\n", timeC.hours, timeC.minutes);

    return timeC;
}

void writeToDocument(timeCounter timeToSave, PREFIX_TYPE type)
{
    char prefixText[MAX_CHARACTERS_FILETYPE];
    char outputFilename[MAX_CHARACTERS_FILENAME];

    FILE *file;

    strcpy(prefixText, listPrefix[type]);

    sprintf(outputFilename, OUTPUT_FILENAME,reportName ,prefixText);

    file = fopen(outputFilename, "a");

    fprintf(file, "%02d:%02d\n", timeToSave.hours,timeToSave.minutes);

    fclose(file);

}







