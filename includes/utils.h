#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#include "ff.h"

int fd_in_array(FIL *fd, FIL *array[], int len);

int get_fd_in_array(FIL *fd, FIL *array[], int len);

int add_fd_to_array(FIL *fd, FIL *array[], int len);

int remove_fd_from_array(FIL *fd, FIL *array[], int len);

char create_sense_bitmask(FIL *array[], int len);

#endif