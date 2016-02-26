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
  char *lastLine, *nextLine, *from, *to, *flag, *originalLine;
  originalLine = getLine(stdin);
  nextLine = originalLine;
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
          nextLine = quitReplace(lastLine, from, to);
        }
      }
    }
    if (rLocation) {
      if (!gLocation || strlen(rLocation) < strlen(gLocation)) {
        if (!qLocation || strlen(rLocation) < strlen(qLocation)) {
          // printf("use -r\n");
          nextLine = rescanReplace(lastLine, from, to);
        }
      }
    }
    if (gLocation) {
      if (!rLocation || strlen(gLocation) < strlen(rLocation)) {
        if (!qLocation || strlen(gLocation) < strlen(qLocation)) {
          // printf("use -g\n");
          nextLine = globalReplace(lastLine, from, to);
        }
      }
    }
    if (!qLocation && !gLocation && !rLocation) {
      // printf("use -aaaaq\n");
      nextLine = quitReplace(lastLine, from, to);
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
      free(sLocation);
      free(sBuffer);
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
      free(fLocation);
      free(fBuffer);
    }

    // If matching = 1, we have succesfully matched a FROM
      // We can now copy a new array, but instead of the substring starting at the first int tracker, we replace it with our TO argument
    if (strcmp(lastLine, nextLine) && strrchr(flag, 'S')) {
      i = successNext * 3;
      // printf("This was a success and we go to %d\n", i);
    }
    else if (!strcmp(lastLine, nextLine) && strrchr(flag, 'F')) {
      i = failureNext * 3;
      // printf("This was a failure and we go to %d\n", i);
    }
    free(from);
    free(to);
    free(flag);
    // free(lastLine);
  }

  printf("%s", nextLine);
  free(originalLine);
  free(nextLine);
  return 0;
}

// Quit Replace
char *quitReplace(char *line, char *from, char *to) {
  // Leftmost occurence of string FROM is replaced by string TO
  char *buffer, *ch, *tmpTo, *bufferTo, *replacedFrom;
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = (lineLength + toLength + fromLength);
  int length;

  if(!(ch = StrStr(line, from)))
    return line;

  buffer = malloc(10 * newLineLength * sizeof(char));
  buffer[0] = '\0';
  length = (int)(ch - line);
  strncat(buffer, line, length);
  buffer[length] = '\0';
  replacedFrom = malloc(newLineLength * sizeof(char));
  strncpy(replacedFrom, ch, fromLength);
  replacedFrom[fromLength] = '\0';
  if(strchr(to, '^')) {
    tmpTo = strdup(to);
    bufferTo = globalReplace(tmpTo, "^", replacedFrom);
    free(tmpTo);
    strcat(buffer, bufferTo);
    strcat(buffer, &line[length + fromLength]);
    free(replacedFrom);
    return buffer;
  }
  else {
    strcat(buffer, to);
    strcat(buffer, &line[length + fromLength]);
    free(replacedFrom);
    return buffer;
  }
}

// Global Replace
char *globalReplace(char *line, char *from, char *to) {
  // Every occurence of FROM in the line is replaced by TO
  // Returns the new string
  char *buffer, *ch, *replacedFrom, *tmpTo, *bufferTo, *tmpLine = strdup(line);
  int lineLength = strlen(line), fromLength = strlen(from), toLength = strlen(to);
  int newLineLength = lineLength + toLength + fromLength;
  int size, end = 0, everIn = 0;

  buffer = malloc(10 * newLineLength * sizeof(char));
  buffer[0] = '\0';
  replacedFrom = malloc(newLineLength * sizeof(char));
  replacedFrom[fromLength] = '\0';
  while (!end) {
    if(!(ch = StrStr(tmpLine, from))) {
      if (everIn) {
        strcat(buffer, &line[tmpLine - line]);
        end = 0;
        free(replacedFrom);
        return buffer;
      }
      else
        return line;
    }
    everIn = 1;
    strncpy(replacedFrom, ch, fromLength);
    replacedFrom[fromLength] = '\0';
    if(strchr(to, '^')) {
      tmpTo = strdup(to);
      bufferTo = globalReplace(tmpTo, "^", replacedFrom);
      free(tmpTo);
      size = (int)(ch - tmpLine);
      strncat(buffer, tmpLine, size);
      strcat(buffer, bufferTo);
      tmpLine = &tmpLine[size + fromLength];
    }
    else {
      size = (int)(ch - tmpLine);
      strncat(buffer, tmpLine, size);
      strcat(buffer, to);
      tmpLine = &tmpLine[size + fromLength];
    }
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

  buffer = malloc(2*newLineLength * sizeof(char));
  buffer[0] = '\0';
  tmp = malloc(2*newLineLength * sizeof(char));
  tmp[0] = '\0';
  int size;
  while (!end) {
    if(!(ch = StrStr(tmpLine, from))) {
      if (everIn) {
        strcat(buffer, &line[tmpLine - line]);
        end = 0;
        free(tmp);
        free(tmpLine);
        return buffer;
      }
      else {
        free(tmp);
        free(tmpLine);
        return line;
      }
    }
    free(tmp);
    everIn = 1;
    tmp = strdup(to);
    tmp = realloc(tmp, 2*newLineLength * sizeof(char));
    size = (int)(ch - tmpLine);
    strncat(buffer, tmpLine, size);
    tmp = strdup(strcat(tmp, &tmpLine[size + fromLength]));
    free(tmpLine);
    tmpLine = strdup(tmp);
  }
  free(tmp);
  free(tmpLine);
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
