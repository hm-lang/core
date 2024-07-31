#include "common.h"
#include "file.h"

#include <stdio.h>

int main(int Arguments_count, const char **Arguments)
{   const char *File_path = Arguments[Arguments_count];
    file_t File;
    file_uh_t Uh = file_open({.File = &File, .Mode = "r", .Path = File_path});
    if (Uh != File_uh_ok)
    {   fprintf(Error_file, "could not open %s for reading\n", File_path);
        return 1;
    }

    char Line[256];
    while
    (       File_uh_ok
        ==  (Uh = file_read({.File = &File, .Line = Line, .Line_available_bytes = 256}))
    )
    {   fprintf(Output_file, "reading: %s!\n", Line);
    }
    if (Uh == File_uh_ended)
    {   fprintf(Output_file, "file ended!\n");
    }
    else
    {   fprintf(Error_file, "uncaught error reading file\n");
        return 1;
    }
}
