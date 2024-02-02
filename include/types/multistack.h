#ifndef MULTISTACK_H
#define MULTISTACK_H
#include <stdbool.h>
#include <stddef.h>

struct stack {
    char *name;
    char **members;
    size_t count;
    size_t capacity;
};

struct multistack {
    struct stack *base;
    size_t len;
    size_t capacity;
};

struct stack *
push_name_init_size(struct multistack *ms, char *name, size_t size);

struct stack *
push_name(struct multistack *ms, char *name);

char **
push_member_addr(struct stack *xs, char *name);

char **
push_member(struct multistack *ms, char *member);

struct stack *
pop_name(struct multistack *ms);

char *
pop_member(struct stack *xs);

bool
is_empty(const struct multistack *ms);

void
cleanup_stack(struct stack *xs);

void
cleanup_multistack(struct multistack *xs);

#endif /* MULTISTACK_H */
