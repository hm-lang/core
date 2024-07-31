#include "file.h"

#include <stdio.h>

file_uh_t file_open(file_open_arguments_t Arguments)
{   Arguments.File->Pointer = fopen(Arguments.Path, Arguments.Mode);
    if (Arguments.File->Pointer == Null)
    {   return File_uh_not_found;
    }
    return File_uh_ok;
}

void file_close(file_close_arguments_t Arguments)
{   fclose(Arguments.File->Pointer);
}

file_uh_t file_read(file_read_arguments_t Arguments)
{   if (Arguments.File->Pointer == Null)
    {   return File_uh_not_found;
    }
    char *Result = fgets(Arguments.Line, Arguments.Line_available_bytes, Arguments.File->Pointer);
    if (Result == Null)
    {   if (feof(Arguments.File->Pointer))
        {   return File_uh_ended;
        }
        if (ferror(Arguments.File->Pointer))
        {   return File_uh_read_error;
        }
        return File_uh_unexpected;
    }
    while (True)
    {   switch (*Result)
        {   case 0:
                return File_uh_line_too_long;
            case '\n':
                // Remove the trailing newline
                *Result = 0;
                return File_uh_ok;
            default:
                ++Result;
        }
    }
}

