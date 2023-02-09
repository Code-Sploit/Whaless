#include <openings.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/*
* Rip performance (TODO: Improve this)
*/

static struct __opening_entry *__items = NULL;
static const unsigned int ITEMS_MAX = INT32_MAX;
static unsigned int __items_count = 0;

static unsigned long ntohll(unsigned long __n)
{
    unsigned char *__np = (unsigned char *) &__n;

    return ((uint64_t) __np[0] << 56) | ((uint64_t) __np[1] << 48) | ((uint64_t) __np[2] << 40) | ((uint64_t) __np[3] << 32) |
           ((uint64_t) __np[4] << 24) | ((uint64_t) __np[5] << 16) | ((uint64_t) __np[6] << 8)  | (uint64_t)  __np[7];
}

static unsigned short ntohs(unsigned short __n)
{
    unsigned char *__np = (unsigned char *) &__n;

    return ((unsigned short) __np[0] << 8 | (unsigned short) __np[1]);
}

static struct __opening_entry *openings_search_binary(unsigned long __hash, unsigned int __low, unsigned int __high)
{
    assert(__high < __items_count);
    assert(2 * (unsigned int) ITEMS_MAX - 2 <= UINT32_MAX);

    while (__low <= __high)
    {
        unsigned int __mid = (__high + __low) / 2;

        if (__items[__mid].__hash == __hash)
        {
            return &__items[__mid];
        }
        else if (__items[__mid].__hash < __hash)
        {
            __low = __mid + 1;
        }
        else
        {
            __high = __mid - 1;
        }
    }

    return NULL;
}

struct __opening_entry *openings_find(ZobristHash __hash)
{
    if (__items_count == 0)
    {
        return NULL;
    }
    else
    {
        return openings_search_binary(__hash, 0, __items_count - 1);
    }
}

void openings_initialize(void)
{
    const size_t ENTRY_SIZE = 16;
    const unsigned short MOVE_VALUE_MASK = 0x7;

    const unsigned short TO_FILE_SHIFT = 0;
    const unsigned short TO_ROW_SHIFT  = 3;
    const unsigned short FROM_FILE_SHIFT = 6;
    const unsigned short FROM_ROW_SHIFT = 9;
    const unsigned short PROMO_PIECE_SHIFT = 12;

    const int PROMO_PIECE_NONE = 0;
    const int PROMO_PIECE_QUEEN = 4;

    size_t __items_size = 100;

    __items = malloc(sizeof(struct __opening_entry) * __items_size);

    FILE *__book = fopen("data/openings.bin", "rb");

    if (!__book)
    {
        printf("[OPENINGS]: Error while initializing opening book!\n");

        exit(EXIT_FAILURE);
    }

    char __entry[ENTRY_SIZE];

    while (fread(__entry, ENTRY_SIZE, 1, __book) == 1)
    {
        if (__items_count == ITEMS_MAX)
        {
            break;
        }

        if (__items_count == __items_size)
        {
            __items = realloc(__items, sizeof(struct __opening_entry) * __items_size * 2);
            __items_size = (__items_size * 2);
        }

        unsigned long __hash = ntohll(*(unsigned long *) __entry);
        unsigned short __move = ntohs(*(unsigned short *) (__entry + 8));

        int __to_file = (__move >> TO_FILE_SHIFT) & MOVE_VALUE_MASK;
        int __to_row = (__move >> TO_ROW_SHIFT) & MOVE_VALUE_MASK;
        int __from_file = (__move >> FROM_FILE_SHIFT) & MOVE_VALUE_MASK;
        int __from_row = (__move >> FROM_ROW_SHIFT) & MOVE_VALUE_MASK;
        int __promo_piece = (__move >> PROMO_PIECE_SHIFT) & MOVE_VALUE_MASK;

        if (__promo_piece != PROMO_PIECE_NONE && __promo_piece != PROMO_PIECE_QUEEN)
        {
            continue;
        }

        struct __board_pos __from = BoardPos(__from_file, 7 - __from_row);
        struct __board_pos __to   = BoardPos(__to_file, 7 - __to_row);
        struct __move __amove = {__from, __to};

        if (__items_count != 0 && __items[__items_count - 1].__hash == __hash)
        {
            struct __opening_entry *__item = &__items[__items_count - 1];

            if (__item->__moves_count == UINT8_MAX)
            {
                continue;
            }

            __item->__moves_count = (__item->__moves_count + 1);
            __item->__moves = realloc(__item->__moves, sizeof(struct __move) * __item->__moves_count);
            __item->__moves[__item->__moves_count - 1] = __amove;
        }
        else
        {
            struct __opening_entry __item = {.__hash = __hash, .__moves = NULL, .__moves_count = 1};

            __item.__moves = malloc(sizeof(struct __move));
            __item.__moves[0] = __amove;
            __items[__items_count++] = __item;
        }
    }

    fclose(__book);

    if (__items_count == 0)
    {
        printf("[OPENINGS]: Error invalid opening book file!\n");

        exit(EXIT_FAILURE);
    }

    __items = realloc(__items, sizeof(struct __opening_entry) * __items_count);
}

void openings_deinitialize(void)
{
    for (unsigned int i = 0; i < __items_count; i++)
    {
        free(__items[i].__moves);
    }

    free(__items);

    __items = NULL;
    __items_count = 0;
}