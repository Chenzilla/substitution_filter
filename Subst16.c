#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "./getLine.h"

// char *getLine(FILE *fp);
char *quitReplace(char *line, char *from, char *to);
char *rescanReplace(char *line, char *from, char *to);
char *globalReplace(char *line, char *from, char *to);


int main(int argc, char *argv[]) {
  // Use getLine to load up the input
  char *line, *lastLine, *from, *to, *flag;
  // int length;
  line = lastLine = getLine(stdin);
  // length = strlen(line);
  char *gLocation, *qLocation, *rLocation, sLocation, fLocation;
  long ret;
  char *ptr;
  // For each argument set of 3
  int inFrom, inTo, inFlag, flagLength;
  inTo = inFlag = 0;
  inFrom = 1;
  for (int i = 1; i < argc; i++) {
    // Load current argument set
    // Process the FROM
    if (inFrom) {
      from = strdup(argv[i]);
      inTo = 1;
      inFrom = 0;
      printf("%s\n", from);
      continue;
    }
    // Process the TO
    else if (inTo) {
      to = strdup(argv[i]);
      inFlag = 1;
      inTo = 0;
      printf("%s\n", to);
      continue;
    }
    // Process the flag
    else if (inFlag) {
      flag = strdup(argv[i]);
      inFrom = 1;
      inFlag = 0;
      printf("%s\n", flag);
    }
    // Determine what kind of matching to use.
    flagLength = strlen(flag);
    printf("The length of the flag is %d\n", flagLength);
    gLocation = strrchr(flag, 'g');
    qLocation = strrchr(flag, 'q');
    rLocation = strrchr(flag, 'r');
    if (!qLocation && !gLocation && !rLocation) {
      printf("use -q\n");
      lastLine = quitReplace(lastLine, from, to);
    }
    if (qLocation) {
      if (!gLocation || strlen(qLocation) < strlen(gLocation)) {
        if (!rLocation || strlen(qLocation) < strlen(rLocation)) {
          printf("use -q\n");
          lastLine = quitReplace(lastLine, from, to);
        }
      }
    }
    if (rLocation) {
      if (!gLocation || strlen(rLocation) < strlen(gLocation)) {
        if (!qLocation || strlen(rLocation) < strlen(qLocation)) {
          printf("use -r\n");
          lastLine = rescanReplace(lastLine, from, to);
        }
      }
    }
    if (gLocation) {
      if (!rLocation || strlen(gLocation) < strlen(rLocation)) {
        if (!qLocation || strlen(gLocation) < strlen(qLocation)) {
          printf("use -g\n");
          lastLine = globalReplace(lastLine, from, to);
        }
      }
    }

    // Process F and S flags
    sLocation = strrchr(flag, 'S');
    fLocation = strrchr(flag, 'F');
    int failureNext, successNext;


    printf("Flag: %s\n", flag);
    ret = strtol("01d", &ptr, 10);
    printf("STRINGTOL: %ld\n", ret);
    // free(lastLine);
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
  printf("This is to: %s\n", to);

  return 0;
}

// Quit Replace
char *quitReplace(char *line, char *from, char *to) {
  // Leftmost occurence of string FROM is replaced by string TO
  char *buffer, *ch;
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength - fromLength + toLength + 1;

  if(!(ch = strstr(line, from)))
    return line;

  buffer = malloc(newLineLength * sizeof(char));
  strncat(buffer, line, ch - line);
  strcat(buffer, to);
  strcat(buffer, &line[ch - line + fromLength]);
  return buffer;
}

// Global Replace
char *globalReplace(char *line, char *from, char *to) {
  // Every occurence of FROM in the line is replaced by TO
  // Returns the new string
  char *buffer, *ch, *tmpLine = strdup(line);
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength - fromLength + toLength + 1;
  int end = 0;

  buffer = malloc(newLineLength * sizeof(char));
  while (!end) {
    if(!(ch = strstr(tmpLine, from))) {
      strcat(buffer, &line[tmpLine - line]);
      end = 0;
      return buffer;
    }
    strncat(buffer, tmpLine, ch - tmpLine);
    strcat(buffer, to);
    tmpLine = &tmpLine[ch - tmpLine + fromLength];
  }
  return buffer;
}

// Rescan Replace
char *rescanReplace(char *line, char *from, char *to) {
  // Leftmost occurence of string FROM is replaced by string TO repeatedly
  // Returns the new string
  char *buffer, *ch, *tmp, *tmpLine = strdup(line);
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength - fromLength + toLength + 1;
  int end = 0;  // newLine = quitReplace(oldLine, from, to);

  // while (strcmp(oldLine, newLine)){
  //   printf("This is the oldline %s", oldLine);
  //   printf("This is the newline %s", newLine);
  //   oldLine = newLine;
  //   newLine = quitReplace(oldLine, from, to);
  // }
  buffer = malloc(newLineLength * sizeof(char));
  tmp = malloc(newLineLength * sizeof(char));
  memcpy(tmp, to, toLength);
  while (end < 2) {
    if(!(ch = strstr(tmpLine, from))) {
      strcat(buffer, &line[tmpLine - line]);
      free(tmp);
      end = 0;
      return buffer;
    }
    printf("We want to go from %s to %s", from, to);
    printf("This is the line we are looking at: %s\n", tmpLine);
    printf("this is the characters that match and onwards %s\n", ch);
    strncat(buffer, tmpLine, ch - tmpLine);
    // strcat(buffer, to);
    printf("This is ampersand: %s\n", &tmpLine[ch - tmpLine + toLength]);
    strcat(tmp, &tmpLine[ch - tmpLine + toLength]);
    tmpLine = tmp;
    end++;
  }
  return tmpLine;
}
