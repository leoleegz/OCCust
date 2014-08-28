#ifndef _BASE64_H_
#define _BASE64_H_

#include <wtypes.h>

/**
This encodes raw data to Base64.
*/
int base64_encode(unsigned char* in, WCHAR* out, int len);

/**
This function decodes Base64 code into its original form.
*/
int base64_decode(unsigned char* in, unsigned char* out, int len);

#endif //_BASE64_H_