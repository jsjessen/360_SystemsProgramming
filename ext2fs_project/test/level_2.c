#include <level_2.h>

static u8* SAMPLE_DATA;
static long SAMPLE_SIZE;
static char* filename = "test";

int GetRandomSize()
{
    if (SAMPLE_SIZE > 0)
        return  rand() % (SAMPLE_SIZE - MIN_TEST_SIZE) + MIN_TEST_SIZE;
    else
        return rand() % MAX_TEST_SIZE + MIN_TEST_SIZE;
}

int GetCommonSize(void)
{
    // 8,16,32,...,1024,2048,4096
    return (int)pow(2, rand() % 12 + 3); 
}

int GetUncommonSize()
{
    int size = 0;

    // Must be odd
    while (size % 2 == 0)
        size = GetRandomSize();

    return size;
}

void GenerateSampleData()
{
    SAMPLE_SIZE = 0;
    SAMPLE_SIZE = GetRandomSize();
    SAMPLE_DATA = (u8*)malloc(SAMPLE_SIZE);

    for (int i = 0; i < SAMPLE_SIZE; i++)
        SAMPLE_DATA[i] = rand() % 256;
}

void DeleteSampleData()
{
    free(SAMPLE_DATA);
    SAMPLE_DATA = NULL;
    SAMPLE_SIZE = 0;
}

void LinuxRead()
{
    u8 buf[SAMPLE_SIZE];

    int fd = open(filename, O_RDONLY);
    int bytes_read = read(fd, buf, SAMPLE_SIZE);
    close(fd);

    if (memcmp(SAMPLE_DATA, buf, bytes_read) == 0)
        printf("PASS\n");
    else
        printf("FAIL\n");

    if(bytes_read != SAMPLE_SIZE)
        printf("\tWARNING: Linux was unable to read the entire sample!\n");
}

void LinuxWrite()
{
    int fd = open(filename, O_TRUNC | O_WRONLY);
    int bytes_wrote = write(fd, SAMPLE_DATA, SAMPLE_SIZE);
    close(fd);

    if(bytes_wrote != SAMPLE_SIZE)
        printf("\tWARNING: Linux was unable to write the entire sample!\n");
}

void SequentialWrite(int fd, int buf_size)
{
    long position = 0;
    seek_file(fd, position);

    for (int i = 0; i < SAMPLE_SIZE; i += buf_size)
    {
        int bytes_write;
        if((i + buf_size) > SAMPLE_SIZE)
            bytes_write = write_file(fd, (char*)(SAMPLE_DATA + i), SAMPLE_SIZE - i);
        else
            bytes_write = write_file(fd, (char*)(SAMPLE_DATA + i), buf_size);

        position += bytes_write;

        // Validate the stream position after each write.
        if (running->fd[fd]->offset != position)
        {
            printf("Failed: Incorrect position\n");
            return;
        }
    }
}

void SequentialRead(int fd, int buf_size)
{
    seek_file(fd, 0);

    u8 buf[SAMPLE_SIZE];

    for (int i = 0; i < SAMPLE_SIZE; i += buf_size)
        read_file(fd, (char*)(buf + i), buf_size);

    if (memcmp(SAMPLE_DATA, buf, SAMPLE_SIZE) == 0)
        printf("PASS\n");
    else
        printf("FAIL\n");
}



void VerifyContents(int fd)
{
    putchar('\n');
    printf("\tRead: Linux...");
    LinuxRead();

    printf("\tRead: all at once...");
    SequentialRead(fd, SAMPLE_SIZE);

    int buf_size = GetCommonSize();
    printf("\tRead: sequentially with %d byte buffer...", buf_size);
    SequentialRead(fd, buf_size);

    buf_size = GetUncommonSize();
    printf("\tRead: sequentially with %d byte buffer...", buf_size);
    SequentialRead(fd, buf_size);
}

void TestWriteValidation(int fd)
{
    printf("Write Validation\n");
    print_divider('-');

    printf("\nWrite: all at once...");
    SequentialWrite(fd, SAMPLE_SIZE);
    VerifyContents(fd);

    int buf_size = GetCommonSize();
    printf("\nWrite: sequentially with %d byte buffer...", buf_size);
    SequentialWrite(fd, buf_size);
    VerifyContents(fd);

    buf_size = GetUncommonSize();
    printf("\nWrite: sequentially with %d byte buffer...", buf_size);
    SequentialWrite(fd, buf_size);
    VerifyContents(fd);
}

u8* SequentialRead2(int fd, int buf_size, int low_bound, int high_bound)
{
    printf("Reading in sequential order...\n");

    //int size = high_bound - low_bound;

    seek_file(fd, low_bound);

    u8* buf = (u8*)malloc(SAMPLE_SIZE);

    for (int i = low_bound; i < high_bound; i += buf_size)
        read_file(fd, (char*)(buf + i), buf_size);

    return buf;
}

u8* BackwardsRead2(int fd, int buf_size, int low_bound, int high_bound)
{
    printf("Reading in backwards order...\n");

    //int size = high_bound - low_bound;

    u8* buf = (u8*)malloc(SAMPLE_SIZE);

    for (int i = buf_size; (i - buf_size) < high_bound; i += buf_size)
    {
        long position;

        if ((high_bound - i) < low_bound)
            position = low_bound;
        else
            position = high_bound - i;

        seek_file(fd, position);
        read_file(fd, (char*)buf + position, buf_size);
    }

    return buf;
}

u8* RandomRead2(int fd, int low_bound, int high_bound)
{
    printf("Reading in random order...\n");

    int size = high_bound - low_bound;

    bool read_complete = false;
    u8* tracker = (u8*)malloc(SAMPLE_SIZE);
    u8* buf = (u8*)malloc(SAMPLE_SIZE);

    memset(tracker, 0, SAMPLE_SIZE);

    while (!read_complete)
    {
        long position = rand() % size + low_bound;
        int buf_size = GetRandomSize();

        seek_file(fd, position);
        read_file(fd, (char*)(buf + position), buf_size);

        for (int i = position; i < (position + buf_size); i++)
        {
            if (i > high_bound)
                break;

            tracker[i] = 1;
        }

        read_complete = true;
        for (int i = low_bound; i < high_bound; i++)
        {
            if (tracker[i] == 0)
            {
                read_complete = false;
                break;
            }
        }
    }

    return buf;
}

void TestDataConsistency(int fd)
{
    printf("\nData Consistency\n");
    print_divider('-');

    seek_file(fd, 0);
    write_file(fd, (char*)SAMPLE_DATA, SAMPLE_SIZE);

    int low_bound = rand() % running->fd[fd]->mip->inode.i_size;
    int high_bound = 0;

    while (high_bound <= low_bound)
        high_bound = rand() % running->fd[fd]->mip->inode.i_size;

    int region_size = high_bound - low_bound;

    u8* sequential = SequentialRead2(fd, GetRandomSize(), low_bound, high_bound);
    u8* backwards = BackwardsRead2(fd, GetRandomSize(), low_bound, high_bound);
    u8* random = RandomRead2(fd, low_bound, high_bound);

    // If A=B and B=C, then A=C.  Thus, A=B=C.
    if (memcmp(sequential + low_bound, backwards + low_bound, region_size) == 0
            && memcmp(backwards + low_bound, random + low_bound, region_size) == 0)
        printf("Result: PASS\n");
    else
        printf("Result: FAIL\n");

    free(sequential);
    free(backwards);
    free(random);
}

void level_2()
{
    srand(time(NULL));

    int fd = open_file(filename, RW);

    int trial = 0;
    while(trial < TRIALS)
    {
        GenerateSampleData();
        printf("Sample Size = %ld\n\n", SAMPLE_SIZE);

        TestWriteValidation(fd);
        TestDataConsistency(fd);

        print_divider('*');

        DeleteSampleData();
        trial++;
    }

    close_file(fd);
}
