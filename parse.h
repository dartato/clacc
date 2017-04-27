#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/xalloc.h"
#include "lib/hdict.h"
#include "clacc.h"

bool parse(char *path, clac_file *output);
/* Parses a .clac file at null terminated string starting at path,
 * and populates the fields of the clac_file at output.
 * returns true if success, false if fail.
 */

#endif /* _PARSE_H_ */
