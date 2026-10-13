#ifndef UTILS_H
#define UTILS_H

#include <libpq-fe.h>

void cleanBuffer(void);
int getInteger(void);
void printQueryResult(PGresult *res);
void getString(char *buffer, size_t size);

#endif
