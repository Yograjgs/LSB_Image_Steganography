# LSB_Image_Steganography

A simple and efficient implementation of Least Significant Bit (LSB) Image Steganography in the C programming language.
This project allows you to hide secret data inside a 24-bit BMP image and retrieve it back using command-line operations.
The encoder and decoder use bit manipulation to embed data inside the LSBs of image pixels without visibly altering the image.

 Features :
 
Embed (hide) text/data inside BMP images
Extract hidden data from stego images
Uses LSB technique for steganography
Works with 24-bit BMP image format
Simple command-line usage
Easy to compile and run

Compilation :

Compile all .c files at once using GCC:

gcc *.c

This generates the executable file a.out.


Usage

Encode (Hide Data)
./a.out -e beautiful.bmp secret.txt stego.bmp

beautiful.bmp → input cover image
secret.txt → file to hide
stego.bmp → output image containing hidden data


Decode (Extract Data)
./a.out -d stego.bmp output.txt

stego.bmp → image with hidden data
output.txt → extracted secret data

Project Structure

├── common.h
├── types.h
├── encode.h
├── decode.h
├── encode.c
├── decode.c
├── test_encode.c
├── beautiful.bmp
├── secret.txt
├── stego.bmp
├── output.txt
└── README.md

Only works with uncompressed 24-bit BMP images
Ensure that the secret file size is smaller than the image’s capacity
Output files are generated automatically during encoding/decoding
Pure C implementation — no external libraries required

Overview

This project demonstrates how digital images can be used as carriers for hidden information.
LSB steganography provides a simple yet effective way to conceal data without noticeably changing the image.
It is useful for learning:

Bitwise operations
File handling in C
Working with BMP image formats
Data hiding and retrieval techniques
