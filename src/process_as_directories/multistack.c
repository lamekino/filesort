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
    const size_t cur_cap = *capacity;
    const size_t new_len = *length + 1;
    const size_t new_cap = 2 * cur_cap + 1;
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


static struct stack *
push_stack(struct multistack *ms, char *name, char **members,
        size_t stack_cap) {
    const size_t idx = ms->len - 1;
    struct stack *base =
        push(ms->base, &ms->len, &ms->capacity, SIZE_AT(ms->base));

    if (!base) {
        return NULL;
    }

    ms->base = base;
    ms->base[idx] =
        (struct stack) {
            .name = name,
            .members = members,
            .capacity = stack_cap,
            .count = 0
        };

    return &ms->base[idx];
}

struct stack *
push_name(struct multistack *ms, char *name) {
    char **members = calloc(INITIAL_STACK_CAPACITY, sizeof(char **));
    char *dup = strdup(name);
    struct stack *ys = push_stack(ms, dup, members, INITIAL_STACK_CAPACITY);

    if (!ys || !members || !dup) {
        free(members);
        free(dup);
        return NULL;
    }

    return ys;
}

static char **
push_string(struct stack *xs, char *name) {
    const size_t idx = xs->count - 1;
    char **members =
        push(xs->members, &xs->count, &xs->capacity, SIZE_AT(xs->members));

    if (!members) {
        return NULL;
    }

    xs->members = members;
    xs->members[idx] = name;
    return &xs->members[idx];
}

char **
push_member(struct multistack *ms, char *member_name) {
    struct stack *cur = &ms->base[ms->len - 1];
    char *dup = strdup(member_name);
    char **ys = push_string(cur, dup);

    if (!ys || !dup) {
        free(dup);
        return NULL;
    }

    return ys;
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
        free(cur->name);
        free(cur);
    }
}
