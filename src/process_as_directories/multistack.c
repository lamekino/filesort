#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "process_as_directories/multistack.h"


#define INITIAL_STACK_CAPACITY 15u
#define SIZE_AT(p) (sizeof(*(p)))

static void *
push(void *p, size_t *length, size_t *capacity, size_t elem_size) {
    size_t cur_cap = *capacity;
    size_t new_len = *length + 1;
    size_t new_cap = 2 * cur_cap + 1;
    void *ys = p;

    if (new_len > cur_cap) {
        ys = realloc(p,  new_cap * elem_size);
    }

    if (ys != NULL) {
        *capacity = new_cap;
        *length = new_len;
    }

    return ys;
}


static struct stack
init_stack(char *name, char **members, size_t capacity) {
    return (struct stack) {
        .name = name,
        .members = members,
        .capacity = capacity,
        .count = 0
    };
}

struct stack *
push_name(struct multistack *ms, char *name) {
    struct stack init;
    struct stack *base = ms->base;
    const size_t idx = ms->len;
    char **members = NULL;

    members = calloc(INITIAL_STACK_CAPACITY, sizeof(char **));
    base = push(ms->base, &ms->len, &ms->capacity, SIZE_AT(ms->base));
    if (!members || !base) {
        return NULL;
    }

    ms->base = base;
    ms->base[idx] = init_stack(name, members, INITIAL_STACK_CAPACITY);
    return &ms->base[idx];
}

char **
push_member(struct multistack *ms, char *member) {
    struct stack *cur = &ms->base[ms->len - 1];
    const size_t idx = cur->count;
    char *dup = strdup(member);
    char **members =
        push(cur->members, &cur->count, &cur->capacity, SIZE_AT(cur->members));

    if (!members) {
        return NULL;
    }

    cur->members = members;
    cur->members[idx] = strdup(member);
    return &cur->members[idx];
}

struct stack *
pop_name(struct multistack *ms) {
    if (ms->len) {
        ms->len--;
        return &ms->base[ms->len];
    }

    return NULL;
}

char *
pop_member(struct stack *xs) {
    if (xs->count) {
        xs->count--;
        return xs->members[xs->count];
    }

    return NULL;
}

bool
is_empty(const struct multistack *ms) {
    return ms->len == 0;
}

void
cleanup_multistack(struct multistack *ms) {
    while (ms->capacity--) {
        struct stack *cur = &ms->base[ms->capacity];

        while (cur->count--) {
            free(cur->members[cur->count]);
        }

        free(cur->members);
        free(cur);
    }
}
