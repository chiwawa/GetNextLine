#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "get_next_line.h"

static LineManagement	__needFullLine = FULL_LINE;
static ssize_t		nbRead;
static size_t		currentByte;
static char		buffer[BUFF_SIZE + 1];

void
set_get_next_line_full_or_truncated(LineManagement v) {
  __needFullLine = v;
}

static
ssize_t
__get_end_of_line(char *s) {
  int i = 0;

  while (s[i] != '\n' && s[i] != 0) ++i;
  return i;
}

static
int
__strlen(char *s) {
  if (s) return strlen(s);
  return 0;
}

static
char*
__strncat(char *s, char *s2, int l) {
  char *toFree = s;

  if ((s = realloc(s, l + __strlen(s) + 2)) == 0) {
    free(toFree);
    return 0;
  }
  if (toFree == 0) memset(s, 0, l + __strlen(s) + 2);
  return strncat(s, s2, l);
}

void
init_get_next_line() {
  nbRead = 0;
  currentByte = 0;
}

char *
get_next_line(const int fd) {
  char*			res = 0;
  ssize_t		bytesTilEndOfLine;
  int			shouldContinue = 1;

  while (shouldContinue) {
    shouldContinue = 0;
    if (nbRead <= currentByte) {
      if ((nbRead = read(fd, buffer, BUFF_SIZE)) <= 0) return 0;
      buffer[nbRead] = 0;
      currentByte = 0;
    }
    bytesTilEndOfLine = __get_end_of_line(buffer + currentByte);
    if (__needFullLine == FULL_LINE && buffer[bytesTilEndOfLine + currentByte] != '\n') shouldContinue = 1;
    if ((res = __strncat(res, buffer + currentByte, bytesTilEndOfLine)) == 0) return 0;
    currentByte += bytesTilEndOfLine + 1;
  }
  return res;
}
