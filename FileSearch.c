#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int findFile ( int choice, char filename[]) {
     DIR * dir = opendir(".");
     struct dirent *entry;
    struct stat info;

    long best_size = 0;
    int found = 0;

    if (dir == NULL){
        return 0;
    }

    while ((entry = readdir(dir )) != NULL){
        int len = strlen (entry -> d_name);

        if (strncmp ( entry ->d_name, "movies_", 7) == 0 && len>= 4 && strcmp (entry->d_name + len - 4, ".csv") ==0){
            if (stat (entry -> d_name, &info) == 0 && S_ISREG (info.st_mode)) {

                if ( found == 0 || (choice == 1 && info.st_size > best_size) || (choice == 2 && info.st_size < best_size)){
                    best_size = info.st_size;
                    strcpy (filename, entry -> d_name);
                    found = 1;
                }
            }
        }
    }

    closedir (dir);
    return found;
}

void selectFile(char filename[]) {
    int choice;
    int found = 0;

    while (!found){
        printf("\nWhich file you want to process?\n");
        printf ( "Enter 1 to pick the largest file\n");
        printf( "Enter 2 to pick the smallest file\n");
        printf ("Enter 3 to specify the name of a file\n\n");
        printf ( "Enter a choice from 1 to 3: ");

        scanf("%d", &choice);

        if (choice == 1 || choice == 2){
            found = findFile(choice, filename);

            if (! found) {
                printf("No matching file was found.\n");
            }
        }
        else if ( choice == 3) {
            struct stat info;

            printf("Enter the complete file name: ");
            scanf ("%255s", filename);
        }

        if ( stat ( filename, &info) == 0 && S_ISREG( info.st_mode)) {
            found = 1;
        } else {
            printf ("The file %s was not found. Try again\n", filename);
        }
    }
    else {
        printf( "You entered an incorrect choice. Try again.\n");
    }
}

int createDirectory ( char directory[]){
     int random_number;
    int created = 0;

    for (int i = 0; i < 100000; i++){
         random_number = rand() % 100000;

         sprintf (directory, "movies.%d", random_number);

         if (mkdir ( directory, 0750) == 0){
            created = 1;
            break;
         }
    }

    if (created ) {
        chmod( directory, 0750);
        printf ("Created directory with name %s\n", directory);
        return 1;
    }
    return 0;
}

void processFile(char fileName[], char directory[]) {
    FILE *input = fopen(fileName, "r");

    if ( input ==NULL){
        printf ( "Could not open file.\n");
        return;
    }
    char line[1000];

    // skip header
    fgets ( line, sizeof (line), input);

    while ( fgets( line, sizeof(line), input) != NULL) {
        char title[100];
        char languages[ 100];
        int year;
        double rating;

        if (sscanf (line, "%499[^,],%d,%199[^,],%lf", title, &year, languages, &rating) == 4) {
             char output_name[300];
            sprintf ( output_name, "%s/%d.txt", directory, year);
            FILE *output = fopen ( output_name, "a");

            if ( output != NULL){
                fprintf(output, "%s\n", title);
                fclose (output);
                chmod (output_name, 0640);
            }
        }
    }
    fclose (file);
}

int main(void) {

    int choice;

    char filename[256];
    char directory[256];

    srand (time ( NULL));

    while ( 1) {

        printf ("1. Select file to process\n");
        printf( "2. Exit the program\n\n");

        printf("Enter a choice 1 or 2: ");
        scanf("%d", &choice);

        if (choice == 1) {

            // fifle

            selectFile(filename);
            printf( "Now processing the chosen file named %s\n", filename);

            if (createDirectory(directory)) {
                processFile (filename, directory);
            } else {
                printf("Could not create directory.\n");
            }
            printf("\n");

        }
        else if (choice == 2) {

            break;

        }
        else {

            printf ("You entered an incorrect choice. Try again.\n\n");

        }
    }

    return 0;
}
