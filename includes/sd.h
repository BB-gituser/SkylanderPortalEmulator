#ifndef SD_H
#define SD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tusb.h"
#include "bsp/board.h"

#include "../includes/usb_descriptors.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "ff.h"

// INIT SD
#define MAX_ENTRIES        400
#define MAX_DEPTH          8
#define MAX_PATH           256
#define NAME_STORE         40

typedef struct {
    uint8_t depth;
    bool    is_dir;
    char    name[NAME_STORE + 1];
} entry_t;

static entry_t entries[MAX_ENTRIES];
static size_t n_entries = 0;
static bool truncated = false;

static char path[MAX_PATH] = "0:";
static DIR dirs[MAX_DEPTH];
static FILINFO finfos[MAX_DEPTH];
static FATFS fs;

// pour l'arborescence

typedef struct Folder Folder;
typedef struct File File;

// sd.h

typedef struct File {
    int FileIndex;
    char filename[64];
    char path[256];
    Folder *parent;
} File;

typedef struct Folder Folder;

struct Folder {
    char foldername[64];
    char path[256];
    Folder *parent;
    int file_count;
    int folder_count;
    Folder *folderlist;
    File *filelist;
};

Folder* scan_sd_root();
Folder* scan_directory(char path[256], Folder* parent_folder);

#endif