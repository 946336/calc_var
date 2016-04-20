#include "env.h"
#include "binding.h"
#include "value.h"
#include "ast.h"
#include "utility.h"
#include "subexp.h"

#include "tokenize.h"
#include "parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum VERBOSITY {QUIET, NORMAL, VERBOSE} VERBOSITY;
static VERBOSITY verbosity = NORMAL;

typedef enum ECHO {YES, NO} ECHO;
static ECHO echo = YES;

static const char *HELPME = "-q: Quiet - Suppress most output\n"
                            "-v: Verbose - Produce extra output\n"
                            "--no-echo: No echo - Don't echo parsed expression";
const char *INTERACTIVE_PROMPT = ">>> ";
const char *NONINTERACTIVE_PROMPT = "";

const char *PROMPT;

int main(int argc, char **argv)
{
    FILE *fp = stdin;
    PROMPT = INTERACTIVE_PROMPT;

    int i = 1;
    if (argc > 1) {
        for (; i < argc; ++i) {
            if (strcmp(argv[i], "-q") == 0) verbosity = QUIET;
            else if (strcmp(argv[i], "-v") == 0) verbosity = VERBOSE;
            else if (strcmp(argv[i], "--no-echo") == 0) echo = NO;
            else if (strcmp(argv[i], "-h") == 0) {
                fprintf(stdout, "%s\n", HELPME);
                exit(EXIT_SUCCESS);
            }
            else break;
        }
    }

    if (argv[i] != '\0') {
        fp = fopen(argv[i], "r");
        FILENAME = argv[i];
        PROMPT = NONINTERACTIVE_PROMPT;
        if (fp == NULL) {
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (++i < argc) {
        fprintf(stderr, "Ignoring options specified after filename: "
                        " The first is: [%s]\n", argv[i]);
    }

/****************************************************************************/

    Env e = Env_new();

    char *line = NULL;
    size_t size = 0;
    size_t len = 0;

    if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
    fflush(stdout);

    while ((len = my_getline(&line, &size, fp)) != (size_t) -1) {
        ++LINE_NUMBER;
        line[--len] = '\0';
        if (len == 0) {
            if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
            fflush(stdout);
            continue;
        }

        SubExp s = parse(line, e);
        AST_Node root = SubExp_toAST(s);
        AST_replace_vars(root, e);
    
        if ((verbosity != QUIET) && (root != NULL) && !AST_validate(root)) {
            if (verbosity == VERBOSE)
                fprintf(stderr, "%s [Line %d]: "
                                "Incomplete expression!\n",
                                FILENAME, LINE_NUMBER);
        }
    
        Type t = AST_typeof(root, e, (verbosity == QUIET) ? false : true);
        if ((t != NONE) && (t != INVALID)) {
            if ((echo == YES) &&(verbosity == NORMAL)) {
                if (root->v.type == OP) AST_print(root);
            } else if ((echo == YES) &&(verbosity == VERBOSE)) {
                if (root->v.type == OP) AST_print_verbose(root);
            }
    
            Value result = AST_eval(root);
            switch (result.type) {
                case NUMBER:
                    fprintf(stdout, "= %.15g\n", result.u.d);
                    break;
                case STRING:
                    fprintf(stdout, "= \"%s\"\n", result.u.s);
                    break;
                default:
                    break;
                    fprintf(stderr, "%s [Line %d]: %s\n", FILENAME, LINE_NUMBER,
                                    "Argh! You've found an interpreter "
                                    "bug: Impossible value");
            }
            Value_free(&result);
        } else if (t == INVALID) {
            if (verbosity != QUIET)
                fprintf(stderr, "%s [Line %d]: Invalid expression\n",
                                FILENAME, LINE_NUMBER);
        } else {
            if (verbosity != QUIET)
                fprintf(stderr, "%s [Line %d]: Expression is not well-typed/"
                                "well-formed\n", FILENAME, LINE_NUMBER);
        }
    
        AST_free(&root);
        SubExp_free(&s);

        if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
        fflush(stdout);
    }
    Env_free(&e);
    free(line);

    if (fp != stdin) fclose(fp);
    fputc('\n', stdout);

    return 0;
}
