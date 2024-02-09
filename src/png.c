#include "png.h"

DEFLATED_DATA_PTR de_flate(unsigned char* data, unsigned long data_size)
{
    /* ZLIB stuff */    
    int ret;
    z_stream strm;
    unsigned char* compressed_pixels;
    unsigned long compressed_pixels_size;
    // allocate deflate state 
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
    {
        //handle error
        printf("Error ret != Z_OK");
    }
    // compress input buffer    
    strm.avail_in = data_size;
    strm.next_in = data;
    // allocate output buffer
    compressed_pixels_size = deflateBound(&strm, data_size);
    compressed_pixels = (unsigned char *)malloc(compressed_pixels_size);

    strm.avail_out = compressed_pixels_size;
    strm.next_out = compressed_pixels;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        // handle error

        printf("Error ret != Z_STREAM_END");
    }

    // clean up and return 
    (void)deflateEnd(&strm);   
    /* ZLIB stuff */

    DEFLATED_DATA_PTR ptr = malloc(sizeof(DEFLATED_DATA));

    ptr->size = compressed_pixels_size;
    ptr->data = compressed_pixels;

    return ptr;
}


/* ****************************************************************************************** */
/* ******************************************* CRC ****************************************** */
/* ****************************************************************************************** */  
/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];   
/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;

/* Make the table for a fast CRC. */
void make_crc_table(void)
{
    unsigned long c;
    int n, k;
   
    for (n = 0; n < 256; n++) {

        c = (unsigned long) n;

        //printf(" c =  ");

        for (k = 0; k < 8; k++) {

            if (c & 1) {

                c = 0xedb88320L ^ (c >> 1);

            } else {

                c = c >> 1;
            }        
        }
    
        crc_table[n] = c;
    }

    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */   
unsigned long update_crc(unsigned long crc, unsigned char *buf, unsigned int len)
{
    unsigned long c = crc;
    int n;
   
    if (!crc_table_computed) {

       make_crc_table();
    }

    for (n = 0; n < len; n++) {

       c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* ****************************************************************************************** */
/* ***************************************** CRC Ends *************************************** */
/* ****************************************************************************************** */


/* ****************************************************************************************** */
/* ***************************************** Endiness *************************************** */
/* ****************************************************************************************** */
unsigned int little_endian_read_u32(unsigned char* ptr)
{

    return 0;
}

unsigned char* little_endian_write_u32(unsigned char* ptr, unsigned int value)
{

    return NULL;
} 

/*
    @ptr has an array of size 4, this array has a big-endian representation of a value.
    This function will turn the big-endian representation to it equivalent value in little-endian
 */
unsigned int big_endian_read_u32(unsigned char* ptr)
{
    
    return (ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3]);
}

/*
    @ptr has array of size 4, value is stored in this array in its big-endian form/representation
 */
void big_endian_write_u32(unsigned char* ptr, unsigned int value)
{  

    ptr[0] = (value & 0xff000000) >> 24;
    ptr[1] = (value & 0x00ff0000) >> 16;
    ptr[2] = (value & 0x0000ff00) >> 8;
    ptr[3] = (value & 0x000000ff);    
}
/* ****************************************************************************************** */
/* **************************************** Endiness Ends *********************************** */
/* ****************************************************************************************** */

void create_png_file(PNG_PTR png, char* filename) 
{
    FILE* fptr = fopen(filename,"wb");

    // Write signature
    fwrite(png->signature, sizeof(png->signature), 1, fptr);

    if (png != NULL)
    {
        PNG_CHUNK_PTR ptr = png->ptr;

        while (ptr != NULL)
        {
            PNG_CHUNK_PTR next = ptr->next;
            fwrite(ptr->length, sizeof(ptr->length), 1, fptr);
            fwrite(ptr->type, sizeof(ptr->type), 1, fptr);
            if (big_endian_read_u32(ptr->length) > 0)
            {
                fwrite(ptr->data, big_endian_read_u32(ptr->length), 1, fptr);
            }
            fwrite(ptr->crc, sizeof(ptr->crc), 1, fptr);
            
            ptr = next;
        }                         
    }
    
    fclose(fptr);
}

void png_dealloc(PNG_PTR png)
{
    // Only deallocting one PNG file with many chunks
    if (png != NULL)
    {
        PNG_CHUNK_PTR ptr = png->ptr;

        while (ptr != NULL)
        {
            PNG_CHUNK_PTR next = ptr->next;

            if (ptr->data != NULL)
            {
                free(ptr->data);
            }

            free(ptr);

            ptr = next;
        }                         
    }

    free(png);

    png = NULL;
}

EXPORT_IMPORT void png_init(const unsigned char* pixels, size_t height, size_t width, unsigned char bit_depth, wchar_t* filename)
{
    char mbFilename[FILENAME_MAX];
    unsigned char* type_data;
    unsigned long crc = 0;

    PIXEL pixel_black = {0x00, 0x00, 0x00/*, 0x00*/}, pixel_white = {0xff, 0xff, 0xff/*, 0x00*/};

    unsigned int j = 0;

    // Set the locale to the user's default locale
    setlocale(LC_ALL, "");

    // Convert wide-character filename to multibyte
    if (wcstombs(mbFilename, filename, sizeof(mbFilename)) == (size_t)-1)
    {
        return;
    }

    PNG_PTR png = malloc(sizeof(PNG));

    png->n = 0;
    png->ptr = NULL;
    png->signature[0] = 0x89; 
    png->signature[1] = 0x50;
    png->signature[2] = 0x4E;
    png->signature[3] = 0x47;
    png->signature[4] = 0x0D;
    png->signature[5] = 0x0A;
    png->signature[6] = 0x1A;
    png->signature[7] = 0x0A;

// ***********************************
// *    Begin creating IHDR Chunk    *
// *********************************** 
    PNG_CHUNK_PTR ihdr = malloc(sizeof(PNG_CHUNK));
    png->n = 1;
    png->ptr = ihdr;

    ihdr->next = NULL;
    ihdr->prev = NULL;

    //Set length
    big_endian_write_u32(ihdr->length, PNG_CHUNK_IDHR_LENGTH);
    // Set type/name
    ihdr->type[0] = 0x49;
    ihdr->type[1] = 0x48;
    ihdr->type[2] = 0x44;
    ihdr->type[3] = 0x52;
    // Set Data
    ihdr->data = malloc(big_endian_read_u32(ihdr->length));
    big_endian_write_u32(ihdr->data, width);
    big_endian_write_u32(ihdr->data + 4, height);    
    ihdr->data[8] = 0x08; // Bit depth /* The number of bits per sample or per palette index (not per pixel). Valid values are 1, 2, 4, 8, and 16. Not all values are allowed for all colour types. */
    ihdr->data[9] = 0x02;
    //ihdr->data[9] = 0x03;
    //ihdr->data[9]  = 0x06; // Color type, truecolour with alpha
    /*
        PNG image type          Color type      Allowed bit depths      Interpretations
        Greyscale               0               1, 2, 4, 8, 16          Each pixel is a greyscale sample
        Truecolor               2               8, 16                   Each pixel is an R,G,B triple
        Indexed-colour          3               1, 2, 4, 8              Each pixel is a palette index; a PLTE chunk shall appear.
        Greyscale with alpha    4               8, 16                   Each pixel is a greyscale sample followed by an alpha sample.
        Truecolour with alpha   6               8, 16                   Each pixel is an R,G,B triple followed by an alpha sample.
     */
    ihdr->data[10] = 0x00; // Compression method
    ihdr->data[11] = 0x00; // Filter method
    ihdr->data[12] = 0x00; // Interlace method
    // Set CRC 
    unsigned char *ihdr_type_data = malloc(sizeof(ihdr->type) + big_endian_read_u32(ihdr->length));

    for (unsigned int j = 0; j < sizeof(ihdr->type); j++)
    {            
        ihdr_type_data[j] = ihdr->type[j];
    }
    for (unsigned int j = 0; j < big_endian_read_u32(ihdr->length); j++)
    {            
        ihdr_type_data[j + sizeof(ihdr->type)] = ihdr->data[j];
    }
    crc = update_crc(0xffffffffL, ihdr_type_data, sizeof(ihdr->type) + big_endian_read_u32(ihdr->length)) ^ 0xffffffffL;    
    big_endian_write_u32(ihdr->crc, crc);

    free(ihdr_type_data);
// *********************************
// *    End creating IHDR Chunk    *
// *********************************

// ***********************************
// *    Begin creating PLTE Chunk    *
// ***********************************

    PNG_CHUNK_PTR plte = malloc(sizeof(PNG_CHUNK));
    //png->n = png->n + 1;
    ihdr->next = plte;

    plte->next = NULL;
    plte->prev = ihdr;

    // Set type/name    
    plte->type[0] = 0x50;
    plte->type[1] = 0x4C;
    plte->type[2] = 0x54;
    plte->type[3] = 0x45;

    // Set length
    big_endian_write_u32(plte->length, 0x0F);
    // Set data    
    plte->data = malloc(big_endian_read_u32(plte->length));
    plte->data[0]  = 0x00; // Black at index 0
    plte->data[1]  = 0x00;
    plte->data[2]  = 0x00;
    plte->data[3]  = 0xFF; // White at index 1
    plte->data[4]  = 0xFF; 
    plte->data[5]  = 0xFF;
    plte->data[6]  = 0xFF; // Red at index 2
    plte->data[7]  = 0x00;
    plte->data[8]  = 0x00;
    plte->data[9]  = 0x00; // Green at index 3
    plte->data[10] = 0xFF;
    plte->data[11] = 0x00;
    plte->data[12] = 0x00; // Blue at index 4
    plte->data[13] = 0x00;
    plte->data[14] = 0xFF;

    // Set CRC    
    type_data = malloc(sizeof(plte->type) + big_endian_read_u32(plte->length));
    for (unsigned int j = 0; j < sizeof(plte->type); j++)
    {            
        type_data[j] = plte->type[j];
    }
    for (unsigned int j = 0; j < big_endian_read_u32(plte->length); j++)
    {            
        type_data[j + sizeof(plte->type)] = plte->data[j];
    }
    crc = update_crc(0xffffffffL, type_data, sizeof(plte->type) + big_endian_read_u32(plte->length)) ^ 0xffffffffL;        
    big_endian_write_u32(plte->crc, crc);

    free(type_data);    
// *********************************
// *    End creating PLTE Chunk    *
// *********************************

// ***********************************
// *    Begin creating IDAT Chunk    *
// ***********************************
    PNG_CHUNK_PTR idat = malloc(sizeof(PNG_CHUNK));
    png->n = png->n + 1;
    ihdr->next = idat;
    //plte->next = idat;

    idat->next = NULL;
    //idat->prev = plte;
    idat->prev = ihdr;

    // Set type/name 
    idat->type[0] = 0x49;
    idat->type[1] = 0x44;
    idat->type[2] = 0x41;
    idat->type[3] = 0x54;

    // Set data
    //idat->data = malloc(height*width);
    idat->data = malloc(height*width*bit_depth);
    
    /*for (unsigned int i = 0; i < height*width*bit_depth; i++)
    {
        if ((i + 7) < width*bit_depth)
        {
        idat->data[i + 7] = pixels[i];
        }
        else
        {
            //idat->data[i] = pixels[i];
        }
    }*/
    
    
    /*for (unsigned int i = 0; i < height*width*bit_depth; i+=bit_depth)
    {
        idat->data[i + 0] = pixels[i + 2];
        idat->data[i + 1] = pixels[i + 1];
        idat->data[i + 2] = pixels[i + 0];
        //idat->data[i + 3] = pixels[i + 3];
    }*/
             
    for (unsigned int i = 0; i < height*width*bit_depth; i++)
    {
        idat->data[i] = pixels[i];
    }
            
    /*
    for (unsigned int r = 0; r < height; r++)
    {
        for (unsigned int c = 0; c < width; c++)
        {
            //pixels[r*width + c
            switch (((((PIXEL*)(pixels + r*width*bit_depth + c*bit_depth))->r << 16) | (((PIXEL*)(pixels + r*width*bit_depth + c*bit_depth))->g << 8) | (((PIXEL*)(pixels + r*width*bit_depth + c*bit_depth))->b << 0)))
            {
                case REAL_COLOR_BLACK:                
                    idat->data[r*width + c + 0] = PLTE_BLACK_INDEX;
                    //idat->data[r*width + c + 1] = PLTE_BLACK_INDEX;
                    //idat->data[r*width + c + 2] = PLTE_BLACK_INDEX;
                break;
                case REAL_COLOR_WHITE:                
                    idat->data[r*width + c + 0] = PLTE_WHITE_INDEX;                    
                    //idat->data[r*width + c + 1] = PLTE_WHITE_INDEX;
                    //idat->data[r*width + c + 2] = PLTE_WHITE_INDEX;
                break;
                case REAL_COLOR_RED:                
                    idat->data[r*width + c + 0] = PLTE_RED_INDEX;                    
                    //idat->data[r*width + c + 1] = PLTE_RED_INDEX;
                    //idat->data[r*width + c + 2] = PLTE_RED_INDEX;
                break;
                case REAL_COLOT_GREEN:                
                    idat->data[r*width + c + 0] = PLTE_GREEN_INDEX;                    
                    //idat->data[r*width + c + 1] = PLTE_GREEN_INDEX;
                    //idat->data[r*width + c + 2] = PLTE_GREEN_INDEX;
                break;
                case REAL_COLOR_BLUE:                
                    idat->data[r*width + c + 0] = PLTE_BLUE_INDEX;                    
                    //idat->data[r*width + c + 1] = PLTE_BLUE_INDEX;
                    //idat->data[r*width + c + 2] = PLTE_BLUE_INDEX;
                break;
            }
        }
    }
     */

    for (unsigned int i = 0; i < height*width*bit_depth; i+=bit_depth)
    {
        switch (((pixels[i + 2] << 16) | (pixels[i + 1] << 8) | pixels[i + 0]))
        {
            case REAL_COLOR_BLACK:
                
                /*idat->data[i*bit_depth + 0] = PLTE_BLACK_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_BLACK_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_BLACK_INDEX;*/
                /*idat->data[i*bit_depth + 3] = 0x00;*/
                
                //idat->data[j] = PLTE_BLACK_INDEX;
            break;
            case REAL_COLOR_WHITE:
                
                /*idat->data[i*bit_depth + 0] = PLTE_WHITE_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_WHITE_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_WHITE_INDEX;*/
                /*idat->data[i*bit_depth + 3] = 0x00;*/
                
                //idat->data[j] = PLTE_WHITE_INDEX;
            break;
            case REAL_COLOR_RED:
                
                /*idat->data[i*bit_depth + 0] = PLTE_RED_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_RED_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_RED_INDEX;*/
                /*idat->data[i*bit_depth + 3] = 0x00;*/
                
                //idat->data[j] = PLTE_RED_INDEX;
            break;
            case REAL_COLOT_GREEN:
                
                /*idat->data[i*bit_depth + 0] = PLTE_GREEN_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_GREEN_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_GREEN_INDEX;*/
                /*idat->data[i*bit_depth + 3] = 0x00;*/
                
                //idat->data[j] = PLTE_GREEN_INDEX;
            break;
            case REAL_COLOR_BLUE:
                
                /*idat->data[i*bit_depth + 0] = PLTE_BLUE_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_BLUE_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_BLUE_INDEX;*/
                /*idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[j] = PLTE_BLUE_INDEX;
            break;
        }

        j++;
    }
    for (unsigned int i = 0; i < height*width; i++)
    {
        /*
        // White
        if (((PIXEL*)(pixels + i*bit_depth))->r == 0xff && ((PIXEL*)(pixels + i*bit_depth))->g == 0xff && ((PIXEL*)(pixels + i*bit_depth))->b == 0xff)
        {
            idat->data[i] = 1;
        }
        // Black
        else if (((PIXEL*)(pixels + i*bit_depth))->r == 0x00 && ((PIXEL*)(pixels + i*bit_depth))->g == 0x00 && ((PIXEL*)(pixels + i*bit_depth))->b == 0x00)
        {
            idat->data[i] = 0;
        }
         */
        
        switch (((((PIXEL*)(pixels + i*bit_depth))->r << 16) | (((PIXEL*)(pixels + i*bit_depth))->g << 8) | (((PIXEL*)(pixels + i*bit_depth))->b << 0)))
        {
            case REAL_COLOR_BLACK:
                
                /*
                idat->data[i*bit_depth + 0] = PLTE_BLACK_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_BLACK_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_BLACK_INDEX;
                idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[i] = PLTE_BLACK_INDEX;
            break;
            case REAL_COLOR_WHITE:
                
                /*
                idat->data[i*bit_depth + 0] = PLTE_WHITE_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_WHITE_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_WHITE_INDEX;
                idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[i] = PLTE_WHITE_INDEX;
            break;
            case REAL_COLOR_RED:
                
                /*
                idat->data[i*bit_depth + 0] = PLTE_RED_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_RED_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_RED_INDEX;
                idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[i] = PLTE_RED_INDEX;
            break;
            case REAL_COLOT_GREEN:
                
                /*
                idat->data[i*bit_depth + 0] = PLTE_GREEN_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_GREEN_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_GREEN_INDEX;
                idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[i] = PLTE_GREEN_INDEX;                
            break;
            case REAL_COLOR_BLUE:
                
                /*
                idat->data[i*bit_depth + 0] = PLTE_BLUE_INDEX;
                idat->data[i*bit_depth + 1] = PLTE_BLUE_INDEX;
                idat->data[i*bit_depth + 2] = PLTE_BLUE_INDEX;
                idat->data[i*bit_depth + 3] = 0x00;
                 */
                //idat->data[i] = PLTE_BLUE_INDEX;
            break;
        }
    }

    // Deflate data 
    //DEFLATED_DATA_PTR deflated = de_flate(idat->data, height*width);
    DEFLATED_DATA_PTR deflated = de_flate(idat->data, height*width*bit_depth);
    //DEFLATED_DATA_PTR deflated = de_flate(pixels, height*width*bit_depth);
    free(idat->data);
    idat->data = NULL;
    // Set to de-flatted data 
    idat->data = deflated->data;    
    big_endian_write_u32(idat->length, deflated->size);
    type_data = malloc(sizeof(idat->type) + big_endian_read_u32(idat->length));
    for (unsigned int j = 0; j < sizeof(idat->type); j++)
    {            
        type_data[j] = idat->type[j];
    }
    for (unsigned int j = 0; j < big_endian_read_u32(idat->length); j++)
    {            
        type_data[j + sizeof(idat->type)] = idat->data[j];
    }
    crc = update_crc(0xffffffffL, type_data, sizeof(idat->type) + big_endian_read_u32(idat->length)) ^ 0xffffffffL;
    big_endian_write_u32(idat->crc, crc);
    free(type_data);  
// *********************************
// *    End creating IDAT Chunk    *
// *********************************

// ***********************************
// *    Begin creating IEND Chunk    *
// ***********************************
    PNG_CHUNK_PTR iend = malloc(sizeof(PNG_CHUNK));
    png->n = png->n + 1;
    idat->next = iend;

    iend->next = NULL;
    iend->prev = idat;

    //Set length
    big_endian_write_u32(iend->length, 0x00);    
    // Set type/name 
    iend->type[0] = 0x49;
    iend->type[1] = 0x45;
    iend->type[2] = 0x4E;
    iend->type[3] = 0x44;
    // No data
    iend->data = NULL;
    // Set CRC
    crc = update_crc(0xffffffffL, iend->type, sizeof(iend->type) + big_endian_read_u32(iend->length)) ^ 0xffffffffL;
    big_endian_write_u32(iend->crc, crc);    
// *********************************
// *    End creating IEND Chunk    *
// *********************************
    
    create_png_file(png, mbFilename);

    png_dealloc(png);        
}
