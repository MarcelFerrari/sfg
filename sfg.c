
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Copyright © 2018 Marcel Ferrari
*/
// Standard header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// Define arbitrary values
#define MAX_ARGS 5

#define LICENSE "license"
#define HELP "-help"
#define BS_STAT "-bs_stat"

#define PATH_LENGTH 5
#define PATH_ARG "path="
#define PATH_INDEX 0

#define SIZE_LENGTH 5
#define SIZE_ARG "size="
#define SIZE_INDEX 1

#define UNIT_LENGTH 5
#define UNIT_ARG "unit="
#define UNIT_INDEX 2

#define BS_LENGTH 3
#define BS_ARG "bs="
#define BS_INDEX 3

#define NAME_LENGTH 5
#define NAME_ARG "name="
#define NAME_INDEX 4

// Function prototypes
void arg_process(int argc, char* argv[]);
int rek_mkdir (char *path);
long long bs_calculate (void);
bool isNumeric (char *str);
char* concat(char* s1, char* s2);

// Define global variables
char* input_args[MAX_ARGS] = {NULL, NULL, NULL, NULL, NULL}; // Maximum of five arguments (path, size, unit, bs, name)


int main (int argc, char *argv[])
{
  // License
  printf("\nCopyright © 2018-2019 Marcel Ferrari\nThis program is free software and comes with ABSOLUTELY NO WARRANTY.\nFor more information type 'license'.\nFor the latest updates check https://github.com/MarcelFerrari/sfg\n\n ");
  // Check flags --> path, size, unit, (bs), (name)
  arg_process(argc, argv);
  // Extract path variable
  char* path = input_args[PATH_INDEX];
  char* size_str = input_args[SIZE_INDEX];
  char* unit_str = input_args[UNIT_INDEX];
  char* bs_str = input_args[BS_INDEX];
  char* name = input_args[NAME_INDEX];
  // Check for all needed values
  if (path == NULL || size_str == NULL || unit_str == NULL || bs_str == NULL) {
      fprintf (stderr, "Usage: sfg path='path' size='size' unit='unit' bs='block_size' name='filename' [bs_stat]\nor sfg -help\n");
      exit(0);
  }
  // Process numeric variables
  // OF unit
  long long unit;
  if (strcmp(unit_str, "gb") == 0)
    {
      unit = 1000000000;
    }
  else if (strcmp(unit_str, "mb") == 0)
    {
      unit = 1000000;
    }
  else if (strcmp(unit_str, "kb") == 0)
    {
      unit = 1000;
    }
  else if (strcmp(unit_str, "b") == 0)
    {
      unit = 1;
    }
  else
    {
      fprintf (stderr,
	       "Unknown unit! Please input 'gb', 'mb', 'kb' or 'b'\n");
      return 1;
    }
  // Check OF size
  long long size = atoi(size_str) * unit;
  if (size <= 0){
    size = atoi(size_str) * unit;
    fprintf(stderr, "Error: size cannot be neither 0 nor negative!\n");
    exit(1);
  }

  // Check OF name
  if (name == NULL)
    {
      name = "file";
    }

  // Check block size
  long long bs = atoi(bs_str);
  if (strcmp("auto", bs_str) == 0) {
    bs = bs_calculate();
    printf("Using automatic bs size (%llib)...\n", bs);
  }
  else if (bs <= 0)
    {
      fprintf(stderr, "Invalid bs size %s\n", bs_str);
      exit(1);
    }

  // Check if bs is smaller than filesize
  if(bs > size){
    fprintf(stderr, "Error: bs (%llii b) is greater than file size (%s %s)!\n", bs, argv[2], argv[3]);
    return 2;
  }
  // Check if directory exists
  DIR *dir_check = opendir(path);
  if (dir_check)
    {
      // Directory exists
      fprintf (stderr, "Directory already exists...\n");
      closedir (dir_check);
    }
  else if (ENOENT == errno)
    {
      // Directory does not exist
      printf ("Directory does not exist! Do you want to create it? (y/n)\n");
      char check;
      // Input validation
      do
	{
	  check = tolower(getchar ());
	}
      while (strcmp (&check, "y") != 0 && strcmp (&check, "n") != 0);
      // Handle input
      // Create directory
      if (strcmp (&check, "y") == 0)
	{
	  rek_mkdir (path);
	  // Check if directory was created succesfuly
	  printf ("Directory created\n");
	}
      else
	{
	  printf ("Process interrupted by user\n");
	  return 0;
	}
    }
  else
    {
      //opendir() failed for some other reason
      fprintf (stderr, "Directory check failed! Do i have sufficient permission?\n");
      return 2;
    }

// Check if file exists
// Prepare filename and path
  char* tmp = concat(path, "/");
  char* fname = concat(tmp, name);
// Open file
  FILE *of;
  if (access (fname, F_OK) != -1)
    {
      fprintf (stderr, "Unable to create file! File already exists!\n");
      return 2;
    }
  else
    {
      of = fopen (fname, "wb");
    }
// Check if file was created succesfuly
  if (of == NULL)
    {
      fprintf (stderr, "Error! Unable to create file!\n");
      return 2;
    }
// Time operation
  clock_t begin = clock();
// Fill file with 0
  // Calculate number of iterations
  long long iterations = size / bs;
  // Prepare single block
  char block[bs + 1];
  for (int i = 0; i < bs; i++)
    {
      block[i] = '0';
    }
  block[bs] = '\0';

  // Write file
  for (int i = 0; i < iterations; i++)
    {
      int tmp_check = fputs(block, of);
      if (tmp_check == EOF)
	{
	  fprintf (stderr, "Error while writing file! Operation aborted!\n");
	  return 0;
	}
    }
  // Done writing
  clock_t end = clock();
  double time_spent = (double)(end - begin);
  fclose (of);
  // Free heap memory
  free(tmp);
  free(fname);
  // Done
  printf("All done!\n");
  float mod = 1.00;
  char* time_unit = "seconds";
  if (time_spent / CLOCKS_PER_SEC <= 0.01)
  {
    mod = 1000.00;
    time_unit = "ms";
  }
  printf("------------------------------------------------\n%f %s written\nsystem time: %f clocks\nreal time: %f %s\naverage writing speed: %fmb/s\n------------------------------------------------\n", (double)(bs * iterations) / unit, unit_str, time_spent, (time_spent / CLOCKS_PER_SEC) * mod, time_unit, (double)((bs * iterations)/(1048576))/(time_spent / CLOCKS_PER_SEC));
  return 0;
}

// Create Directory Curtesy of Jens Harms @ StackOverflow (https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c)
int rek_mkdir (char *path)
{
  char *sep = strrchr (path, '/');
  if (sep != NULL)
    {
      *sep = 0;
      rek_mkdir (path);
      *sep = '/';
    }
  if (mkdir (path, 0777) && errno != EEXIST)
    {
      printf("Error while trying to create '%s'! Do i have sufficient permission?\n Is a '.' missing at the beginning of [path]?\n Is '~' at the beginning of [path]?\n", path);
      exit(1);
    }
  return 0;
}

// Handle command line arguments
void arg_process(int argc, char* argv[])
{
  // Check argv
  for (int i = 1; i < argc; i++) {
    // Path ("path=" must be a substring of argv[i], input_args[PATH_INDEX] must still be empty and length of argv[i] must be > PATH_LENGTH)
    if (strstr(argv[i], PATH_ARG) != NULL && input_args[PATH_INDEX] == NULL && strlen(argv[i]) > PATH_LENGTH) {
      // store value at specific index
      input_args[PATH_INDEX] = argv[i] + PATH_LENGTH;
    }
    // Size
    else if (strstr(argv[i], SIZE_ARG) != NULL && input_args[SIZE_INDEX] == NULL && strlen(argv[i]) > SIZE_LENGTH && isNumeric(argv[i] + SIZE_LENGTH)) {
      // store value at specific index
      input_args[SIZE_INDEX] = argv[i] + SIZE_LENGTH;
    }
    // Unit
    else if (strstr(argv[i], UNIT_ARG) != NULL && input_args[UNIT_INDEX] == NULL && strlen(argv[i]) > UNIT_LENGTH) {
      // store value at specific index
      input_args[UNIT_INDEX] = argv[i] + UNIT_LENGTH;
    }
    // Bs
    else if (strstr(argv[i], BS_ARG) != NULL && input_args[BS_INDEX] == NULL && strlen(argv[i]) > BS_LENGTH && (isNumeric(argv[i] + BS_LENGTH) || strcmp("auto", argv[i] + BS_LENGTH) == 0)) {
      // store value at specific index
      input_args[BS_INDEX] = argv[i] + BS_LENGTH;
    }
    // Name
    else if (strstr(argv[i], NAME_ARG) != NULL && input_args[NAME_INDEX] == NULL && strlen(argv[i]) > NAME_LENGTH) {
      // store value at specific index
      input_args[NAME_INDEX] = argv[i] + NAME_LENGTH;
    }
    else if (strcmp(argv[i], HELP) == 0) {
      printf("HALP\n");
      exit(0);
    }
    else if(strcmp(argv[i], LICENSE) == 0){
      printf("This program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program. If not, see <http://www.gnu.org/licenses/>.\n");
      exit(0);
    }
    else if (strcmp(argv[i], BS_STAT) == 0) {
      printf("Block size for system I/O is %llib\n", bs_calculate());
      exit(0);
    }
  }
}


// Calculate block size for system I/O
long long bs_calculate (void)
{
  struct stat stats;

  if (!stat ("/", &stats))
    {
      return stats.st_blksize;
    }
  else
    {
      fprintf (stderr, "Error while determining automatic bs!");
      exit(3);
    }
}


// Check if string is numeric
bool isNumeric (char *str)
{
  int length = strlen (str);
  for (int i = 0; i < length; i++)
    {
      if (!isdigit (str[i]))
	{
	  return false;
	}
    }
  return true;
}


// Concatenate 2 strings
char* concat(char* s1, char* s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // Check for errors
    if(result == NULL)
    {
      fprintf(stderr, "Fatal error calling malloc()! Operation aborted!");
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
