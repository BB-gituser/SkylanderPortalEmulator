#ifndef CST_H
#define CST_H

#include <stdint.h>
#include <stdbool.h>

#include "ff.h"

#include "../includes/skylanders.h"
#include "../includes/sd.h"
#include "../includes/utils.h"


// define
#define MSG_SIZE 32
#define BLOCK_SIZE 16
#define MAX_SKYLANDER_COUNT 16 // nombre de slots virtuels

#define FPS 1000/24

// LCD TFT
#define SCREEN_W            160
#define SCREEN_H            128
#define CHAR_W              6
#define CHAR_H              8
#define COLS                (SCREEN_W / CHAR_W) * 7 / 8
#define HEADER_Y            1
#define LIST_Y              12
#define LIST_ROWS           ((SCREEN_H - LIST_Y) / CHAR_H)

#define SKY_Q               8
#define PORTAL_PUSH_MS      30

extern int to_send;

extern char sense_counter;

extern int slot_index; // index to new slot

extern Slot slots_list[MAX_SKYLANDER_COUNT];
extern FIL fil_list[MAX_SKYLANDER_COUNT];
extern Folder *root;

extern SLOT_STAT slots_states[MAX_SKYLANDER_COUNT];

#endif
