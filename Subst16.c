#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

char *getLine(FILE *fp);
char *quitReplace(char *line, char *from, char *to);
char *rescanReplace(char *line, char *from, char *to);

int main(int argc, char *argv[]) {
// Use getLine to load up the input
char *line, *lastLine, *from, *to, *flag;
int length;
line = lastLine = getLine(stdin);
length = strlen(line);
printf("The length of %s is %d\n", line, length);

// For each argument set of 3
int inFrom, inTo, inFlag, flagLength;
char *lastFlag;
inTo = inFlag = 0;
inFrom = 1;
for (int i = 1; i < argc; i++) {
  // Load current argument set
  // Process the FROM
  if (inFrom) {
    from = argv[i];
    inTo = 1;
    inFrom = 0;
    printf("%s\n", from);
    continue;
  }
  // Process the TO
  else if (inTo) {
    to = argv[i];
    inFlag = 1;
    inTo = 0;
    printf("%s\n", to);
    continue;
  }
  // Process the flag
  else if (inFlag) {
    flag = argv[i];
    inFrom = 1;
    inFlag = 0;
    printf("%s\n", flag);
  }
  // Determine what kind of matching to use.
  flagLength = strlen(flag);
  printf("The length of the flag is %d\n", flagLength);

  // Default of Quit matching
  if (flagLength == 1 && !strcmp(flag, "-")) {
    printf("this is a -, use q\n");
  }

  // If there are only 2 chars in the flag, we match to existing flags
  else if (flagLength == 2) {
    if (!strcmp(flag, "-g"))
      printf("use -g\n");
    else if (!strcmp(flag, "-q"))
      printf("use -q\n");
    else if (!strcmp(flag, "-r"))
      printf("use -r\n");
    else if (!strcmp(flag, "-S") || !strcmp(flag, "-F"))
      printf("use -q\n");
    else {
      printf("Not a valid flag");
      return 1;
    }
  }

  // If multiple flags, figure out the last flag
  else {
    lastFlag = &flag[flagLength - 1];
    printf("This is the value of the last flag: ");
    printf("%s\n", lastFlag);
    // Check to see if the last flag matches g, q, r
    if (!strcmp(lastFlag, "g"))
      printf("use -g\n");
    else if (!strcmp(lastFlag, "q"))
      printf("use -q\n");
    else if (!strcmp(lastFlag, "r"))
      printf("use -r\n");
  }

  lastLine = rescanReplace(line, from, to);
  // Set state of matching to see if we are in the middle of matching a FROM in the input
  // Set int of current char we are looking at to 0
  // Set first int tracker to -1

  // For each character in the input line
    // If not in matching state, i.e. trying to match first char still:
      // See if the current char of input line matches first char of FROM line
      // If it does,
        // Set first int tracker to current index
        // Increment char count and enter matching
      // Else
        // Set first int tracker to -1
        // Set matching and current char to 0
        // Skip to next increment
    // Else if trying to match
      // See if current input line char matches the FROM char corresponding to current char
      // If it does
        // Increment current char count and go to next
      // Else
        // Set first int tracker to -1
        // Set matching and current char to 0
        // Skip to next increment

  // If matching = 1, we have succesfully matched a FROM
    // We can now copy a new array, but instead of the substring starting at the first int tracker, we replace it with our TO argument
  }
  printf("This is the last line: %s", lastLine);
  return 0;
}


// Quit Replace
char *quitReplace(char *line, char *from, char *to){
  // Leftmost occurence of string FROM is replaced by string TO
  char *buffer, *ch;
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength - fromLength + toLength + 1;

  if(!(ch = strstr(line, from))) {
    printf("No matches");
    return line;
  }

  buffer = malloc(newLineLength * sizeof(char));
  strncat(buffer, line, ch - line);
  strcat(buffer, to);
  strcat(buffer, &line[ch - line + fromLength]);
  return buffer;
}

// Global Replace
// Every occurence of FROM in the line is replaced by TO
// Returns the new string

// Rescan Replace
char *rescanReplace(char *line, char *from, char *to){
  // Leftmost occurence of string FROM is replaced by string TO repeatedly
  // Returns the new string
  char *oldLine = line, *newLine = line;
  newLine = quitReplace(oldLine, from, to);
  while (strcmp(oldLine, newLine)){
    oldLine = newLine;
    newLine = quitReplace(oldLine, from, to);
  }
  return newLine;
}

char *getLine(FILE *fp)
{
    char *line;                 // Line being read
    int size;                   // #chars allocated
    int c, i;

    size = sizeof(double);                      // Minimum allocation
    line = malloc (size);
    for (i = 0;  (c = getc(fp)) != EOF; )  {
  if (i == size-1) {
      size *= 2;                          // Double allocation
      line = realloc (line, size);
  }
  line[i++] = c;
  if (c == '\n')                          // Break on newline
      break;
    }

    if (c == EOF && i == 0)  {                  // Check for immediate EOF
  free (line);
  return NULL;
    }

    line[i++] = '\0';                           // Terminate line
    line = realloc (line, i);                   // Trim excess storage

    return (line);
}
