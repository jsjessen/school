#!/bin/bash

# Utilize OpenSSL to encrypt the following value with the specified cipher as demonstrated in class. Include, the ciphertext (use xxd) for each answer and the command used. Openssl should be available on any Linux/Unix system.
# 
# Text: BBBBBBBBBBBBBBBBCCCCCCCCDDDDDD
# 
# a) Cipher: DES-ECB, Key: 01234567
#  Also, what interesting feature occurs in the cipher text?

echo "a)"
openssl enc -des-ecb -in p7.txt -out p7a.out -K 01234567
echo
echo "openssl enc -des-ecb -in p7.txt -out p7a.out -K 01234567"
echo
echo "------------------------------------------------------------------"
xxd p7a.out
echo "------------------------------------------------------------------"
echo

# b) Cipher: AES-128-ECB, Key: 0123456789abcdef

echo "b)"
openssl enc -aes-128-ecb -in p7.txt -out p7b.out -K 0123456789abcdef
echo
echo "openssl enc -aes-128-ecb -in p7.txt -out p7b.out -K 0123456789abcdef"
echo
echo "------------------------------------------------------------------"
xxd p7b.out
echo "------------------------------------------------------------------"
echo
 
# c) Cipher: AES-128-CBC, Key: 0123456789abcdef, IV = 0000000000000001

echo "c)"
openssl enc -aes-128-cbc -in p7.txt -out p7c.out -K 0123456789abcdef -iv 0000000000000001
echo
echo "openssl enc -aes-128-cbc -in p7.txt -out p7c.out -K 0123456789abcdef -iv 0000000000000001"
echo
echo "------------------------------------------------------------------"
xxd p7c.out
echo "------------------------------------------------------------------"
echo
 
#  Also, why is this ciphertext different from b's?
