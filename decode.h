#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    // destination Image info  (.txt file)
    char *dest_image_fname;
    FILE *fptr_dest_image;
   
    
    // Stego Image Info 
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char password[11];

} DecodeInfo;



// Read and validate Encode args from argv 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *dcInfo);

// Perform the encoding 
Status do_decoding(DecodeInfo *dcInfo);

// Get File pointers for i/p and o/p files 
Status open_decode_files(DecodeInfo *dcInfo);

// Store Magic String 
Status decode_magic_string(char *magic_string, DecodeInfo *dcInfo);

// Encode secret file extenstion 
Status decode_secret_file_extn(DecodeInfo *dcInfo);  //decode size and extention

// Encode function, which does the real encoding 
Status decode_image_to_data(DecodeInfo *dcInfo); //decode size and extention

// Encode a byte into LSB of image data array 
Status decode_byte_to_lsb(char *data, char *image_buffer);

Status decode_size_to_lsb(long *size, char *array);

#endif


