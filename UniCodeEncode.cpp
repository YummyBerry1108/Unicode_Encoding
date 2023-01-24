#include <stdio.h>
#include <cstring>

unsigned long bigToLittleEndian16(unsigned long uniCode);
unsigned long bigToLittleEndian32(unsigned long uniCode);
unsigned long bigToLittleEndian16_4byte(unsigned long uniCode);
void writeToFile(char *fn, unsigned char *buf, int size);

int mStrPos;
int mStrLen;
// "Aα一𐌀" : 英文字母'A', 希臘字母'α', 中文字'一', 古義大利字'𐌀'
unsigned long mUniCodeStr[] = {0x00000041, 0x000003B1, 0x00002F00, 0x00010300};
unsigned long mUniCode, mUniCodeRaw;

int mBufPos;
unsigned char mEncodeBuffer[100];

int main()
{
    mStrLen = sizeof(mUniCodeStr) / sizeof(mUniCodeStr[0]);

    // UTF-8 Encoding
    memset(mEncodeBuffer, 0, sizeof(mEncodeBuffer));

    mBufPos = 0;
    mEncodeBuffer[mBufPos++] = 0xEF;
    mEncodeBuffer[mBufPos++] = 0xBB;
    mEncodeBuffer[mBufPos++] = 0xBF;

    for (mStrPos = 0; mStrPos < mStrLen; mStrPos++)
    {
        mUniCode = mUniCodeStr[mStrPos];
        mUniCodeRaw = mUniCode;

        if (mUniCode >=0x00 && mUniCode <= 0x7F)
        {
            mEncodeBuffer[mBufPos++] = mUniCodeRaw;
        }
        else if (mUniCode >=0x80 && mUniCode <= 0x7FF)
        {
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 6) | 0xC0;
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw & 0x3F) | 0x80;
        }
        else if (mUniCode >=0x800 && mUniCode <= 0xFFFF)
        {
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 12) | 0xE0;
            mEncodeBuffer[mBufPos++] = ((mUniCodeRaw >> 6) & 0x3F) | 0x80;
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw & 0x3F) | 0x80;
        }
        else if (mUniCode >=0x10000 && mUniCode <= 0x10FFFF)
        {
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 18) | 0xF0;
            mEncodeBuffer[mBufPos++] = ((mUniCodeRaw >> 12) & 0x3F) | 0x80;
            mEncodeBuffer[mBufPos++] = ((mUniCodeRaw >>  6) & 0x3F) | 0x80;
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw & 0x3F) | 0x80;
        }
    }

    writeToFile("UTF-8.txt", mEncodeBuffer, mBufPos);

    // UTF-16LE and UTF-16BE Encoding
    for (int endian = 0; endian < 2; endian++)
    {
        memset(mEncodeBuffer, 0, sizeof(mEncodeBuffer));

        mBufPos = 0;
        if (endian == 0)
        {
            mEncodeBuffer[mBufPos++] = 0xFF;
            mEncodeBuffer[mBufPos++] = 0xFE;
        }
        else
        {
            mEncodeBuffer[mBufPos++] = 0xFE;
            mEncodeBuffer[mBufPos++] = 0xFF;
        }

        for (mStrPos = 0; mStrPos < mStrLen; mStrPos++)
        {
            mUniCode = mUniCodeStr[mStrPos];

            if (mUniCode >=0x00 && mUniCode <= 0xFFFF)
            {
                if (endian == 0)
                    mUniCodeRaw = bigToLittleEndian16(mUniCode);
                else
                    mUniCodeRaw = mUniCode;

                mEncodeBuffer[mBufPos++] = mUniCodeRaw >> 8;
                mEncodeBuffer[mBufPos++] = mUniCodeRaw & 0xFF;
            }
            else if (mUniCode >=0x10000 && mUniCode <= 0x10FFFF)
            {
                mUniCode -= 0x10000;
                if (endian == 0)
                {
                    mUniCodeRaw = bigToLittleEndian16_4byte(mUniCode);
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 12) ;
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 10) & 0x03 | 0xD8;
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 2) & 0xFF ;
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw & 0x03) | 0xDC;
                }
                else
                {
                    mUniCodeRaw = mUniCode;
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 18) | 0xD8;
                    mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 10) & 0xFF;
                    mEncodeBuffer[mBufPos++] = ((mUniCodeRaw >> 8) & 0x03) | 0xDC;
                    mEncodeBuffer[mBufPos++] = mUniCodeRaw & 0xFF;
                }
            }
        }

        if (endian == 0)
            writeToFile("UTF-16LE.txt", mEncodeBuffer, mBufPos);
        else
            writeToFile("UTF-16BE.txt", mEncodeBuffer, mBufPos);
    }

    // UTF-32LE and UTF-32BE Encoding
    for (int endian = 0; endian < 2; endian++)
    {
        memset(mEncodeBuffer, 0, sizeof(mEncodeBuffer));

        mBufPos = 0;
        if (endian == 0)
        {
            mEncodeBuffer[mBufPos++] = 0xFF;
            mEncodeBuffer[mBufPos++] = 0xFE;
            mEncodeBuffer[mBufPos++] = 0x00;
            mEncodeBuffer[mBufPos++] = 0x00;
        }
        else
        {
            mEncodeBuffer[mBufPos++] = 0x00;
            mEncodeBuffer[mBufPos++] = 0x00;
            mEncodeBuffer[mBufPos++] = 0xFE;
            mEncodeBuffer[mBufPos++] = 0xFF;
        }

        for (mStrPos = 0; mStrPos < mStrLen; mStrPos++)
        {
            mUniCode = mUniCodeStr[mStrPos];
            if (endian == 0)
                mUniCodeRaw = bigToLittleEndian32(mUniCode);
            else
                mUniCodeRaw = mUniCode;

            mEncodeBuffer[mBufPos++] = mUniCodeRaw >> 24;
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 16) & 0xFF;
            mEncodeBuffer[mBufPos++] = (mUniCodeRaw >> 8) & 0xFF;
            mEncodeBuffer[mBufPos++] = mUniCodeRaw & 0xFF;
        }

        if (endian == 0)
            writeToFile("UTF-32LE.txt", mEncodeBuffer, mBufPos);
        else
            writeToFile("UTF-32BE.txt", mEncodeBuffer, mBufPos);
    }
}

unsigned long bigToLittleEndian16(unsigned long uniCode)
{
    return (mUniCode << 8) + (mUniCode >> 8);
}

unsigned long bigToLittleEndian16_4byte(unsigned long uniCode)
{
    return ((mUniCode << 2) & 0x000FF3FC) + ((mUniCode >> 8) & 0x00000C03);
}

unsigned long bigToLittleEndian32(unsigned long uniCode)
{
    return (uniCode << 24) + ((uniCode << 8) & 0x00FF0000) + ((uniCode >> 8) & 0x0000FF00) + (uniCode >> 24);
}

void writeToFile(char* fn, unsigned char* buf, int size)
{
    FILE *fp;

    if (fp = fopen(fn, "w"))
    {
        fwrite(buf, 1, size, fp);
        fclose(fp);
    }
}