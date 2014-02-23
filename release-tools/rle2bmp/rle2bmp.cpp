/*
 * rle2bmp.cpp
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

#define _DEBUG

#define IMAGE_WIDTH	(720)
#define IMAGE_HEIGHT	(1280)

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

static inline void rgb565to888(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b) {
    b = (uint8_t)((rgb565 & 0x001F) << 3);  // 00000 000000 11111
    g = (uint8_t)((rgb565 & 0x07E0) >> 3);  // 00000 111111 00000
    r = (uint8_t)((rgb565 & 0xF800) >> 8); // 11111 000000 00000
}


int main(int argc, char** argv)
{
    int ret = 0;
    FILE *fp;
    size_t fileSize = 0;
    fpos_t filepos;
    uint8_t* pFileBuf = NULL;
    uint8_t* pRleData = NULL;
    uint8_t* pBmpBuf = NULL;
    struct BITMAPFILEHEADER bmpFileHeader;
    struct BITMAPINFOHEADER bmpInfoHeader;

    if (argc != 2) {
        fprintf(stderr, "Usage: rle2bmp [initlogo.rle]\n");
        ret = -1;
        goto exit;
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "error: can't open rle file.\n");
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
    pRleData = pFileBuf;

    fread(pFileBuf, sizeof(uint8_t), fileSize, fp);

    fclose(fp);


    bmpFileHeader.bfType = 0x4d42; // BM
    bmpFileHeader.bfSize = IMAGE_WIDTH * IMAGE_HEIGHT * 3 + 54;
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = 54;

    fwrite(&bmpFileHeader.bfType, sizeof(uint8_t), 2, stdout);
    fwrite(&bmpFileHeader.bfSize, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpFileHeader.bfReserved1, sizeof(uint8_t), 2, stdout);
    fwrite(&bmpFileHeader.bfReserved2, sizeof(uint8_t), 2, stdout);
    fwrite(&bmpFileHeader.bfOffBits, sizeof(uint8_t), 4, stdout);

    bmpInfoHeader.biSize = 40;
    bmpInfoHeader.biWidth = IMAGE_WIDTH;
    bmpInfoHeader.biHeight = IMAGE_HEIGHT;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = 24;;
    bmpInfoHeader.biCompression = 0;
    bmpInfoHeader.biSizeImage = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
    bmpInfoHeader.biXPixPerMeter = 3779;
    bmpInfoHeader.biYPixPerMeter = 3779;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImporant = 0;

    fwrite(&bmpInfoHeader.biSize, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biWidth, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biHeight, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biPlanes, sizeof(uint8_t), 2, stdout);
    fwrite(&bmpInfoHeader.biBitCount, sizeof(uint8_t), 2, stdout);
    fwrite(&bmpInfoHeader.biCompression, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biSizeImage, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biXPixPerMeter, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biYPixPerMeter, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biClrUsed, sizeof(uint8_t), 4, stdout);
    fwrite(&bmpInfoHeader.biClrImporant, sizeof(uint8_t), 4, stdout);

    // convert rgb565 rle to rgb888
    uint8_t r,g,b;
    uint16_t count, rgb565;
    uint32_t cur_y, cur_x, out_pos;

    pBmpBuf = (uint8_t*)malloc(bmpInfoHeader.biSizeImage);
    cur_x = 0;
    cur_y = IMAGE_HEIGHT - 1;
    out_pos = IMAGE_WIDTH * 3 * cur_y;

    while (fileSize) {
        count = read_2byte(pRleData);
        rgb565 = read_2byte(pRleData);
        fileSize -= 4;
        rgb565to888(rgb565, r, g, b);
        while (count--) {
            pBmpBuf[out_pos++] = b;
            pBmpBuf[out_pos++] = g;
            pBmpBuf[out_pos++] = r;
            cur_x++;
            if (cur_x == IMAGE_WIDTH) {
                cur_y--;
                cur_x = 0;
                out_pos = IMAGE_WIDTH * 3 * cur_y;
            }
        }
    }

    fwrite(pBmpBuf, sizeof(uint8_t), bmpInfoHeader.biSizeImage, stdout);

    fprintf(stderr, "success: convert bitmap image complete.\n");
exit:
    if (pBmpBuf) free(pBmpBuf);
    if (pFileBuf) free(pFileBuf);
    return ret;
}






