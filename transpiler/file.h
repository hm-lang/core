#pragma once

#include "common.h"

typedef struct file
{   void *Pointer;
}   file_t;

enum file_uh_t
{   File_uh_ok,
    File_uh_ended,
    File_uh_not_found,
    File_uh_line_too_long,
    File_uh_read_error,
    File_uh_unexpected,
};

typedef struct file_open_arguments
{   file_t *File;
    const char *Mode;
    const char *Path;
}   file_open_arguments_t;

file_uh_t file_open(file_open_arguments_t Arguments);

typedef struct file_close_arguments
{   file_t *File;
}   file_close_arguments_t;

void file_close(file_close_arguments_t Arguments);

typedef struct file_read_arguments
{   file_t *File;
    char *Line;
    int Line_available_bytes;
}   file_read_arguments_t;

file_uh_t file_read(file_read_arguments_t Arguments);
