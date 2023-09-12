#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELL_COUNT 32768

typedef uint8_t byte_t;

FILE *g_src_file;
char *g_src_file_name;
char *g_src_chars; // each character from the source file
long g_src_len;
int g_src_idx;

byte_t g_cells[CELL_COUNT]; // one-byte cells for manipulation
int g_data_ptr = 0;         // instruction pointer (index of current cell)
int g_error = 0;            // true if an error has occured

void execute_command(char command);
void print_error(char *file_name, char *type, char *message);

/* Interpret and execute Brainf*ck program. */
int main(int argc, char *argv[])
{
    int i;
    char command;

    printf("\n");
    printf("=== cbfi v1.0.0 ===\n");
    printf("An interpreter written in C for the esoteric programming language Brainf*ck\n");
    printf("Written by Andrew Harabor, https://github.com/andrewharabor/cbfi\n");
    printf("Usage: %s [FILENAME.bf]\n", argv[0]);
    printf("\n");

    if (argc != 2)
    {
        print_error(argv[0], "ARGUMENT ERROR", "incorrect number of command-line arguments");

        return g_error;
    }

    // Open source file and read data
    g_src_file_name = malloc(sizeof(char) * strlen(argv[1]));
    strcpy(g_src_file_name, argv[1]);

    g_src_file = fopen(g_src_file_name, "r");
    if (g_src_file == NULL)
    {
        print_error(g_src_file_name, "FILE ERROR", "cannot open file");

        free(g_src_file_name);

        return g_error;
    }

    fseek(g_src_file, 0, SEEK_END);
    g_src_len = ftell(g_src_file) + 1;
    fseek(g_src_file, 0, SEEK_SET);

    g_src_chars = malloc(sizeof(char) * g_src_len);
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
        if (g_error)
        {
            free(g_src_file_name);
            free(g_src_chars);

            return g_error;
        }
    }

    free(g_src_file_name);
    free(g_src_chars);

    return 0;
}

/* Execute a Brainf*ck command. */
void execute_command(char command)
{
    int balance, open_idx, close_idx, i;
    char skipped;
    byte_t input;

    if (command == '>')
    {
        if (g_data_ptr++ == CELL_COUNT - 1)
        {
            g_data_ptr = 0;
        }
    }
    else if (command == '<')
    {
        if (g_data_ptr-- == 0)
        {
            g_data_ptr = CELL_COUNT - 1;
        }
    }
    else if (command == '+')
    {
        if (g_cells[g_data_ptr]++ == 255)
        {
            g_cells[g_data_ptr] = 0;
        }
    }
    else if (command == '-')
    {
        if (g_cells[g_data_ptr]-- == 0)
        {
            g_cells[g_data_ptr] = 255;
        }
    }
    else if (command == '.')
    {
        fprintf(stdout, "%c", g_cells[g_data_ptr]);
    }
    else if (command == ',')
    {
        fscanf(stdin, "%c", &input);
        g_cells[g_data_ptr] = input;
    }
    else if (command == '[')
    {
        balance = 1;
        open_idx = g_src_idx;

        // Find index of the closing bracket
        i = open_idx + 1;
        for (i = open_idx + 1; i < g_src_len; i++)
        {
            if ((skipped = g_src_chars[i]) == '\0')
            {
                print_error(g_src_file_name, "SYNTAX ERROR", "unmatched opening bracket");

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

        // Execute the commands within the matching brackets if applicable
        while (g_cells[g_data_ptr] != 0)
        {
            for (g_src_idx = open_idx + 1; g_src_idx < close_idx; g_src_idx++)
            {
                execute_command(g_src_chars[g_src_idx]);
            }

            if (g_error)
            {
                return;
            }
        }

        g_src_idx = close_idx;
    }
    else if (command == ']') // Matching brackets are handled together
    {
        print_error(g_src_file_name, "SYNTAX ERROR", "unmatched closing bracket");

        return;
    }

    return;
}

/* Print "`file_name`: `type`: `message` to `stderr`."" */
void print_error(char *file_name, char *type, char *message)
{
    fprintf(stderr, "%s: %s: %s\n", file_name, type, message);
    g_error = 1;

    return;
}
