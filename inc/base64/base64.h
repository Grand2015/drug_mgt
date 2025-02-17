

#ifndef __BASE64_HWW_H__
#define __BASE64_HWW_H__

typedef enum{
    HWW_DEBUG_LEVEL_ERR = 0,
    HWW_DEBUG_LEVEL_INFO,
    HWW_DEBUG_LEVEL_HINT
}HWW_DEBUG_LEVEL_E;


/*
@fuction:hww_base64_init
@bef:print ctrl function
@param:
    debug_level
*/
void hww_base64_init(HWW_DEBUG_LEVEL_E debug_level);

/*
@fuction:hww_base64_encryption
@bef:base64 encrytion function
@param:
    str_input
    len_input
    out_put
    len_output
@return:0 - OK/ -1 - ERROR
*/
int hww_base64_encryption(char *str_input, int len_input, char *out_put, int len_output);

/*
@fuction:hww_base64_decryption
@bef:base64 decrytion function
@param:
    str_input
    len_input
    out_put
    len_output
@return:0 - OK/ -1 - ERROR
*/
int hww_base64_decryption(char *str_input, int len_input, char *out_put, int len_output);

#endif