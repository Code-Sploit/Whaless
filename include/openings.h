#ifndef OPENINGS_H
#define OPENINGS_H

#include <chess.h>
#include <zobrist.h>

struct __opening_entry
{
    zobrist_hash __hash;

    struct __move *__moves;

    unsigned char __moves_count;
};

void openings_initialize(void);
void openings_deinitialize(void);

struct __opening_entry *openings_find(zobrist_hash __hash);

#endif