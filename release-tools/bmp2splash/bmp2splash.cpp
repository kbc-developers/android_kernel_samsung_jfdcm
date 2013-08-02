/*
 * bmp2splash.cpp
 *
 * Copyright (C) 2012 sakuramilk <c.sakuramilk@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

//#define _DEBUG

//#define BITMAP_FORMAT_RGB
#define BITMAP_FORMAT_BRG
//#define BITMAP_FORMAT_BGR

struct tg_SupportedSize
{
	unsigned short width;
	unsigned short height;
}SupportedSize[] = 
{
	{ 480, 800,},
	{ 720,1280,},
	{ 800,1280,},
	{1080,1920,},
};


#ifdef _DEBUG
#define PRINT_VAL(val) printf(#val " = %d(0x%08x)\n", (unsigned int)val, (unsigned int)val)
#else
#define PRINT_VAL(val)
#endif

struct BITMAPFILEHEADER {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

struct BITMAPINFOHEADER {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPixPerMeter;
    int32_t  biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImporant;
};


static inline uint8_t read_byte(uint8_t* &pData) {
    uint8_t value = *pData;
    pData += 1;
    return value;
}

static inline uint16_t read_2byte(uint8_t* &pData) {
    uint16_t value = (*(pData+1) << 8) | *(pData);
    pData += 2;
    return value;
}

static inline uint32_t read_3byte(uint8_t* &pData) {
    uint32_t value = (*(pData+2) << 16) | (*(pData+1) << 8) | *(pData);
    pData += 3;
    return value;
}

static inline uint32_t read_4byte(uint8_t* &pData) {
    uint32_t value = (*(pData+3) << 24) | (*(pData+2) << 16) | (*(pData+1) << 8) | *(pData);
    pData += 4;
    return value;
}

static inline uint16_t rgb888to565(uint8_t r, uint8_t g, uint8_t b) {
#if defined(BITMAP_FORMAT_RGB)
    return ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3));
#elif defined(BITMAP_FORMAT_BRG)
    return ((((g) >> 3) << 11) | (((r) >> 2) << 5) | ((b) >> 3));
#elif defined(BITMAP_FORMAT_BGR)
    return ((((b) >> 3) << 11) | (((g) >> 2) << 5) | ((r) >> 3));
#endif
}


int checkImageSize( struct BITMAPINFOHEADER* pBmpInfoHeader )
{
	int lp;
	int ret = -1;
	fprintf(stderr, "Input Image is %dx%d %dbit.\n", pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight,pBmpInfoHeader->biBitCount);

	if(pBmpInfoHeader->biBitCount != 24) {
		fprintf(stderr, "error: not 24bit bitmap format.\n");
	}
	else
	{
		for(lp=0;lp< (sizeof(SupportedSize)/sizeof(struct tg_SupportedSize));lp++)
		{
			if((pBmpInfoHeader->biWidth == SupportedSize[lp].width && 
				 pBmpInfoHeader->biHeight == SupportedSize[lp].height) )
			{
				ret = 0;
				break;
			}
		}
	}
	return ret;
}

void printSupportedFormat()
{
	int lp;
	fprintf(stderr, "error: bitmap format is not supported.\n");
	fprintf(stderr, "       supported format is \n");
	for(lp=0;lp< (sizeof(SupportedSize)/sizeof(struct tg_SupportedSize));lp++)
	{
		fprintf(stderr, "       %dx%d 24bit.\n", SupportedSize[lp].width, SupportedSize[lp].height);
	}
}

int main(int argc, char** argv)
{
    int ret = 0;
    FILE *fp;
    size_t fileSize = 0;
    fpos_t filepos;
    uint8_t* pFileBuf = NULL;
    uint8_t* pBmpData = NULL;
    struct BITMAPFILEHEADER bmpFileHeader;
    struct BITMAPINFOHEADER bmpInfoHeader;
    uint16_t count = 0, total = 0;


    if (argc != 2) {
        fprintf(stderr, "Usage: bmp2splash [24bit bmp file]\n");
        ret = -1;
        goto exit;
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "error: can't open bmp file.\n");
        ret = -1;
        goto exit;
    }

    fseek(fp, 0, SEEK_END);
    fgetpos(fp, &filepos);
    fseek(fp, 0, SEEK_SET);
    fileSize = filepos.__pos;

    pFileBuf = (uint8_t*)malloc(fileSize);
    if (!pFileBuf) {
        fprintf(stderr, "error: can't allocation memory.\n");
        ret = -1;
        goto exit;
    }
    pBmpData = pFileBuf;

    fread(pFileBuf, sizeof(uint8_t), fileSize, fp);

    fclose(fp);

    bmpFileHeader.bfType = read_2byte(pBmpData);
    bmpFileHeader.bfSize = read_4byte(pBmpData);
    bmpFileHeader.bfReserved1 = read_2byte(pBmpData);
    bmpFileHeader.bfReserved2 = read_2byte(pBmpData);
    bmpFileHeader.bfOffBits = read_4byte(pBmpData);

    PRINT_VAL(bmpFileHeader.bfType);
    PRINT_VAL(bmpFileHeader.bfSize);
    PRINT_VAL(bmpFileHeader.bfReserved1);
    PRINT_VAL(bmpFileHeader.bfReserved2);
    PRINT_VAL(bmpFileHeader.bfOffBits);


    bmpInfoHeader.biSize = read_4byte(pBmpData);
    bmpInfoHeader.biWidth = read_4byte(pBmpData);
    bmpInfoHeader.biHeight = read_4byte(pBmpData);
    bmpInfoHeader.biPlanes = read_2byte(pBmpData);
    bmpInfoHeader.biBitCount = read_2byte(pBmpData);
    bmpInfoHeader.biCompression = read_4byte(pBmpData);
    bmpInfoHeader.biSizeImage = read_4byte(pBmpData);
    bmpInfoHeader.biXPixPerMeter = read_4byte(pBmpData);
    bmpInfoHeader.biYPixPerMeter = read_4byte(pBmpData);
    bmpInfoHeader.biClrUsed = read_4byte(pBmpData);
    bmpInfoHeader.biClrImporant = read_4byte(pBmpData);

    PRINT_VAL(bmpInfoHeader.biSize);
    PRINT_VAL(bmpInfoHeader.biWidth);
    PRINT_VAL(bmpInfoHeader.biHeight);
    PRINT_VAL(bmpInfoHeader.biPlanes);
    PRINT_VAL(bmpInfoHeader.biBitCount);
    PRINT_VAL(bmpInfoHeader.biCompression);
    PRINT_VAL(bmpInfoHeader.biSizeImage);
    PRINT_VAL(bmpInfoHeader.biXPixPerMeter);
    PRINT_VAL(bmpInfoHeader.biYPixPerMeter);
    PRINT_VAL(bmpInfoHeader.biClrUsed);
    PRINT_VAL(bmpInfoHeader.biClrImporant);

	ret = checkImageSize(&bmpInfoHeader);
    if ( ret != 0 )
	{
		printSupportedFormat();
        goto exit;
    }

    // convert rgb888 to rgb565 rle
    uint8_t *pLine;
    uint16_t last, color;
    count = 0;
    total = 0;

    for (uint32_t h = 0; h < bmpInfoHeader.biHeight; h++) {
        pLine = pBmpData + (bmpInfoHeader.biHeight - h - 1) * bmpInfoHeader.biWidth * 3;
        for (uint32_t w = 0; w < bmpInfoHeader.biWidth; w++) {
            uint32_t b = read_byte(pLine);
            uint32_t g = read_byte(pLine);
            uint32_t r = read_byte(pLine);
            uint16_t color = rgb888to565(r, g, b);
            if (count) {
                if ((color == last) && (count != 65535)) {
                    count++;
                    continue;
                } else {
                    fwrite(&count, sizeof(uint8_t), 2, stdout);
                    fwrite(&last, sizeof(uint8_t), 2, stdout);
                    total += count;
                }
            }
            last = color;
            count = 1;
        }
    }
    if (count) {
        fwrite(&count, sizeof(uint8_t), 2, stdout);
        fwrite(&last, sizeof(uint8_t), 2, stdout);
        total += count;
    }

    fprintf(stderr, "success: generate splash image complete.\n");
exit:
    if (pFileBuf) free(pFileBuf);
    return ret;
}






