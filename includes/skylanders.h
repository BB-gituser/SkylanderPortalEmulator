#ifndef SKYLANDERS_H
#define SKYLANDERS_H

#include "ff.h"

typedef enum {
    SKY_ELEM_NONE = 0,
    SKY_ELEM_UNKNOWN,
    SKY_ELEM_AIR,
    SKY_ELEM_EARTH,
    SKY_ELEM_FIRE,
    SKY_ELEM_WATER,
    SKY_ELEM_MAGIC,
    SKY_ELEM_TECH,
    SKY_ELEM_LIFE,
    SKY_ELEM_UNDEAD,
    SKY_ELEM_LIGHT,
    SKY_ELEM_DARK,
    SKY_ELEM_KAOS,
    SKY_ELEM_COUNT
} skylander_element_t;

typedef enum {
    SPYRO_S_ADVENTURE = 0,
    GIANTS,
    SWAP_FORCE,
    TRAP_TEAM,
    SUPERCHARGERS,
    IMAGINATOR,
    OTHER_GAME
} skylander_game_t;

typedef struct {
    char name[64];
    uint16_t id;
    uint16_t variant;
    skylander_element_t element;
    skylander_game_t game;
} skylander_t;

extern skylander_t skylander_db[703];

skylander_t *get_skylander_data(uint16_t id, uint16_t variant);

#define SKYLANDER_FILE_SIZE 1024

typedef struct {
    uint8_t raw_data[SKYLANDER_FILE_SIZE];
    uint32_t uid;
    uint16_t character_id;
    uint16_t variant_id;
} SkylanderDump;

FRESULT read_skylander_bin(FIL *fil, SkylanderDump* dump);

#endif