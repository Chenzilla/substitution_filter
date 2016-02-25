#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "./getLine.h"

char *quitReplace(char *line, char *from, char *to);
char *rescanReplace(char *line, char *from, char *to);
char *globalReplace(char *line, char *from, char *to);
char* StrStr(char *str, char *target);

int main(int argc, char *argv[]) {
  // Use getLine to load up the input
  char *lastLine, *nextLine, *from, *to, *flag;
  // int length;
  lastLine = nextLine = getLine(stdin);
  // length = strlen(line);
  char *gLocation, *qLocation, *rLocation, *sLocation, *fLocation, *sBuffer, *fBuffer;
  // For each argument set of 3
  int inFrom, inTo, inFlag;
  inTo = inFlag = 0;
  inFrom = 1;
  for (int i = 1; i < argc; i++) {
    lastLine = strdup(nextLine);
    // Load current argument set
    // Process the FROM
    if (inFrom) {
      from = strdup(argv[i]);
      inTo = 1;
      inFrom = 0;
      continue;
    }
    // Process the TO
    else if (inTo) {
      to = strdup(argv[i]);
      inFlag = 1;
      inTo = 0;
      continue;
    }
    // Process the flag
    else if (inFlag) {
      flag = strdup(argv[i]);
      inFrom = 1;
      inFlag = 0;
    }
    // Determine what kind of matching to use.
    // printf("This is the old line: %s", lastLine);
    // printf("We want to replace %s with %s\n", from, to);
    gLocation = strrchr(flag, 'g');
    qLocation = strrchr(flag, 'q');
    rLocation = strrchr(flag, 'r');
    if (qLocation) {
      if (!gLocation || strlen(qLocation) < strlen(gLocation)) {
        if (!rLocation || strlen(qLocation) < strlen(rLocation)) {
          // printf("use -qaaffa\n");
          nextLine = quitReplace(nextLine, from, to);
        }
      }
    }
    if (rLocation) {
      if (!gLocation || strlen(rLocation) < strlen(gLocation)) {
        if (!qLocation || strlen(rLocation) < strlen(qLocation)) {
          // printf("use -r\n");
          nextLine = rescanReplace(nextLine, from, to);
        }
      }
    }
    if (gLocation) {
      if (!rLocation || strlen(gLocation) < strlen(rLocation)) {
        if (!qLocation || strlen(gLocation) < strlen(qLocation)) {
          // printf("use -g\n");
          nextLine = globalReplace(nextLine, from, to);
        }
      }
    }
    if (!qLocation && !gLocation && !rLocation) {
      // printf("use -aaaaq\n");
      nextLine = quitReplace(nextLine, from, to);
    }
    // Process F and S flags
    int failureNext = i, successNext = i;
    if (strrchr(flag, 'S')) {
      sLocation = strdup(strrchr(flag, 'S'));
      sBuffer = strdup(sLocation);
      if (strlen(sLocation) == 1)
        successNext = 0;
      else {
        memmove(sBuffer, sLocation+1, strlen(sLocation));
        successNext = (int)strtol(sBuffer, NULL, 10);
      }
    }
    if (strrchr(flag, 'F')) {
      fLocation = strdup(strrchr(flag, 'F'));
      fBuffer = strdup(fLocation);
      if (strlen(fLocation) == 1)
        failureNext = 0;
      else {
        memmove(fBuffer, fLocation+1, strlen(fLocation));
        failureNext = (int)strtol(fBuffer, NULL, 10);
      }
    }

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
    // printf("This is the new line: %s\n", nextLine);
    if (strcmp(lastLine, nextLine) && strrchr(flag, 'S')) {
      i = successNext * 3;
      // printf("This was a success and we go to %d\n", i);
    }
    else if (!strcmp(lastLine, nextLine) && strrchr(flag, 'F')) {
      i = failureNext * 3;
      // printf("This was a failure and we go to %d\n", i);
    }
  }
  printf("%s", nextLine);
  // free(lastLine);
  // free(nextLine);
  // free(from);
  // free(to);
  // free(flag);
  // free(gLocation);
  // free(qLocation);
  // free(rLocation);
  // free(sLocation);
  // free(fLocation);
  // free(sBuffer);
  // free(fBuffer);
  return 0;
}

// Quit Replace
char *quitReplace(char *line, char *from, char *to) {
  // Leftmost occurence of string FROM is replaced by string TO
  char *buffer, *ch;
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength - fromLength + toLength + 1;

  if(!(ch = StrStr(line, from)))
    return line;

  buffer = malloc(newLineLength * sizeof(char));
  strncat(buffer, line, ch - line);
  *buffer++ = '\0';
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
  int end = 0, everIn = 0;

  buffer = malloc(newLineLength * sizeof(char));
  while (!end) {
    if(!(ch = StrStr(tmpLine, from))) {
      if (everIn) {
        strcat(buffer, &line[tmpLine - line]);
        end = 0;
        return buffer;
      }
      else
        return line;
    }
    everIn = 1;
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
  int newLineLength = (lineLength - fromLength + toLength + 1)*2;
  int end = 0, everIn = 0;  // newLine = quitReplace(oldLine, from, to);

  buffer = malloc(newLineLength * sizeof(char));
  tmp = malloc(newLineLength * sizeof(char));
  int length;
  while (!end) {
    if(!(ch = StrStr(tmpLine, from))) {
      if (everIn) {
        strcat(buffer, &line[tmpLine - line]);
        free(tmp);
        end = 0;
        return buffer;
      }
      else
        return line;
    }
    everIn = 1;
    tmp = strdup(to);
    length = (int)(ch - tmpLine);
    strncat(buffer, tmpLine, length);
    tmp = strcat(tmp, &tmpLine[length + fromLength]);
    tmpLine = tmp;
  }
  return tmpLine;
}

char* StrStr(char *str, char *target) {
  int escaped = 0;
  if (!*target)
    return NULL;
  char *tmpStr = str;
  while (*tmpStr) {
    char *strStart = tmpStr, *tmpTarget = target;
    if (*tmpTarget == '@' && !escaped) {
      escaped = 1;
      tmpTarget++;
    }
    while (*tmpStr && *tmpTarget && *tmpStr != '\n' && (*tmpStr == *tmpTarget || (*tmpTarget == '.' && !escaped))) {
      // if (*tmpStr == '\n');
      //   printf("YESSS");
      if (*tmpTarget == '@' && !escaped) {
        escaped = 1;
        tmpTarget++;
      }
      else {
        escaped = 0;
        tmpStr++;
        tmpTarget++;
      }
    }
    if (!*tmpTarget)
      return strStart;
    tmpStr = strStart + 1;
  }
  return NULL;
}
