/*Project : Steganography
  Name    : YOGRAJ G S
  ID      : 25026_259*/

  
#include<stdio.h>
#include<string.h>
#include "types.h"
#include "encode.h"

//To check whether the file beautiful.bmp ,stego.bmp and secret.txt present or not
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strstr(argv[2], ".bmp") != NULL) //checks beautiful.bmp
    {
        printf(".bmp is present\n");
       encInfo -> src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }

    if(strstr(argv[3], ".txt") != NULL)//checks secret.txt
    {
        printf(".txt is present\n");
       encInfo -> secret_fname = argv[3];
    }
    else
    {
        printf(".txt is not present\n");
        return e_failure;
    }

 if(strstr(argv[4], ".bmp") != NULL) //Checks stego.bmp
    {
        printf("stego.bmp is present\n");
       encInfo -> stego_image_fname = argv[4];
    }
    else
    {
       encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;

}


//Encoding starts here
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)     //checks files are opening or not
    {
        printf("All the files are opened successfully\n");
    }
    else
    {
        printf("Files are not opened\n");
        return e_failure;
    }

    /*Checks if the capacity of the stego image is greater than total capcity of extention ,magic string 
      data size*/
    
    if(check_capacity(encInfo) == e_success)
    {  
        printf("Check capacity is Successfully\n");
    }
    else
    {
         printf("Check capacity is Unsuccessfully\n");
         return e_failure;   
    }

    //Copy bmp header to stego image
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("Header copied Successfully\n");
    }
    else
    {
        printf("Header Does Not Copied Successfully\n");
        return e_failure;
    }

    //Encoding of magic string takes place
    if(encode_magic_string(encInfo) == e_success)
    {
        printf("Magic string Encoded Successfully\n");
    }
    else
    {
        printf("Magic string not Encoded.\n");
        return e_failure;
    }

    //To find the length of the secret file
    int size = strlen(strchr(encInfo-> secret_fname,'.'));
   
    //It encode the secret file extention size
    if(encode_size_to_lsb(size,encInfo) == e_success)
    {
        printf("Size of extension is encoded Successfully\n");
    }
    else
    {
         printf("Size of extension is not encoded\n");
         return e_failure;
    }

    //To encode the secret file extention size in bytes
    if(encode_secret_file_extn(strchr(encInfo-> secret_fname,'.'),encInfo) == e_success)
    {
        printf("Secret file Extension is encoded Successfully\n");
    }
    else
    {
         printf("Secret file Extension encoding is Unsuccessfull\n");
         return e_failure;
    }

    //Encodes actual extention in string ( ex : .txt, .bmp)
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
    {
        printf("Secret file size is encoded Successfully\n");

    }
    else
    {
        printf("Secret file size encoding is Unsuccessfull\n");
    }

    //To Encode the secret file data
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data is encoded Successfully\n");
    }
    else
    {
         printf("Secret file data encoding is Unsuccessfull\n");
         return e_failure;
    }

    //To copy remaining data to the stego image
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("Remaining data has been copied\n");
    }
    else
    {
         printf("Remaining data not copied\n");
         return e_failure;
    }

    return e_success;
}

//Checks wheather the files are opening or not 
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"r");//opens beautiful.bmp in read mode
    if(encInfo->fptr_src_image == NULL)
    {
        printf("Source file is not present\n");
        return e_failure;
    }
     encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");//opens secret.txt in read mode
    if(encInfo->fptr_secret == NULL)
    {
        printf("Secret file is not present\n");
        return e_failure;
    }
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname,"w");//opens stego.bmp in write mode
    return e_success;
}

/*Checks if the capacity of the stego image is greater than total capcity of extention ,magic string 
      data size*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo ->fptr_src_image);
  
    encInfo -> size_secret_file  = get_file_size(encInfo->fptr_secret);

    printf("Enter the magic string : "); //User enters the magic code here 
    scanf("%[^\n]",encInfo->magic);

   if(encInfo->image_capacity > (strlen(encInfo->magic) * 8 + 32 + 32 + 32 + encInfo -> size_secret_file * 8))
   {
        return e_success;
   }
   else
   {
        printf("Image capacity is not Enough\n");
        return e_failure;
   }

}

//To get secret.txt file size
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0,SEEK_END);
    return ftell(fptr);
}

//To get beautiful.bmp file size
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width,height;
    fseek(fptr_image,18,SEEK_SET);
    fread(&width, 4, 1, fptr_image);
    fread(&height, 4, 1, fptr_image);

    return width * height * 3;

}

//Copy bmp header to Stego image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image); //comes back to the starting

    char buffer[54];

    fread(buffer, 54, 1, fptr_src_image);  //Reads starting 54 byes data from sourse image
    fwrite(buffer, 54, 1, fptr_dest_image); //Writes read data to Stego image
    return e_success;
}

//Encoding of Magic string is done here
Status encode_magic_string(EncodeInfo *encInfo)
{
    if(encode_data_to_image(encInfo->magic,strlen(encInfo->magic),encInfo) == e_success)
    {
        printf("Magic string has been Encoded\n");
    }
    else
    {
        printf("Magic string has been failed to Encode\n");
        return e_failure;
    }
    return e_success;
}

//
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{   
    for(int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, 8, 1, encInfo -> fptr_src_image); //reads 8 bit from source image
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, encInfo-> fptr_stego_image);//writes 8 bit data to stego immage

    }
  
    return e_success;
}

// To Encode hidden character one by one from the stego image
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & (1 << i)) >> i); //stores bits to buffer
    }
   return e_success;
}

//Encodes the extention size of the file (in bytes)
Status encode_size_to_lsb(int size,EncodeInfo *encInfo)
{
    char buffer[32];
     fread(buffer, 32, 1, encInfo -> fptr_src_image);
     for(int i = 0; i < 32; i++)
     {
        buffer[i] = (buffer[i] & 0xFE) | ((size & (1 << i)) >> i);//stores encoded bits to buffer
     }
        fwrite(buffer, 32, 1,encInfo->fptr_stego_image);

     return e_success;
}

//It Encodes the extention string(ex: .txt,.bmp etc)
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo) == e_success)
    {
        printf("Successfull\n");
        return e_success;
    }
     else
    {
        printf("Unsuccessfull\n");
        return e_failure;
    }
return e_success;
}

//Encodes the size of the secret file from the image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

//To Encode the secret file data (in strings)
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);

    encode_data_to_image(buffer,encInfo->size_secret_file,encInfo);
    return e_success;
}

//To copy remaining data from source image to stego image
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1, fptr_src))
    fwrite(&ch,1,1,fptr_dest);
    return e_success;
}