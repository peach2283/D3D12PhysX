#include "stdafx.h"
#include "HeightField.h"

PxHeightFieldSample * readTerrain(const char *filename, int rows, int cols)
{
    int size;
    FILE *fp = NULL;
    PxHeightFieldSample * data = NULL;

    errno_t errno;
    errno = fopen_s(&fp, filename, "rb");

    if (errno != 0)
    {
        return NULL;
    }

    size = rows * cols;
    data = new PxHeightFieldSample[size];

    for (int i = 0; i < size; i++)
    {
        unsigned char height;   //raw 파일에서 읽은 높이값

        fread(&height, 1, 1, fp);

        data[i].height         = height;  //높이값 저장
        data[i].materialIndex0 = 0; 
        data[i].setTessFlag(); 
    }
    fclose(fp);

    return data;
}
