#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "getLine.h"

int main(int argc, char *argv[]) {
// Use getLine to load up the input

// For each argument set of 3
  // Load current argument set
  // Process the flag

  // Process the FROM
    // This means getting length of FROM
  // Process the TO
    // This means getting the length of TO

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

