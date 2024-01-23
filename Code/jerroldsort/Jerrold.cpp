//
//  Jerrold.cpp
//
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>

#include "jlmTypes.h"
#include "sha1.h"

#define TEST


void PrintString(byte* pbData, int iSize, int col)
{
    int i;

    for (i= 0; i<iSize; i++) {
        printf("%c", pbData[i]);
        if((i%col)==(col-1))
            printf("\n");
        }
    printf("\n");
}


void PrintBytes(byte* pbData, int iSize)
{
    int i;

    for (i= 0; i<iSize; i++) {
        printf("%02x", pbData[i]);
        }
    printf("\n");
}


// -----------------------------------------------------------------------


byte  delimiter= (byte)':';


//  index points to dictentries
//      Sorted order is rgdictent[0], rgdictent[1], ...


struct dictent {
    byte    m_hash[20];
    i64     position;
};


int compare(byte* hash1, byte* hash2)
// return 1, if hash1>hash2
//        0, if hash1=hash2
//        -1, if hash1<hash2
{
    int i;

    for(i=0; i<20; i++) {
        if(hash1[i]>hash2[i])
            return 1;
        if(hash1[i]<hash2[i])
            return -1;
    }
    return 0;
}


int compar(const void* in1, const void* in2)
{
    return compare(((struct dictent*)in1)->m_hash, ((struct dictent*)in2)->m_hash);
}


bool sortTable(i64 numents, struct dictent* rgdictent)
{
#ifdef TEST1
    printf("sorttable\n");
#endif
    qsort((void*)rgdictent, (size_t)numents, sizeof(struct dictent), compar);
    return true;
}



#define BUFSIZE 8192
int     g_cur= 0;
int     g_nleft= 0;
byte    g_buf[BUFSIZE];


int nextline(int in, int sizeline, byte* line)
{
    byte    b;
    int     sizeout= 0;

    for(;;) {
        if(g_nleft<=0) {
            g_nleft= read(in, g_buf, BUFSIZE-1);
            if(g_nleft<=0)
                return -1;
            g_cur= 0;
        }
        b= g_buf[g_cur];
        g_cur++;
        g_nleft--;
#ifdef TEST1
        printf("nextline read %d bytes, out %d, %c %02x\n", 
               g_nleft, sizeout, b, '\n');
#endif
        line[sizeout++]= b; 
        if(b==(byte)'\n') {
            line[sizeout]= 0; 
            return sizeout;
        }
        if(sizeout>(sizeline-1))
            return -1;
    }
}


#define MAXLINE 4096


bool processTable(const char* file, i64 allocatedsizeContent, byte* contentarray,
                  i64 allocatedEnts, struct dictent* rgEnts, i64* contentbytesused, i64* numberents)
{
    int     in= open(file, O_RDONLY);
    i64     nument= 0L;
    int     linesize= 0;
    byte    line[MAXLINE];
    i64     curContentposition= 0;
    int     endofkey= 0;
    Sha1    oHash;
    int     contSize;

    if(in<0) {
        printf("Cant open input file\n");
        return false;
    }

#ifdef TEST
    printf("processTable\n");
    fflush(stdout);
#endif
    // read lines
    while((linesize=nextline(in, MAXLINE, line))>0 && nument<allocatedEnts) {
        endofkey= 0;
#ifdef TEST1
        printf("Line (%d): %s", linesize, line);
        fflush(stdout);
#endif
        if(linesize<3)
            continue;
        while(endofkey<linesize) {
            if(line[endofkey]==delimiter)
                break;
            endofkey++;
        }
        if(endofkey>=linesize)
            break;
        oHash.Init();
        oHash.Update(line, endofkey);
        oHash.Final();
        oHash.getDigest(rgEnts[nument].m_hash);
        endofkey++;
        contSize= linesize-endofkey;
        memcpy((void*) &contentarray[curContentposition], (void*)&line[endofkey], contSize);
        rgEnts[nument].position= curContentposition;
        curContentposition+= contSize;
        nument++;
#ifdef TEST1
        printf("End line, contSize %d, curpos: %d\n", contSize, curContentposition);
        fflush(stdout);
#endif
    }

    // close file
    close(in);

    // sort
    if(!sortTable(nument, rgEnts)) {
        printf("Cant sort table\n");
        return false;
    }
    *numberents= nument;
#ifdef TEST
    printf("processTable done\n");
    fflush(stdout);
#endif
    return true;
}


i64 findEntry(int sizein, byte* in, int* psizeout, byte* out,
              i64 num, struct dictent* ent)
{
    i64     lower= 0L;
    i64     upper= num-1; 
    i64     middle;
    Sha1    oHash;
    byte    rghash[20];
    int     cmp;

#ifdef TEST1
    printf("findEntry(%d, %d), %d entries\n", sizein, *psizeout, num);
    fflush(stdout);
#endif
    // hash
    oHash.Init();
    oHash.Update(in, sizein);
    oHash.Final();
    oHash.getDigest(rghash);
#ifdef TEST
    printf("findEntry looking for: ");
    PrintBytes(rghash, 20);
    printf("\n");
    fflush(stdout);
#endif

    // binary search
    while(lower<=upper) {
        if((upper-lower)<=1) {
            if(compare(rghash, ent[lower].m_hash)==0)
                return lower;
            if(compare(rghash, ent[upper].m_hash)==0)
                return upper;
            return -1L;
        }

        middle= (lower+upper)/2;
        cmp= compare(rghash, ent[middle].m_hash);
        if(cmp==0)
            return middle;
        if(cmp>0)
            lower= middle;
        else
            upper= middle;
    }
    return -1L;
}


void printEnt(struct dictent* pent, byte* content)
{
    i64 i;
    byte b;

    PrintBytes(pent->m_hash, 20);
    for(i=0; i<500; i++) {
        b= content[i+pent->position];
        printf("%c", b);
        if(b=='\n')
            break;
    }
    printf("\n\n");
}


// -----------------------------------------------------------------------


#define MAXENTS        1000
#define MAXCONTENT  1000000

i64             g_numents;
i64             g_contentsize;

struct dictent  g_dict[MAXENTS];
byte            g_content[MAXCONTENT];


int main(int an, char** av)
{
    const char* szFile= NULL;
    int         i;

    for(i=1; i<an; i++) {
        if(strcmp(av[i], "-input")==0) {
            szFile= av[++i];
            i++;
        }
    }

    printf("Processing file %s\n", szFile);

    if(!processTable(szFile, MAXCONTENT, g_content, MAXENTS, g_dict,
                     &g_contentsize, &g_numents)) {
        printf("Cant process input file\n");
        return -1;
    }

    for(i=0;i<g_numents;i++) {
        printf("%04d ", i);
        printEnt(&g_dict[i], g_content);
    }

#ifdef TEST
    printf("\n\n%d entries processed\n\n\n", g_numents);
#endif

    // look up some entries
    byte    answer[8192];
    int     size= 8192;
    int     num= 2;
    byte*   key[5]= {
        (byte*)"John",
        (byte*)"bacho kiro"
    };
    int     sizein;
    byte*   in;

    i64 res;
    for(i=0; i<num;i++) {
        printf("\nQuery %d, %s\n", i, key[i]);
        size= 8192;
        sizein= (int)strlen((const char*)key[i]);
        in= key[i];
        res= findEntry(sizein, in, &size, answer, g_numents, g_dict);
        if(res<0) {
            printf("Cant find entry\n\n");
        }
        else {
            printf("%d bytes\n", res);
            printf("answer: ");
            printEnt(&g_dict[res], g_content);
        }
    }
    return 0;
}


// ----------------------------------------------------------------------------


