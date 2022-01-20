#include <fstream>
#include <cstdio>
#include <memory>
#include <iostream>


static int readFile(const char* path, char* &data, int* size)
{
    std::ifstream file(path);
    if (!file)
        return -1;
    file.seekg(std::ios::end); // End of the stream
    size_t length = file.tellg();
    data = (char*)malloc(length * sizeof(char));
    if (!data)
    {
        throw "Can't allocate memory";
    }
    file.seekg(std::ios::beg); // Beginning of the stream
    for (int i = 0; i < length; i++)
    {
        file >> data[i];
    }
    return 0;
}

static int preprocessInclude(char* data, const size_t size)
{
    char* buffer = (char*)malloc(size+1);
    if (!buffer)
    {
        throw "Can't allocate memory";
    }
    buffer[size] = ' ';
    memcpy(buffer, data, size);

    char* cp = buffer;
    bool comment = false;

    for (uint32_t index = 0; index <= size; index++)
    {
        if (cp[0] == '/' && cp[1] == '/')
        {
            comment = true;
        }
        else if (*cp == '\n')
        {
            comment = false;
        }

        if (comment || *cp < 32 || *cp > 128)
        {
            *cp = 32;
        }
        cp++;
    }

    comment = false;
    cp = buffer;
    char* md = data;
    uint32_t newLen = 0;

    for (uint32_t index = 0; index <= size; index++)
    {
        if (cp[0] == '/' && cp[1] == '*')
        {
            comment = true;
            cp[0] = 32;
            cp[1] = 32;
        }
        else if (cp[0] == '*' && cp[1] == '/')
        {
            comment = false;
            cp[0] = 32;
            cp[1] = 32;
        }

        if (comment)
        {
            cp++;
            continue;
        }
        else if (cp[0] == ' ' && cp[1] == ' ')
        {
            cp++;
            continue;
        }
        else if (cp[-1] == '*' && cp[0] == ' ')
        {
            cp++;
            continue;
        }
        else
        {
            md[0] = cp[0];
            md++;
            newLen++;
            cp++;
        }
    }

    free(buffer);
    return newLen;
}

int main(int argc, char* argv[])
{
	//std::ofstream fileDna(argv[1], std::ios::out);
	//FILE* file_dna_offsets = fopen(argv[2], "w");
	//FILE* file_dna_verify = fopen(argv[3], "w");
    //if (!fileDna) {
    //    printf("Unable to open file: %s\n", argv[1]);
    //    return 1;
    //}
    //else if (!file_dna_offsets) {
    //    printf("Unable to open file: %s\n", argv[2]);
    //    return 1;
    //}
    //else if (!file_dna_verify) {
    //    printf("Unable to open file: %s\n", argv[3]);
    //    return 1;
    //}
    //else
    //{
    //    fileDna << "extern ";
    //}
    char* data = nullptr;
    int size = 0;
    if (!readFile("../../../../Spring/SpringDNA/src/structs.hpp", data, &size))
    {
        preprocessInclude(data, size);
    }
    else
    {
        std::cout << "Couldn't open file " << std::endl;
    }
	return 0;
}