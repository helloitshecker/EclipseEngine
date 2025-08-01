#include "FileSystem.h"

#include <stdio.h>
#include <stdlib.h>

EString* eFileSystem_ReadFile(EMemory* memory, const char* filepath, bool binary) {
        FILE* file;
        if (!fopen_s(&file, filepath, "rb")) {
                EERROR("Failed to open file %s", filepath);
                return nullptr;
        }

        EString* str = (EString*)eMemory_Alloc(memory, sizeof(EString));

        fseek(file, 0, SEEK_END);
        u64 size = ftell(file);
        rewind(file);

        str->content = (char*)eMemory_Alloc(memory, size + 1);
        str->contentSize = size + 1;

        fread(str->content, 1, size, file);
        str->content[size] = '\0';

        fclose(file);
        return str;
}
