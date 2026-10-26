#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#include "ff.h"
#include "../includes/skylanders.h"
#include "../includes/sd.h"

typedef enum {
    EMPTY,
    JUST_ADDED,
    PRESENT,
    JUST_REMOVED
} SLOT_STAT;

typedef struct {
    int FileIndex; // init to -1
    SLOT_STAT statut; // init to empty
    FIL *fil; //init to NULL
    skylander_t *skylander;
} Slot;

uint32_t calculate_sens_bitmask();

void unload_slot(int index);

int load_slot(File *file, int *index);

int get_nloaded_slots();

int get_loaded_slot_from_index_loaded(int index);

Slot *get_slot_from_index(char index);

int File_is_used_by_index(int FileIndex);

uint32_t cur_time();


#define MSG_SIZE 32
void create_status(uint8_t out[MSG_SIZE]);

void send_status();

#endif