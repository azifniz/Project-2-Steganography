
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "decode.h"

//to validate the input arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *dcInfo)          
{
    if(argv[2] == NULL)                                                          //stego file
    {
        printf(">> Error : encoded file is not present\n");
        return e_failure ;
    }
    else if(strstr(argv[2],".bmp") != NULL)
    {
        dcInfo -> stego_image_fname = argv[2];
    }
    else
    {
        printf(">> Error : Invalid file. Argument 2 must be a .bmp file\n");
        return e_failure;
    }

    if(argv[3] == NULL)                                                          //outputfile name
    {
        printf("## Creating output file with name 'output' \n");
        static char dest_name[100];                                             //temp buffer
        strcpy(dest_name, "output");
        dcInfo->dest_image_fname = dest_name ;                         
    }
    else if (strstr(argv[3], ".txt") != NULL)                                   //for outputfile name
    {
        static char dest_name[100];                                             //temp buffer
        strcpy(dest_name, argv[3]);
        char *token = strtok(dest_name, ".");                                   //splitting till '.' is encountered
        dcInfo->dest_image_fname = dest_name;                                   //storing the first token
    }
    else
    {
        printf(">> Error : Invalid file. Argument 3 must be a .txt file\n");
        return e_failure;
    }

    return e_success ;
}

//decoding function
Status do_decoding(DecodeInfo *dcInfo)                
{
    if(open_decode_files(dcInfo) != e_success)                                  //to open files
    {
        printf(">> Error : Failed to open files \n");
        return e_failure ;
    }
    else
    {
        printf("## All files opened -> success\n");
    }

    printf("\n## Enter the magic string to decode : ");
    scanf("%10s",dcInfo->password);                                     //to read password (magic string) 
    printf("\n");

    if(decode_magic_string(dcInfo->password, dcInfo) != e_success)              //to decode the magic string
    {
        //printf(">> Error : Failed to decode magic string \n");
        fclose(dcInfo->fptr_stego_image);                                       //closing the file if magic string fails
        return e_failure ;
    }

    if(decode_secret_file_extn(dcInfo) != e_success)                            //to decode the size and the extention data
    {
        printf(">> Error : Failed to decode secret file extention \n");
        return e_failure;
    }
    else
    {
        printf("## Decode secret file extention -> success\n");
    }

    if(decode_image_to_data(dcInfo) != e_success)                               //to decode the size and secret message
    {
        printf(">> Error : Failed to decode image to data \n");
        return e_failure ;
    }
    else
    {
        printf("## Decode message -> success\n");
    }

    if (dcInfo->fptr_dest_image)                                                //closing the files
        fclose(dcInfo->fptr_dest_image);

    if (dcInfo->fptr_stego_image)
        fclose(dcInfo->fptr_stego_image);

    return e_success ;
}

//to open files
Status open_decode_files(DecodeInfo *dcInfo)            
{
    dcInfo -> fptr_stego_image = fopen(dcInfo -> stego_image_fname, "rb");       //opening source (stego) file
    if (dcInfo -> fptr_stego_image == NULL)
    {
        printf(">> Error : Failed to open source bmp file \n");
        return e_failure;
    }
   
    return e_success ;
}

//to decode the magic string
Status decode_magic_string(char *magic_string, DecodeInfo *dcInfo)    
{
    fseek(dcInfo->fptr_stego_image, 54, SEEK_SET);                              //setting the pointer to 54th byte

    //to decode size of magic string
    char magic_buff[32];                                                        //temp buffer
    fread(magic_buff, 32, 1, dcInfo->fptr_stego_image);
    long len_magic;                                                             //to store the size of magic string
    decode_size_to_lsb(&len_magic, magic_buff);

    //to decode magic string
    char magic_array[len_magic + 1] ;                                           //to store magic string
    char arr_magic[8] ;                                                         //temp buffer
    for(int i=0; i<len_magic; i++)
    {
        fread(arr_magic, 8, 1, dcInfo->fptr_stego_image);
        decode_byte_to_lsb(&magic_array[i], arr_magic);
        
    }

    magic_array[len_magic] = '\0';                                              //adding null because strcmp compares till null

    if (strcmp(magic_array, magic_string) != 0)                                 //verifying magicstring
    {
        printf(">> Error : Magic string mismatch\n");
        return e_failure;
    }

    return e_success;
}

//to decode the characters from lsb
Status decode_byte_to_lsb(char *data, char *image_buffer)                  
{
    *data = 0;
    for(int i =0; i<8; i++)
    {
        *data = *data & (~(1<<i));                                             // clearing the bits 
        *data = *data | (((image_buffer[i] >> 0) & 1) << i) ;                  // adding the lsb bit of data to the ith position of decoded message
    }

    return e_success ;
}

//to decode the size
Status decode_size_to_lsb(long *size, char *array)                        
{
    for(int i =0; i<32; i++)
    {
        *size = *size & (~(1<<i));                                             // clearing the bits 
        *size = *size | (((array[i] >> 0) & 1) << i) ;                         // adding the get bits to the ith position of decoded message
    }

    return e_success ;
}

//to decode the size of extention and extention
Status decode_secret_file_extn(DecodeInfo *dcInfo)          
{
    int i;
    char arr_extn_size[32];                                                     //temp buffer
    fread(arr_extn_size, 32, 1, dcInfo->fptr_stego_image);
    long size_extn;                                                             //to store the size of extention
    decode_size_to_lsb(&size_extn, arr_extn_size);

    char extn[size_extn];                                                       //to store the decoded extention

    char arr_extn_data[8];                                                      //temp buffer
    for (i=0; i<size_extn; i++)
    {
        fread(arr_extn_data, 8, 1, dcInfo->fptr_stego_image);
        decode_byte_to_lsb(&extn[i], arr_extn_data);
    }
    
    extn[i] = '\0';
    strcat(dcInfo->dest_image_fname, extn);                                     //concatinating filename and extention

    dcInfo->fptr_dest_image = fopen(dcInfo->dest_image_fname, "wb");            //opening the output text file
    
    return e_success ;

}

//to decode the size of secret message and secret message
Status decode_image_to_data(DecodeInfo *dcInfo)             
{
    char arr_data_size[32];                                                     //temp buffer
    fread(arr_data_size, 32, 1, dcInfo->fptr_stego_image);
    long size_data ;                                                            //to store the size of data
    decode_size_to_lsb(&size_data, arr_data_size);     

    char data[size_data];                                                       //to store the data

    char arr_data[8];                                                           //temp buffer
    for (int i=0; i<size_data; i++)
    {
        fread(arr_data, 8, 1, dcInfo->fptr_stego_image);
        decode_byte_to_lsb(&data[i], arr_data);          
    }

    for(int i = 0; i<size_data; i++ )                                           //loop to print the message to file
    {
        fputc(data[i],dcInfo->fptr_dest_image);
    }

    return e_success ;
}

