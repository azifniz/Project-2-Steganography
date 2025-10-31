#include <stdio.h>
#include <string.h>
#include "types.h"
#include "encode.h"

//to read and validate the user input arguments in  the command line
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)         
{
    if(argv[2] == NULL)                                          //for source file
    {
        printf(">> Error : bmp image file and text message file is not present\n");
        return e_failure ;
    }
    else if(strstr(argv[2],".bmp") != NULL)
    {
        encInfo -> src_image_fname = argv[2];                                   //now encInfo -> src_image_fname will be pointing to same as argv[2]
    }
    else
    {
        printf(">> Error : Invalid file. Argument 2 must be a .bmp file\n");
        return e_failure;
    }

    if(argv[3] == NULL)                                          //for source file
    {
        printf(">> Error : txt message file is not present\n");
        return e_failure ;
    }
    else if(strstr(argv[3],".txt") != NULL)                                          //for secret file
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf(">> Error : Invalid file. Argument 3 must be a .txt file\n");
        return e_failure;
    }

    if(argv[4] == NULL)                                                           //for destination file
    {
        printf("## Creating output (.bmp) file with name 'stego'\n");
        encInfo -> stego_image_fname = "stego.bmp" ;                              //storing a string literal to create file
    }
    else if (strstr(argv[4],".bmp") != NULL)
    {
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        printf(">> Error : Invalid file. Argument 4 must be a .bmp file \n");
        return e_failure;
    }

    return e_success ;
}

//actual encoding of data
Status do_encoding(EncodeInfo *encInfo)                                         
{
    if(open_files(encInfo) != e_success)                                        //to open all the files
    {
        printf(">> Error : Failed to open files  \n");
        return e_failure ;
    }
    else
    {
        printf("## All files opened -> success\n");
    }

    if(check_capacity(encInfo) != e_success)                            //to check capacity of destination file (stego.bmp)
    {
        printf(">> Error : Capacity check failed \n");
        return e_failure ;
    }
    else
    {
        printf("## Capacity check -> success\n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)  //to copy the header file of image
    {
        printf(">> Error : Failed to copy bmp file header \n");
        return e_failure ;
    }
    else
    {
        printf("## Image header copy -> success\n");
    }

    if(encode_magic_string(encInfo->password, encInfo) != e_success)                //to encode the magic string
    {
        printf(">> Error : Failed to encode magic string \n");
        return e_failure ;
    }
    else
    {
        printf("## Encode magic string -> success\n");
    }

    if(encode_secret_file_extn(strstr(encInfo->secret_fname,".txt"), encInfo) != e_success)  //to encode the size and type of extention 
    {
        printf(">> Error : Failed to encode secret file extention \n");
        return e_failure;
    }
    else
    {
        printf("## Encode message file extention -> success\n");
    }
 
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)  //to encode the size of secret data
    {
        printf(">> Error : Failed to encode secret file size \n");
        return e_failure ;
    }
    else
    {
        printf("## Encode message size -> success\n");
    }

    if(encode_secret_file_data(encInfo) != e_success)                             //to encode the secret data
    {
        printf(">> Error : Failed to encode secret file data \n");
        return e_failure ;
    }
    else
    {
        printf("## Encode message -> success\n");
    }
 
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)  //to copy the remaining bytes of image
    {
        printf(">> Error : Failed to copy the remaining data \n");
        return e_failure ;
    }
    else
    {
        printf("## Copy remainig image data -> success\n");
    }
 
    fclose(encInfo->fptr_src_image);                                             //closing all the files
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}

//to open all the files
Status open_files(EncodeInfo *encInfo)                     
{
    encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "rb");        //to open source file (beautiful.bmp)
    if (encInfo -> fptr_src_image == NULL)
    {
        printf(">> Error : Failed to open source .bmp file \n");
        return e_failure;
    }

    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "rb");              //to open secret file (secret.txt)
    if (encInfo -> fptr_secret == NULL)
    {
        printf(">> Error : Failed to open .txt file\n");
        return e_failure;
    }
    
    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "wb");    //to open the stego file (stego.bmp)
    if (encInfo -> fptr_stego_image == NULL)
    {
        printf(">> Error : Failed to open stego.bmp file\n");
        return e_failure;
    }
    return e_success ;   
}

//to check the capacity of the stego file 
Status check_capacity(EncodeInfo *encInfo)                  
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  //to find height and width             
    if (encInfo -> image_capacity == 0)  
    {
        printf(">> Error : Failed to fetch image size \n");
        return e_failure ;
    }
    
    encInfo -> size_secret_file = get_file_size(encInfo->fptr_secret);            //to get the size of scret file                 
    if (encInfo -> image_capacity == 0)    
    {
        printf(">> Error : Failed to fetch secret file size \n");
        return e_failure ;
    }
    
    if (encInfo->image_capacity > (54 + (4+(strlen(encInfo->password))+4+4+4+(encInfo->size_secret_file))*8))  //checking the image capacity
    {
        return e_success ;
    }
    else
    {
        printf(">> Error : Failed Image capacity check \n");
        return e_failure ;
    }
}

//to get the the width and height of .bmp file
uint get_image_size_for_bmp(FILE *fptr_image)                   
{
    uint width , height;
    fseek(fptr_image, 18, SEEK_SET);                                         //width from 18th byte of bmp height from 22nd byte
    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);

    return 3*width*height ;
}

//to get the size of secret message
uint get_file_size(FILE *fptr)                                                  
{
    long size;
    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);                                                      //ftell() returns the current position of the file pointer (in bytes from the start).

    return size;
}

//to get the header bytes of bmp file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)             
{
    char header[54];                                                         //temp buffer

    fseek(fptr_src_image, 0, SEEK_SET);
    fread(header, 54, 1, fptr_src_image);                                    //first 54 bytes are header of bmp file
    fwrite(header, 54, 1, fptr_dest_image);

    return e_success ;
}

//to hide the magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)       
{
    long size_magicstring = strlen(magic_string);

    char arr_size[32];                                                        //temp buffer
    fread(arr_size, 32, 1, encInfo->fptr_src_image);

    //to encode the size 
    if(encode_size_to_lsb(size_magicstring, arr_size) != e_success)
    {
        printf(">> Error : Failed to encode size to lsb \n");
        return e_failure ;
    }
    fwrite(arr_size, 32, 1,  encInfo->fptr_stego_image);


    //to hide the characters
    if(encode_data_to_image((char *)magic_string, (int)size_magicstring, encInfo->fptr_src_image , encInfo->fptr_stego_image) != e_success)
    {
        printf(">> Error : Failed to encode data to image\n");
        return e_failure;  
    }
    return e_success;
}

//hides the characters 
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)  
{
    char arr_byte[8];                                                                   //temp buffer
    for (int i=0; i<size; i++)
    {
        fread(arr_byte, 8, 1, fptr_src_image);
        //to encode characters to LSB position
        if(encode_byte_to_lsb(data[i] , arr_byte) != e_success)         
        {
            printf(">> Error : Failed to encode byte to lsb \n");
            return e_failure ;
        }
        fwrite(arr_byte, 8, 1, fptr_stego_image);   
    }

    return e_success ;
}

//to encode characters to LSB position
Status encode_byte_to_lsb(char data, char *image_buffer)     
{
    for(int i=0; i<8; i++)
    {
        image_buffer[i] = image_buffer[i] & (~(1<<0)) ;                 // to clear the lsb bit of  each elements 
        image_buffer[i] = image_buffer[i] | ((data >> i) & 1) ;         // replacing the LSB bit with 'i'th bit of data    
    }
    return e_success ;
}

//to hide the size of extention and data of extention
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) 
{
    long size_extn = strlen(file_extn);
    char arr_size[32];                                                    //temp buffer
    fread(arr_size, 32, 1, encInfo->fptr_src_image);

    //to encode the size 
    if(encode_size_to_lsb(size_extn, arr_size) != e_success)
    {
        printf(">> Error : Failed to encode size to lsb \n");
        return e_failure ;
    }
    fwrite(arr_size, 32, 1,  encInfo->fptr_stego_image);
    
    encode_data_to_image((char *)file_extn, size_extn, encInfo->fptr_src_image , encInfo->fptr_stego_image); //to hide data
    return e_success ;
    
}

//to encode size to the LSB bit 
Status encode_size_to_lsb(long size, char *array)      
{
    for(int i=0; i<32; i++)    
    {
        array[i] = array[i] & (~(1<<0)) ;                               // to clear the lsb bit of  each elements
        array[i] = array[i] | ((size >> i) & 1) ;                       // replacing the LSB bit with ith bit of data    
    }

    return e_success ;
}

//to encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)     
{
    char arr_secret_size[32];                                             // temp buffer
    fread(arr_secret_size, 32, 1, encInfo->fptr_src_image);
    //to encode the size of secret file 
    encode_size_to_lsb(file_size, arr_secret_size);
    fwrite(arr_secret_size, 32, 1,  encInfo->fptr_stego_image);

    return e_success ;
}

//to encode the data (message)
Status encode_secret_file_data(EncodeInfo *encInfo)             
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char data;                                                            // temp buffer
    while (fread(&data, 1, 1, encInfo->fptr_secret) == 1)                 // reads the secret message
    {
        encode_data_to_image(&data, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }
    return e_success;
}

//to copy the remaining bytes of data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)             
{
    char rem_data;                                                        // temp buffer

    // Read remaining bytes from current position till EOF
    while (fread(&rem_data, 1, 1, fptr_src) == 1)
    {
        fwrite(&rem_data, 1, 1, fptr_dest);
    }

    return e_success;
}











