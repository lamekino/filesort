#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h> /* PATH_MAX */

#include "types/multistack.h"

#define ELEMSIZE(p) (sizeof(*(p)))
#define INITIAL_STACK_CAPACITY 15u

static size_t
next_capacity(size_t cap) {
    return 2 * cap + 1;
}

static size_t
capacity_update(size_t cur_cap, size_t len) {
    if (len <= cur_cap) return cur_cap;
    else return next_capacity(cur_cap);
}

static void *
prepush(void *p, size_t *length, size_t *capacity, size_t elem_size) {
    void *ys = p;
    const size_t cur_cap = *capacity;
    const size_t set_len = *length + 1;
    const size_t set_cap = capacity_update(cur_cap, set_len);

    if (set_cap > cur_cap) {
        ys = realloc(p, set_cap *  elem_size);
    }

    if (ys != NULL) {
        *length = set_len;
        *capacity = set_cap;
    }

    return ys;
}

static struct stack *
push_stack(struct multistack *ms, char *name, char **members, size_t cap) {
    const size_t idx = ms->len - 1;

    struct stack *base =
        prepush(ms->base, &ms->len, &ms->capacity, ELEMSIZE(ms->base));

    if (!base) {
        return NULL;
    }

    ms->base = base;
    ms->base[idx] =
        (struct stack) {
            .name = name,
            .members = members,
            .capacity = cap,
            .count = 0
        };

    return &ms->base[idx];
}

static char **
push_string(struct stack *xs, char *name) {
    const size_t idx = xs->count - 1;

    char **members =
        prepush(xs->members, &xs->count, &xs->capacity, ELEMSIZE(xs->members));

    if (!members) {
        return NULL;
    }

    xs->members = members;
    xs->members[idx] = name;
    return &xs->members[idx];
}

struct stack *
push_name(struct multistack *ms, char *name) {
    char *title = strndup(name, PATH_MAX);
    char **members = calloc(INITIAL_STACK_CAPACITY, sizeof(char **));
    struct stack *ys = push_stack(ms, title, members, INITIAL_STACK_CAPACITY);

    if (!ys || !members || !title) {
        free(members);
        free(title);
        return NULL;
    }

    return ys;
}

char **
push_member(struct multistack *ms, char *member_name) {
    struct stack *cur = &ms->base[ms->len - 1];
    char *dup = strndup(member_name, NEW_FILENAME_SIZE);
    char **ys = push_string(cur, dup);

    if (!ys || !dup) {
        free(dup);
        return NULL;
    }

    return ys;
}

struct stack *
pop_name(struct multistack *ms) {
    if (ms->len == 0) {
        return NULL;
    }

    ms->len--;
    return &ms->base[ms->len];
}

char *
pop_member(struct stack *xs) {
    if (xs->count == 0) {
        return NULL;
    }

    xs->count--;
    return xs->members[xs->count];
}

bool
is_empty(const struct multistack *ms) {
    return ms->len == 0;
}

void
cleanup_multistack(struct multistack *ms) {
    while (ms->capacity--) {
        struct stack *cur = &ms->base[ms->capacity];

        while (cur->capacity--) {
            free(cur->members[cur->capacity]);
        }

        free(cur->members);
        free(cur->name);
        free(cur);
    }
}
