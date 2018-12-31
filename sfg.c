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


// Function placeholders
int rek_mkdir (char *path);
int bs_calculate (void);
bool isNumeric (char *str);
char* concat(char* s1, char* s2);

int main (int argc, char *argv[])
{
  // Check flags --> path, size, unit, (bs), (name)
  // Too few or many arguments
  if (argc != 2 && (argc < 4 || argc > 6))
    {
      fprintf (stderr, "Usage: sfg [path] [size] [unit] [bs] [filenamename]\n");
      return 1;
    }
  else if (argc == 2 && strcmp (argv[1], "--help") == 0)
    {
      printf ("Usage: sfg [path] [size] [unit] [bs] [filename] --> Writes an empty file named [name] of size [size][unit] in [path] using block size [bs]\n");
      printf("or sfg [path] [size] [unit] [bs] --> Writes an empty file named 'file' of size [size][unit] in [path] using block size [bs]\n");
      printf("or sfg [path] [size] [unit] --> Writes an empty file named 'file' of size [size][unit] in [path] using kernel default block size\n");
      printf("[size] must be a valid integer\n");
      printf("[unit] must be a valid data size unit (i.e. b, kb, mb, gb)\n");
      printf("[bs] must be a valid integer. [bs] is always expressed in bytes (b) and must be smaller than the total file size\n");
      printf("If a directory does not exist, sfg can create it (0755). If a file already exists, sfg will not overwrite it.\n");
      return 0;
    }
  // Define useful variables
  // OF path
  char *path = argv[1];
  // OF unit
  int unit;
  if (strcmp (argv[3], "gb") == 0)
    {
      unit = 1000000000;
    }
  else if (strcmp (argv[3], "mb") == 0)
    {
      unit = 1000000;
    }
  else if (strcmp (argv[3], "kb") == 0)
    {
      unit = 1000;
    }
  else if (strcmp (argv[3], "b") == 0)
    {
      unit = 1;
    }
  else
    {
      fprintf (stderr,
	       "Unknown unit! Please input 'gb', 'mb', 'kb' or 'b'\n");
      return 1;
    }
  // Define OF size
  int size;
  if (isNumeric(argv[2]))
  {
    size = atoi(argv[2]) * unit;
  }
  else
  {
    fprintf(stderr, "Please input a valid integer for size!");
  }
  // (Define OF name)
  char *name;
  if (argc == 6)
    {
      name = argv[5];
    }
  else
    {
    name = "file";
  }
  // Define block size
  int bs;
  if (argc >= 5 && isNumeric(argv[4]) && atoi(argv[4]) != 0)
    {
      bs = atoi(argv[4]);
    }
  else
    {
      bs = bs_calculate();
      printf("Using automatic bs size (%i b)...\n", bs);
    }
  // Check if bs is smaller than filesize
  if(bs > size){
    fprintf(stderr, "Error: bs (%i b) is greater than file size (%s %s)!\n", bs, argv[2], argv[3]);
    return 2;
  }
  // Check if directory exists
  DIR *dir_check = opendir (path);
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
	  check = tolower (getchar ());
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

// Fill file with 0
  // Calculate number of iterations
  int iterations = size / bs;
  // Prepare single block
  char* str="";
  char* zero ="0";

  for (int i = 0; i < bs; i++)
    {
      char* tmp_str = concat(str, zero);
      // Very important to avoid mem leaks
      if (i != 0){free(str);}
      str = tmp_str;
    }
  // Time operation
  clock_t begin = clock();
  // Write file
  for (int i = 0; i < iterations; i++)
    {
      int tmp_check = fputs(str, of);
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
  free(str);
  // Done
  printf ("All done!\n");
  float mod = 1.00;
  char* time_unit = "seconds";
  if (time_spent / CLOCKS_PER_SEC <= 0.01)
  {
    mod = 1000.00;
    time_unit = "ms";
  }
  printf("Wrote %i bytes in %f clocks or %f %s!\n", size, time_spent, (time_spent / CLOCKS_PER_SEC) * mod, time_unit);
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
  if (mkdir (path, 0755) && errno != EEXIST)
    {
      printf("Error while trying to create '%s'! Do i have sufficient permission?\n Is a '.' missing at the beginning of [path]?", path);
      exit(1);
    }
  return 0;
}

// Calculate Kernel defined block size
int bs_calculate (void)
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
