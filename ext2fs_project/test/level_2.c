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

void SequentialWrite(int buf_size)
{
    int fd = open_file(filename, WR);
    long position = 0;
    seek_file(fd, position);

    for (int i = 0; i < SAMPLE_SIZE; i += buf_size)
    {
        int bytes_write;
        if((i + buf_size) > SAMPLE_SIZE)
            bytes_write = write_file(fd, (SAMPLE_DATA + i), SAMPLE_SIZE - i);
        else
            bytes_write = write_file(fd, (SAMPLE_DATA + i), buf_size);

        position += bytes_write;

        // Validate the stream position after each write.
        if (running->fd[fd]->offset != position)
        {
            close_file(fd);
            printf("Failed: Incorrect position\n");
            return;
        }
    }
    close_file(fd);
}

void SequentialRead(int buf_size)
{
    int fd = open_file(filename, RD);
    u8 buf[SAMPLE_SIZE];

    for (int i = 0; i < SAMPLE_SIZE; i += buf_size)
        read_file(fd, (buf + i), buf_size);

    close_file(fd);

    if (memcmp(SAMPLE_DATA, buf, SAMPLE_SIZE) == 0)
        printf("PASS\n");
    else
        printf("FAIL\n");
}

void VerifyContents()
{
    int fd = open_file(filename, RD);
    putchar('\n');
    printf("\t\tRead: all at once...");
    SequentialRead(SAMPLE_SIZE);

    int buf_size = GetCommonSize();
    printf("\t\tRead: sequentially with %d byte buffer...", buf_size);
    SequentialRead(buf_size);

    buf_size = GetUncommonSize();
    printf("\t\tRead: sequentially with %d byte buffer...", buf_size);
    SequentialRead(buf_size);
    close_file(fd);
}

void TestWriteValidation()
{
    printf("Write Validation\n");

    printf("\n\tWrite: all at once...");
    SequentialWrite(SAMPLE_SIZE);
    VerifyContents();

    int buf_size = GetCommonSize();
    printf("\n\tWrite: sequentially with %d byte buffer...", buf_size);
    SequentialWrite(buf_size);
    VerifyContents();

    buf_size = GetUncommonSize();
    printf("\n\tWrite: sequentially with %d byte buffer...", buf_size);
    SequentialWrite(buf_size);
    VerifyContents();
}

u8* SequentialRead2(int fd, int buf_size, int low_bound, int high_bound)
{
    printf("\tReading in sequential order...\n");

    //int size = high_bound - low_bound;

    seek_file(fd, low_bound);

    u8* buf = (u8*)malloc(SAMPLE_SIZE);

    for (int i = low_bound; i < high_bound; i += buf_size)
        read_file(fd, (buf + i), buf_size);

    return buf;
}

u8* BackwardsRead2(int fd, int buf_size, int low_bound, int high_bound)
{
    printf("\tReading in backwards order...\n");

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
        read_file(fd, buf + position, buf_size);
    }

    return buf;
}

u8* RandomRead2(int fd, int low_bound, int high_bound)
{
    printf("\tReading in random order...\n");

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
        read_file(fd, (buf + position), buf_size);

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

void TestDataConsistency()
{
    printf("\nData Consistency\n\n");

    int fd = open_file(filename, WR);
    write_file(fd, SAMPLE_DATA, SAMPLE_SIZE);
    int size = running->fd[fd]->mip->inode.i_size;
    close_file(fd);

    int low_bound = rand() % size;
    int high_bound = 0;

    while (high_bound <= low_bound)
        high_bound = rand() % size; 

    int region_size = high_bound - low_bound;

    fd = open_file(filename, RD);
    u8* sequential = SequentialRead2(fd, GetRandomSize(), low_bound, high_bound);
    u8* backwards = BackwardsRead2(fd, GetRandomSize(), low_bound, high_bound);
    u8* random = RandomRead2(fd, low_bound, high_bound);
    close_file(fd);

    // If A=B and B=C, then A=C. Thus, A=B=C.
    if (memcmp(sequential + low_bound, backwards + low_bound, region_size) == 0
            && memcmp(backwards + low_bound, random + low_bound, region_size) == 0)
        printf("\tResult: PASS\n");
    else
        printf("\tResult: FAIL\n");

    free(sequential);
    free(backwards);
    free(random);
}

void level_2()
{
    srand(time(NULL));

    int fd = open_file(filename, RD);
    if(fd < 0)
        creat_file(running->cwd, filename);
    else
        close_file(fd);

    int trial = 0;
    while(trial < TRIALS)
    {
        GenerateSampleData();
        print_divider('=');
        printf("Sample Size = %ld\n", SAMPLE_SIZE);
        print_divider('-');

        TestWriteValidation();
        TestDataConsistency();
        putchar('\n');

        DeleteSampleData();
        trial++;
    }
}
