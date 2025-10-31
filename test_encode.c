/*
NAME : Azif Nizam
BATCH : 25021A
REG NO : 25021A_008 
DATE : 28-10-2025

PROJECT NAME : LSB IMAGE STEGNOGRAPHY
DESCRIPTION : A command-line Image Stegnography Program that allows the user to encode secre-
              t message from a text file to a .bmp file with a password(magic string). The 
              user can identify the encoded file with the matching password(magic string) 
              for decoding. The decoded output is produced in a text file. The program is 
              modularized into multiple files for better organization and maintainability.

              -CMD Arguments- 
              To Encode : ./a.out -e filename.bmp filename.txt stego.bmp
              To Decode : ./a.out -d filename.bmp filename.txt
*/

#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if (check_operation_type(argv) == e_encode)                                 //to check the mode of operation
    {
        printf("## You have chosed encoding\n");
        EncodeInfo encInfo;                                                     //encode structure variable
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)          //to check the input arguments for encoding
        {
            printf("\n## Enter the magic string to encode (max 10 characters will be taken) : "); 
            scanf("%10s",encInfo.password);                                     //to read password (magic string)
            printf("\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("## Encoding success \n");
            }
            else
            {
                printf(">> Encoding failed \n");
                return e_failure ;
            }
        }
        else
        {
            printf(">> Error : Failed to validate arguments \n");
            return e_failure ;
        }
    }
    else if(check_operation_type(argv) == e_decode)                            // if decoding
    {
        printf("## You have chosed decoding \n");
        DecodeInfo dcInfo ;                                                    //decode structure variable
        if(read_and_validate_decode_args(argv, &dcInfo) == e_success)          //to check the input arguments for decoding
        {
            if(do_decoding(&dcInfo) == e_success)
            {
                printf("## Decoding success \n");
            }
            else
            {
                printf(">> Decoding failed \n");
                return e_failure ;
            }
        }
        else
        {
            printf(">> Error : Failed to validate arguments \n");
            return e_failure ;
        }
            
    }
    else                                                                          //if wrong arguments
    {
        printf(">> Error : Missing/Wrong Arguments \n");
        printf("## Argument format :-\n");
        printf("## Encoding : ./a.out -e your_image_file.bmp your_message_file.txt \n");
        printf("## Decoding : ./a.out -d your_encoded_file.bmp \n");

        return e_failure ;
    }

    return e_success ;
}

OperationType check_operation_type(char *argv[])                               //to check whether the encoding or decoding             
{
    if(argv[0] == NULL || argv[1] == NULL )
    {
        return e_unsupported ;
    }

    if(strcmp(argv[1],"-e") == 0)                                               // '-e' for encoding
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)                                          // '-d' for decoding
    {
        return e_decode;
    }
    else
    {
        return e_unsupported ;
    }
}
