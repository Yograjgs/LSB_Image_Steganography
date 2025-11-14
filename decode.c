/*Project : Steganography
  Name    : YOGRAJ G S
  ID      : 25026_259*/


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char decode_byte_from_lsb(FILE *fptr_stego_image);
#include "types.h"
#include "decode.h"

//To check whether the file stego.bmp and output.txt present or not
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2], ".bmp") != NULL) //Checks for stego.bmp.bmp
    {
        printf(".bmp is present\n");
       decInfo ->stego_image_fname= argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }

 if(strstr(argv[3], ".txt") != NULL)     //Checks for sevret.txt
    {
        printf("output.txt is present\n");
       decInfo -> output_fname = argv[3];
    }
    else
    {
       decInfo -> output_fname = "output.txt";
    }
    return e_success;

}

Status do_decoding(DecodeInfo *decInfo)    //Decoding Starts here
{
    char user_magic[256];    //Receiver Entered magic string is stored here  
    char decoded_magic[256];  // Decoded magic string from the image is stored here

    if (open_file(decInfo) != e_success)  
    {
        return e_failure;
    }

    if (skip_header(decInfo) != e_success)  //Skips the header since its not required
    {
        return e_failure;
    }
   
    printf("\nEnter the magic string used during encoding: "); //Taking magic string from the receiver
    if (scanf(" %[^\n]", user_magic) != 1) 
    {
        printf("Failed to read magic string from user.\n");
        return e_failure;
    }

    if (decode_magic_string(decInfo, user_magic, decoded_magic) != e_success)  // The encoded and Decoded magic string is compared
    {
        printf("Magic string mismatch! Decoding aborted.\n");
        return e_failure;
    }
    printf("Magic string verified successfully!\n");

    //It extracts the secret file extention
    if (decode_size_from_lsb(&decInfo->extn_size, decInfo) != e_success)
    {
        return e_failure;
    }

    //To decode the secret file extention size in bytes
    if (decode_secret_file_extn(decInfo) != e_success)
    {
        return e_failure;
    }

    //Decodes actual extention in string ( ex : .txt, .bmp)
    if (decode_secret_file_size(decInfo) != e_success)
    {
        return e_failure;
    }

    //To decode the secret file data
    if (decode_secret_file_data(decInfo) != e_success)
    {
        return e_failure;
    }
    printf("\nSecret file data decoded successfully!\n");

    printf("\nDecoded secret message:\n");                //Secret message is obtained in the terminal here
    fwrite(decInfo->secret_data, 1, decInfo->size_secret_file, stdout);
    printf("\n");

    if (decode_write_secret_file(decInfo) != e_success)         //Secret message is copied to the output.txt 
    {
        printf("Failed to write secret data to file.\n");
        return e_failure;
    }

    return e_success;
}

//Checks wheather the files are opening or not 
Status open_file(DecodeInfo *decInfo)
{
    // Opens in read in binary mode
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Stego file not found.\n");
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "w"); // opens output in write mode
    if (decInfo->fptr_output == NULL)
    {
        printf("Unable to create output file.\n");
        return e_failure;
    }

    return e_success;
}

  
//Skipping header since nothing is encoded inside header
Status skip_header(DecodeInfo *decInfo)
{
    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
    {
        return e_failure;
    }
    return e_success;
}

//Decoding of magic string is done here
Status decode_magic_string(DecodeInfo *decInfo, const char *user_magic, char *decoded_magic_out)
{
    size_t len = strlen(user_magic);  // length of the receiver entered magic string is found here
    if (len == 0 || len > 255) return e_failure; /* sanity */

    for (size_t i = 0; i < len; i++)
    {
        int c = decode_byte_from_lsb(decInfo->fptr_stego_image);
        if (c == EOF) return e_failure;
        decoded_magic_out[i] = (char)c;
    }
    decoded_magic_out[len] = '\0';

    if (strcmp(decoded_magic_out, user_magic) == 0)     //user entered and encoded magic string is compared here
        return e_success;
    else
        return e_failure;
}



// To extract  hidden character one by one from the stego image
char decode_byte_from_lsb(FILE *fptr_stego_image)
{
    unsigned char buffer[8];
    unsigned char ch = 0;
    size_t got = fread(buffer, 1, 8, fptr_stego_image); //To read 8 bytes from the image file and store them in array buffer.
    if (got != 8) 
    {
        // On error or EOF, return 0xFF sentinel (caller should check) 
        return (char)EOF;
    }

    for (int i = 0; i < 8; i++)
    {
        ch |= ((buffer[i] & 1) << i);  // LSB-first
    }

    return (char)ch;
}


//Decodes the extention size of the file (in bytes)
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
    unsigned char buffer[32];
    unsigned int decoded_size = 0;

    //To chech whether exactly 32 bits were read from stego image. 
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32) 
    {
        printf("Error reading size bits from stego image\n");
        return e_failure;
    }

    for (int i = 0; i < 32; i++)
    {
        decoded_size |= ((buffer[i] & 1) << i); // To extract 1 bit from each byte of image
    }

    *size = (int)decoded_size;
    return e_success;
}

//It decodes the extention string(ex: .txt,.bmp etc)
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("Decoding secret file extension...\n");

    //To check whether the decoded extension valid or not
    if (decInfo->extn_size <= 0 || decInfo->extn_size > 50) 
    {
        printf("Invalid extension size: %d\n", decInfo->extn_size);
        return e_failure;
    }

    for (int i = 0; i < decInfo->extn_size; i++)
    {
        char ch = decode_byte_from_lsb(decInfo->fptr_stego_image);
        decInfo->extn[i] = ch;
    }

    decInfo->extn[decInfo->extn_size] = '\0';  

    printf("Decoded extension: %s\n", decInfo->extn);
    return e_success;
}



//Decodes the size of the secret file from the image
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buffer[32];
    unsigned int size = 0;

    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
        return e_failure;

    for (int i = 0; i < 32; i++)
    {
        size |= ((buffer[i] & 1) << i);  
    }

    decInfo->size_secret_file = (long)size;
    return e_success;
}


//To Decode the secret file data (in strings)
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    if (decInfo->size_secret_file <= 0)
    {
        printf("Invalid secret file size!\n");
        return e_failure;
    }

    //This line dynamically allocates memory to store the decoded secret file data
    decInfo->secret_data = malloc(decInfo->size_secret_file + 1);
    if (decInfo->secret_data == NULL)
    {
        printf("Memory allocation failed for secret data!\n");
        return e_failure;
    }

    printf("Decoding secret file data...\n");

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        decInfo->secret_data[i] = decode_byte_from_lsb(decInfo->fptr_stego_image);
    }

    decInfo->secret_data[decInfo->size_secret_file] = '\0';

    return e_success;
}


//To display the secret message in the terminal and store it in output.txt
Status decode_write_secret_file(DecodeInfo *decInfo)
{
    if (!decInfo || !decInfo->secret_data || decInfo->size_secret_file <= 0)
    {
        return e_failure;
    }

    FILE *fptr_output = fopen(decInfo->output_fname, "wb"); 
    if (!fptr_output)
    {
        return e_failure;
    }

    //This line writes the secret decoded data into output file
    size_t written = fwrite(decInfo->secret_data, 1, decInfo->size_secret_file, fptr_output);
    fclose(fptr_output);

    free(decInfo->secret_data);
    decInfo->secret_data = NULL;

    return (written == (size_t)decInfo->size_secret_file) ? e_success : e_failure;
}






