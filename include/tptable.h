#ifndef TPTABLE_H
#define TPTABLE_H

#include <chess.h>
#include <zobrist.h>

enum __entry_type
{
    ENTRY_EXACT,
    ENTRY_UPPER,
    ENTRY_LOWER
};

struct __transposition_entry
{
    zobrist_hash __hash;

    struct __move __best_move;

    char __depth;

    int __value;

    enum __entry_type __type;
};

struct __transposition_entry tptable_get(zobrist_hash __hash);

void tptable_put(struct __transposition_entry __entry);
void tptable_clear(void);
void tptable_init(void);
void tptable_deinit(void);
void tptable_set_protected_hash(zobrist_hash __hash);

#endif