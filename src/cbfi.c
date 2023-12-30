/*
cbfi, an interpreter written in C for the esoteric programming language Brainf*ck
Copyright (C) 2023  Andrew Harabor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELL_COUNT 32768

FILE *g_src_file;
char *g_src_file_name;
char *g_src_chars; // each character from the source file
uint64_t g_src_len;
uint64_t g_src_idx;

uint8_t g_cells[CELL_COUNT]; // one-byte cells for manipulation
uint16_t g_data_ptr = 0;    // instruction pointer (index of current cell)

void execute_command(char command);
void free_memory();
void print_error(FILE *stream, uint8_t status, char *file_name, char *type, char *message);
void print_description(FILE *stream, char *run_command);
void print_usage(FILE *stream, char *run_command);

/* Interpret and execute Brainf*ck program. */
int main(int argc, char *argv[])
{
    uint8_t i;
    char command;

    if (argc < 2 || argc > 3)
    {
        print_usage(stderr, argv[0]);
        print_error(stderr, 2, argv[0], "ARGUMENT ERROR", "invalid number of command-line arguments");
    }

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            print_description(stdout, argv[0]);
        }
        else
        {
            g_src_file_name = malloc(sizeof(char) * strlen(argv[i]));
            if (g_src_file_name == NULL)
            {
                print_error(stderr, 1, argv[0], "INTERNAL ERROR", "cannot allocate memory for Brainf*ck source file name");
            }
            strcpy(g_src_file_name, argv[i]);
        }
    }

    // Open source file and read data
    g_src_file = fopen(g_src_file_name, "r");
    if (g_src_file == NULL)
    {
        print_error(stderr, 3, argv[0], "FILE ERROR", "Brainf*ck source file cannot be opened or is missing");
    }

    fseek(g_src_file, 0, SEEK_END);
    g_src_len = ftell(g_src_file) + 1;
    fseek(g_src_file, 0, SEEK_SET);

    g_src_chars = malloc(sizeof(char) * g_src_len);
    if (g_src_file_name == NULL)
    {
        print_error(stderr, 1, argv[0], "INTERNAL ERROR", "cannot allocate memory for contents of Brainf*ck source file");
    }

    fread(g_src_chars, g_src_len - 1, 1, g_src_file);
    fclose(g_src_file);

    // Loop through and execute commands
    for (g_src_idx = 0; g_src_idx < g_src_len; g_src_idx++)
    {
        if ((command = g_src_chars[g_src_idx]) == '\0')
        {
            break;
        }

        execute_command(command);
    }

    free(g_src_file_name);
    free(g_src_chars);

    if (ferror(stdin))
    {
        print_error(stderr, 3, argv[0], "FILE ERROR", "error reading from stdin");
    }

    if (ferror(stdout))
    {
        print_error(stderr, 3, argv[0], "FILE ERROR", "error writing to stdout");
    }

    exit(0);
}

/* Execute a Brainf*ck command. */
void execute_command(char command)
{
    uint64_t balance, open_idx, close_idx, i;
    char skipped;

    switch (command)
    {
        case '>':
            if (g_data_ptr++ == CELL_COUNT - 1)
            {
                g_data_ptr = 0;
            }
            break;

        case '<':
            if (g_data_ptr-- == 0)
            {
                g_data_ptr = CELL_COUNT - 1;
            }
            break;

        case '+':
            g_cells[g_data_ptr]++;
            break;

        case '-':
            g_cells[g_data_ptr]--;
            break;

        case '.':
            fprintf(stdout, "%c", g_cells[g_data_ptr]);
            break;

        case ',':
            fscanf(stdin, "%c", &g_cells[g_data_ptr]);
            break;

        case '[':
            balance = 1;
            open_idx = g_src_idx;

            // Find index of the closing bracket
            i = open_idx + 1;
            for (i = open_idx + 1; i < g_src_len; i++)
            {
                if ((skipped = g_src_chars[i]) == '\0')
                {
                    print_error(stderr, 4, g_src_file_name, "SYNTAX ERROR", "unmatched opening bracket");

                    return;
                }

                if (skipped == '[')
                {
                    balance++;
                }
                else if (skipped == ']')
                {
                    if (--balance == 0)
                    {
                        close_idx = i;
                        break;
                    }
                }
            }

            // Execute the commands within the matching brackets
            while (g_cells[g_data_ptr] != 0)
            {
                for (g_src_idx = open_idx + 1; g_src_idx < close_idx; g_src_idx++)
                {
                    execute_command(g_src_chars[g_src_idx]);
                }
            }
            g_src_idx = close_idx;

            break;

        case ']': // Matching brackets are handled together so this means the opening bracket is missing
            print_error(stderr, 4, g_src_file_name, "SYNTAX ERROR", "unmatched closing bracket");
            break;

        default:
            break;
    }

    return;
}


/* Free any memory allocated by `g_src_file_name` or `g_src_chars`. */
void free_memory()
{
    if (g_src_file_name != NULL)
    {
        free(g_src_file_name);
    }

    if (g_src_chars != NULL)
    {
        free(g_src_chars);
    }

    return;
}

/* Print "`file_name`: `type`: `message`" to `stream` and exit the program with return code `status`. */
void print_error(FILE *stream, uint8_t status, char *file_name, char *type, char *message)
{
    free_memory();
    fprintf(stream, "%s: %s: %s\n", file_name, type, message);
    exit(status);

    return;
}

/* Print a short description about the interpreter to `stream`. */
void print_description(FILE *stream, char *run_command)
{
    fprintf(stream, "\n");
    fprintf(stream, "           .o8        .o88o.  o8o\n");
    fprintf(stream, "          \"888        888 `\"  `\"'\n");
    fprintf(stream, " .ooooo.   888oooo.  o888oo  oooo\n");
    fprintf(stream, "d88' `\"Y8  d88' `88b  888    `888\n");
    fprintf(stream, "888        888   888  888     888\n");
    fprintf(stream, "888   .o8  888   888  888     888\n");
    fprintf(stream, "`Y8bod8P'  `Y8bod8P' o888o   o888o\n");
    fprintf(stream, "\n");
    fprintf(stream, "An interpreter written in C for the esoteric programming language Brainf*ck.\n");
    fprintf(stream, "Written by Andrew Harabor, https://github.com/andrewharabor/cbfi\n\n");
    print_usage(stream, run_command);
    fprintf(stream, "\n");

    return;
}

/* Print the usage message about the interpreter to `stream`. */
void print_usage(FILE *stream, char *run_command)
{
    fprintf(stream, "USAGE: %s [-h] file.bf\n    -h: print a short description about the interpreter\n    file.bf: the Brainf*ck program to interpret and execute\n", run_command);

    return;
}
