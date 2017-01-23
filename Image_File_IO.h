//  Image_File_IO contains routines for saving and loading Targa (tga) and Bitmap (bmp)
//  files.  The original code for the targa files was written by Richard S. Wright Jr.
//  The original code for the bitmap files was written by Sam Buss.  The code was rewritten
//  by Dr. Don Spickler for use in Math/COSC 482 Computer Graphics.
//
//  The main files of interest are:
//
//  GLbyte *LoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat)
//  bool WriteTGA(const char *szFileName)
//  GLbyte* LoadBMP(const char *filename, GLint *iWidth, GLint *iHeight)
//  bool WriteBMP( const char* filename )
//
//  To implement the readers declare a pointer to a GLbyte type as
//
//  GLbyte *image = NULL;
//
//  then your calls will look like
//
//  image = LoadTGA("testTGA.tga", &picw, &pich, &components, &glformat);       and
//
//  image = LoadBMP("testBMP.bmp", &picw, &pich);
//  glformat = GL_RGB;
//
//  Then transfer the data using
//
//  glDrawPixels(picw, pich, glformat, GL_UNSIGNED_BYTE, image);
//
//  For writing simply flush or swap the buffer and then call either
//
//  WriteTGA("testTGA001.tga");    or     WriteBMP("testBMP001.bmp");

#ifndef __Image_File_IO__
#define __Image_File_IO__

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <sys/time.h>
#else  // Windows & Linux
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// Universal includes
#include <math.h>
#include <stdio.h>

#define GL_BGR_EXT 0x80E0
#define GL_BGRA_EXT 0x80E1

// Define targa header.
#pragma pack(1)
typedef struct
{
    GLbyte	identsize;                      // Size of ID field that follows header (0)
    GLbyte	colorMapType;                   // 0 = None, 1 = paletted
    GLbyte	imageType;                      // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short	colorMapStart;          // First colour map entry
    unsigned short	colorMapLength;         // Number of colors
    unsigned char 	colorMapBits;           // bits per palette entry
    unsigned short	xstart;                 // image x origin
    unsigned short	ystart;                 // image y origin
    unsigned short	width;                  // width in pixels
    unsigned short	height;                 // height in pixels
    GLbyte	bits;                           // bits per pixel (8 16, 24, 32)
    GLbyte	descriptor;                     // image descriptor
} TGAHEADER;
#pragma pack(8)

bool WriteTGA(const char *szFileName)
{
    FILE *pFile;                // File pointer
    TGAHEADER tgaHeader;		    // TGA file header
    unsigned int lImageSize;   // Size in bytes of image
    GLbyte	*pBits = NULL;      // Pointer to bits
    GLint iViewport[4];         // Viewport in pixels
    GLint lastBuffer;           // Storage for the current read buffer setting  --- GLint replaces the GLenum

    // Get the viewport dimensions
    glGetIntegerv(GL_VIEWPORT, iViewport);

    // How big is the image going to be (targas are tightly packed)
    lImageSize = iViewport[2] * 3 * iViewport[3];

    // Allocate block. If this doesn't work, go home
    pBits = (GLbyte *)malloc(lImageSize);
    if(pBits == NULL) return false;

    // Read bits from color buffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

    // Get the current read buffer setting and save it. Switch to
    // the front buffer and do the read operation. Finally, restore
    // the read buffer state
    glGetIntegerv(GL_READ_BUFFER, &lastBuffer);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);
    glReadBuffer(lastBuffer);

    // Initialize the Targa header
    tgaHeader.identsize = 0;
    tgaHeader.colorMapType = 0;
    tgaHeader.imageType = 2;
    tgaHeader.colorMapStart = 0;
    tgaHeader.colorMapLength = 0;
    tgaHeader.colorMapBits = 0;
    tgaHeader.xstart = 0;
    tgaHeader.ystart = 0;
    tgaHeader.width = iViewport[2];
    tgaHeader.height = iViewport[3];
    tgaHeader.bits = 24;
    tgaHeader.descriptor = 0;

    // Do byte swap for big vs little endian
#ifdef __APPLE__
    BYTE_SWAP(tgaHeader.colorMapStart);
    BYTE_SWAP(tgaHeader.colorMapLength);
    BYTE_SWAP(tgaHeader.xstart);
    BYTE_SWAP(tgaHeader.ystart);
    BYTE_SWAP(tgaHeader.width);
    BYTE_SWAP(tgaHeader.height);
#endif

    // Attempt to open the file
    pFile = fopen(szFileName, "wb");
    if(pFile == NULL)
    {
        free(pBits);    // Free buffer and return error
        return false;
    }

    // Write the header
    fwrite(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    // Write the image data
    fwrite(pBits, lImageSize, 1, pFile);

    // Free temporary buffer and close the file
    free(pBits);
    fclose(pFile);
    return true;
}

GLbyte *LoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat)
{
    FILE *pFile;			// File pointer
    TGAHEADER tgaHeader;		// TGA file header
    unsigned int lImageSize;		// Size in bytes of image
    short sDepth;			// Pixel depth;
    GLbyte	*pBits = NULL;          // Pointer to bits

    // Default/Failed values
    *iWidth = 0;
    *iHeight = 0;
    *eFormat = GL_BGR_EXT;
    *iComponents = GL_RGB8;

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    // Attempt to open the file
    pFile = fopen(szFileName, "rb");
    if(pFile == NULL)
        return NULL;

    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);

    // Do byte swap for big vs little endian
#ifdef __APPLE__
    BYTE_SWAP(tgaHeader.colorMapStart);
    BYTE_SWAP(tgaHeader.colorMapLength);
    BYTE_SWAP(tgaHeader.xstart);
    BYTE_SWAP(tgaHeader.ystart);
    BYTE_SWAP(tgaHeader.width);
    BYTE_SWAP(tgaHeader.height);
#endif

    // Get width, height, and depth of texture
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;

    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;

    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Allocate memory and check for success
    pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

    if(pBits == NULL)
        return NULL;

    // Read in the bits
    // Check for read error. This should catch RLE or other
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)
    {
        free(pBits);
        return NULL;
    }

    // Set OpenGL format expected
    switch(sDepth)
    {
    case 3:     // Most likely case
        *eFormat = GL_BGR_EXT;
        *iComponents = GL_RGB8;
        break;
    case 4:
        *eFormat = GL_BGRA_EXT;
        *iComponents = GL_RGBA8;
        break;
    case 1:
        *eFormat = GL_LUMINANCE;
        *iComponents = GL_LUMINANCE8;
        break;
    };

    // Done with File
    fclose(pFile);

    // Return pointer to image data
    return pBits;
}


int GetNumBytesPerRow(int NumCols)
{
    return ((3*NumCols+3)>>2)<<2;
}

void writeint( int data, FILE* outfile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1, byte2, byte3;
    byte0 = (unsigned char)(data&0x000000ff);		// Write bytes, low order to high order
    byte1 = (unsigned char)((data>>8)&0x000000ff);
    byte2 = (unsigned char)((data>>16)&0x000000ff);
    byte3 = (unsigned char)((data>>24)&0x000000ff);

    fputc( byte0, outfile );
    fputc( byte1, outfile );
    fputc( byte2, outfile );
    fputc( byte3, outfile );
}

void writeShort( short data, FILE* outfile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1;
    byte0 = data&0x000000ff;		// Write bytes, low order to high order
    byte1 = (data>>8)&0x000000ff;

    fputc( byte0, outfile );
    fputc( byte1, outfile );
}

bool WriteBMP( const char* filename )
{
    //  Get the buffer from the current openGL buffer

    int viewportData[4];
    glGetIntegerv( GL_VIEWPORT, viewportData );
    int& NumCols = viewportData[2];
    int& NumRows = viewportData[3];
    GLbyte *ImagePtr = NULL;

    ImagePtr = new GLbyte[NumRows*GetNumBytesPerRow(NumCols)];

    int oldGlRowLen;
    glGetIntegerv( GL_UNPACK_ROW_LENGTH, &oldGlRowLen );
    glPixelStorei( GL_UNPACK_ROW_LENGTH, NumCols );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Get the frame buffer data.
    glReadPixels( 0, 0, NumCols, NumRows, GL_RGB, GL_UNSIGNED_BYTE, ImagePtr);

    // Restore the row length in glPixelStorei  (really ought to restore alignment too).
    glPixelStorei( GL_UNPACK_ROW_LENGTH, oldGlRowLen );

    //  Write BMP file

    FILE* outfile = fopen( filename, "wb" );		// Open for reading binary data
    if ( !outfile )
        return false;

    fputc('B',outfile);
    fputc('M',outfile);
    int rowLen = GetNumBytesPerRow(NumCols);
    writeint( 40+14+NumRows*rowLen, outfile );	// Length of file
    writeShort( 0, outfile );					// Reserved for future use
    writeShort( 0, outfile );
    writeint( 40+14, outfile );				// Offset to pixel data
    writeint( 40, outfile );					// header length
    writeint( NumCols, outfile );				// width in pixels
    writeint( NumRows, outfile );				// height in pixels (pos for bottom up)
    writeShort( 1, outfile );		// number of planes
    writeShort( 24, outfile );		// bits per pixel
    writeint( 0, outfile );		// no compression
    writeint( 0, outfile );		// not used if no compression
    writeint( 0, outfile );		// Pixels per meter
    writeint( 0, outfile );		// Pixels per meter
    writeint( 0, outfile );		// unused for 24 bits/pixel
    writeint( 0, outfile );		// unused for 24 bits/pixel

    // Now write out the pixel data:
    GLbyte* cPtr = ImagePtr;

    for ( int i=0; i<NumRows; i++ )
    {
        // Write out i-th row's data
        int j;
        for ( j=0; j<NumCols; j++ )
        {
            fputc( *(cPtr+2), outfile);		// Red color value
            fputc( *(cPtr+1), outfile);		// Green color value
            fputc( *(cPtr+0), outfile);		// Blue color value
            cPtr+=3;
        }
        // Pad row to word boundary
        int k=3*NumCols;					// Num bytes already read
        for ( ; k<GetNumBytesPerRow(NumCols); k++ )
        {
            fputc( 0, outfile );				// Read and ignore padding;
            cPtr++;
        }
    }

    fclose( outfile );	// Close the file
    delete[] ImagePtr;
    return true;
}

short readShort( FILE* infile )
{
    // read a 16 bit integer
    unsigned char lowByte, hiByte;
    lowByte = fgetc(infile);			// Read the low order byte (little endian form)
    hiByte = fgetc(infile);			// Read the high order byte

    // Pack together
    short ret = hiByte;
    ret <<= 8;
    ret |= lowByte;
    return ret;
}

int readint( FILE* infile )
{
    // Read in 32 bit integer
    unsigned char byte0, byte1, byte2, byte3;
    byte0 = fgetc(infile);			// Read bytes, low order to high order
    byte1 = fgetc(infile);
    byte2 = fgetc(infile);
    byte3 = fgetc(infile);

    // Pack together
    int ret = byte3;
    ret <<= 8;
    ret |= byte2;
    ret <<= 8;
    ret |= byte1;
    ret <<= 8;
    ret |= byte0;
    return ret;
}

void skipChars( FILE* infile, int numChars )
{
    for ( int i=0; i<numChars; i++ )
        fgetc( infile );
}

GLbyte* LoadBMP(const char *filename, GLint *iWidth, GLint *iHeight)
{
    FILE* infile = fopen( filename, "rb" );		// Open for reading binary data
    if (!infile) return NULL;

    bool fileFormatOK = false;
    int NumCols;
    int NumRows;

    int bChar = fgetc( infile );
    int mChar = fgetc( infile );
    if ( bChar=='B' && mChar=='M' )   // If starts with "BM" for "BitMap"
    {
        skipChars( infile, 4+2+2+4+4 );			// Skip 4 fields we don't care about
        NumCols = readint( infile );
        NumRows = readint( infile );
        skipChars( infile, 2 );					// Skip one field
        int bitsPerPixel = readShort( infile );
        skipChars( infile, 4+4+4+4+4+4 );		// Skip 6 more fields

        if (NumCols>0 && NumCols<=100000 && NumRows>0 && NumRows<=100000 && bitsPerPixel==24 && !feof(infile))
            fileFormatOK = true;
    }

    *iWidth = (GLint)NumCols;
    *iHeight = (GLint)NumRows;

    if ( !fileFormatOK )
    {
        fclose ( infile );
        return NULL;
    }

    // Allocate memory
    GLbyte* ImagePtr = (GLbyte*)malloc(NumRows*GetNumBytesPerRow(NumCols));

    if ( !ImagePtr )
    {
        fclose ( infile );
        return NULL;
    }

    GLbyte* cPtr = ImagePtr;
    for ( int i=0; i<NumRows; i++ )
    {
        int j;
        for ( j=0; j<NumCols; j++ )
        {
            *(cPtr+2) = fgetc( infile );	// Blue color value
            *(cPtr+1) = fgetc( infile );	// Green color value
            *cPtr = fgetc( infile );		// Red color value
            cPtr += 3;
        }
        int k=3*NumCols;					// Num bytes already read
        for ( ; k<GetNumBytesPerRow(NumCols); k++ )
        {
            fgetc( infile );				// Read and ignore padding;
            *(cPtr++) = 0;
        }
    }
    if ( feof( infile ) )
    {
        delete[] ImagePtr;
        fclose ( infile );
        return false;
    }

    fclose( infile );	// Close the file
    return ImagePtr;
}

#endif
