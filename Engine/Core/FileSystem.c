#include "FileSystem.h"

#include <stdio.h>
#include <stdlib.h>

EString* eFileSystem_ReadFile(EMemory* memory, const char* filepath, bool binary) {
        FILE* file;
        if (fopen_s(&file, filepath, binary?"rb":"r") != 0) {
                EERROR("Failed to read file \"%s\"", filepath);
                return nullptr;
        }

        fseek(file, 0L, SEEK_END);
        const u64 size = ftell(file);
        rewind(file);

        EString* fileContent = eMemory_Alloc(memory, sizeof(EString));
        if (binary) {
                fileContent->content = eMemory_Alloc(memory, size);
                fileContent->contentSize = size;
                fread(fileContent->content, sizeof(char), size, file);
        } else {
                fileContent->content = eMemory_Alloc(memory, size+sizeof(char));
                fileContent->contentSize = size+1;
                fread(fileContent->content, sizeof(char), size, file);
                fileContent->content[size-1] = '\0';
        }

        fclose(file);

        return fileContent;
}
