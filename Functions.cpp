#include "Functions.h"

DWORD Functions::getHashFromString(char *string) {
    DWORD hash = 0xab10f29f;
    char *originalString = string;
    while (*string) hash = (((hash << 5) | (hash >> 27)) + *string++);
    //printf("%s hash: 0x00%x\n", originalString, hash);
    return hash;
}

PDWORD Functions::getFunctionAddressByHash(HMODULE libraryBase, DWORD hash) {
    DWORD base = (DWORD) libraryBase;
    PIMAGE_NT_HEADERS pe = (PIMAGE_NT_HEADERS) (base + ((PIMAGE_DOS_HEADER) base)->e_lfanew); // PE header
    PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY) (base +
                                                                   pe->OptionalHeader.DataDirectory[0].VirtualAddress); // export directory
    PDWORD namePtr = (PDWORD) (base + exportDir->AddressOfNames); // iterator of names
    PWORD ordPtr = (PWORD) (base + exportDir->AddressOfNameOrdinals); // iterator of ordinals

    for (; hash != getHashFromString(
            (char *) (base + *namePtr)); ++namePtr, ++ordPtr); // stop when function name has matching hash

    DWORD funcRVA = *(PDWORD) (base + exportDir->AddressOfFunctions + *ordPtr * 4); // calculate RVA of desired function

    return (PDWORD) ((DWORD_PTR) base + funcRVA); // return memory address of desired function
}