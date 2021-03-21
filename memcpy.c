#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static const size_t ARRAY_SIZE = 26 * 41;

void * memcpyTrivial( void* dst, const void* src, size_t count)
{
    unsigned char* dstChar = (unsigned char*)dst;
    const unsigned char* srcChar = (const unsigned char*)src;
    size_t i;

    for( i=0; i<count; ++i )
    {
        dstChar[i]=srcChar[i];
    }

    return dst;
}

void * memcpy8( void* dst, const void* src, size_t count)
{
    unsigned char* dstChar = (unsigned char*)dst;
    const unsigned char* srcChar = (const unsigned char*)src;

    while( count-- )
    {
        *(dstChar++)=*(srcChar++);
    }

    return dst;
}

void * memcpyLL( void* dst, const void* src, size_t count)
{
    // Copy chars until src ptr aligned
    unsigned char* dstChar = (unsigned char*)dst;
    const unsigned char* srcChar = (const unsigned char*)src;

    while( count  && ((intptr_t)srcChar % sizeof(unsigned long long)) )
    {
        *(dstChar++)=*(srcChar++);
        --count;
    }

    // Determine dest pointer alignment
    if( ((intptr_t)dstChar % sizeof(unsigned long long)) == 0 )
    {
        // Copy what you can as LL
        unsigned long long* dstLL = (unsigned long long*)dstChar;
        const unsigned long long* srcLL = (const unsigned long long*)srcChar;

        while( count >= sizeof(unsigned long long) )
        {
            *(dstLL++)=*(srcLL++);
            count-=sizeof(unsigned long long);
        }

        // Setup to copy remainder
        dstChar = (unsigned char*)dstLL;
        srcChar = (const unsigned char*)srcLL;
    }
    else if( ((intptr_t)dstChar % sizeof(unsigned long)) == 0 )
    {
        // Copy what you can as L
        unsigned long* dstL = (unsigned long*)dstChar;
        const unsigned long* srcL = (const unsigned long*)srcChar;

        while( count >= sizeof(unsigned long) )
        {
            *(dstL++)=*(srcL++);
            count-=sizeof(unsigned long);
        }

        // Setup to copy remainder
        dstChar = (unsigned char*)dstL;
        srcChar = (const unsigned char*)srcL;
    }
    else if( ((intptr_t)dstChar % sizeof(unsigned short)) == 0 )
    {
        // Copy what you can as Short
        unsigned short* dstShort = (unsigned short*)dstChar;
        const unsigned short* srcShort = (const unsigned short*)srcChar;

        while( count >= sizeof(unsigned short) )
        {
            *(dstShort++)=*(srcShort++);
            count-=sizeof(unsigned short);
        }

        // Setup to copy remainder
        dstChar = (unsigned char*)dstShort;
        srcChar = (const unsigned char*)srcShort;
    }

    // copy remainder
    while( count-- )
    {
        *(dstChar++)=*(srcChar++);
    }

    return dst;
}

void initArray( char* array )
{
    char a[]="abcdefghijklmnopqrstuvwxyz";
    size_t i;
    for(i=0; i<ARRAY_SIZE; i+=26)
    {
        memcpy(&array[i], a, 26);
    }
    array[80]=0;
}

void clearArray( char* array )
{
    size_t i;
    for(i=0; i<ARRAY_SIZE; ++i)
    {
        array[i]=0;
    }
}

int diffTime( clock_t before, clock_t after )
{
    return (int)((after-before)/1000);
}

void testMemcpy( void *(*func)(void*,const void*,size_t), char* a, char* b, const char* name, size_t count)
{
    const int REPEAT_TIMES = 100000;
    clock_t before;
    clock_t after;
    int i;
    int j;
    int k;

    clearArray(b);
    printf("B:%s\n", b);
    before=clock();
    for(i=0; i<REPEAT_TIMES; ++i)
    {
        for(j=0;j<sizeof(unsigned long long);j++)
        {
            for(k=0;k<sizeof(unsigned long long);k++)
            {
                (*func)(&b[j], &a[k], count);
            }
        }
    }
    after=clock();
    b[40]='\0';
    printf("%s B:%s - %d\n\n", name, b, diffTime(before, after));

}

int main( int argc, char** argv )
{
    char a[ARRAY_SIZE];
    char b[ARRAY_SIZE+10];
    size_t count=ARRAY_SIZE-8;

    initArray(a);
    printf("A:%s\n", a);

    for( count=0; count<1000; count+=100)
    {
        printf( "\nCOUNT = %d\n", (int)count );
        testMemcpy( &memcpy, a, b, "memcpy", count);
        testMemcpy( &memcpyTrivial, a, b, "memcpyTrivial", count);
        testMemcpy( &memcpy8, a, b, "memcpy8", count);
        testMemcpy( &memcpyLL, a, b, "memcpyLL", count);
    }

    return 0;
}
