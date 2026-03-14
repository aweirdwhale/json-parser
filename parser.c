#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct JsonValue JsonValue;

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonType;

struct JsonValue {
    JsonType type;

    union {
        double number;
        char *string;
        int boolean;

        struct {
            JsonValue **items;
            size_t length;
        } array;

        struct {
            char **keys;
            JsonValue **values;
            size_t length;
        } object;
    };
};

typedef struct {
    const char *input;
    size_t pos;
} JsonParser;


// fonctions utilitaires du parser :

/*
 * Reads the current character of Json input
 * @returns char
 *
 * ---
 * Lis le caractèresur lequel on se trouve.
 */
char current(JsonParser *p){
    return p->input[p->pos];
}


/*
 * Consumes the next character of Json input
 * @returns null
 *
 * ---
 * Avance d'un caractère sur le Json à parser.
 */
void consume(JsonParser *p) {
    p->pos++;
}



// parse strings :
JsonValue* parse_string(JsonParser *p) {
    consume(p);
    char *buffer = (char*)malloc(sizeof(char));
    size_t length = 0;
    while(current(p) != '"') {
        char c = current(p);
        // printf("%c ", c);
        // length +2 pc <stdio.h>q on ajoute le caractere et \0
        buffer = realloc(buffer, length+2);

        buffer[length] = c;
        length++;

        consume(p);
    }

    buffer[length] = '\0';
    consume(p);

    JsonValue *value = (JsonValue*)malloc(sizeof(JsonValue));
    value->type = JSON_STRING;
    value->string = strdup(buffer);

    free(buffer);
    return value;
}

// parse numbers :
JsonValue* parse_numbers(JsonParser *p) {
    size_t start = p->pos;

    if (current(p) == '-') consume(p);

    if (!isdigit(current(p))) return NULL;

    while (isdigit(current(p))) consume(p);

    if (current(p) == '.') {
        consume(p);
        if (!isdigit(current(p))) return NULL;
        while (isdigit(current(p))) consume(p);
    }

    if (current(p) == 'e' || current(p) == 'E') {
        consume(p);

        if (current(p) == '+' || current(p) == '-') consume(p);

        if (!isdigit(current(p))) return NULL;

        while (isdigit(current(p))) consume(p);
    }

    size_t len = p->pos - start;

    char *buf = malloc(len + 1);

    for (size_t i = 0; i < len; i++) {
        buf[i] = p->input[start + i];
    }

    buf[len] = '\0';

    double val = strtod(buf, NULL);
    free(buf);

    JsonValue *value = malloc(sizeof(JsonValue));
    value->type = JSON_NUMBER;
    value->number = val;

    return value;
}



// tests
int main() {
    JsonParser p = {
        .input = "-25e46",
        .pos = 0
    };

    JsonValue *v = parse_numbers(&p);
    printf("lu depuis json : %f\n", v->number);

    return EXIT_SUCCESS;
}
