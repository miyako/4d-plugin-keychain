#include "4DPluginAPI.h"
#include "libjson.h"

#if defined(__APPLE__)
#define DATE_FORMAT_ISO @"yyyy-MM-dd'T'HH:mm:ss"
#define DATE_FORMAT_ISO_GMT @"yyyy-MM-dd'T'HH:mm:ss'Z'"
#endif

JSONNODE *json_parse(C_TEXT &t);

void json_push_back_s(JSONNODE *n, const char *value);
void json_set_s(JSONNODE *n, const char *value);
#if defined(__APPLE__)
void json_set_s(JSONNODE *n, NSString *value);
#endif

#if defined(__APPLE__)
NSString *json_copy_s(JSONNODE *n);
NSString *json_copy_s(json_char *value);
#endif

void json_set_n_for_key(JSONNODE *n, json_char *key);
#if defined(__APPLE__)
void json_set_n_for_key(JSONNODE *n, NSString *key);
#endif

/* utf8 to utf32 */
void json_set_s_for_key(JSONNODE *n, json_char *key, const char *value);
void json_set_s_for_key(JSONNODE *n, json_char *key, CUTF8String &value);
/* utf16 to utf32 */
void json_set_s_for_key(JSONNODE *n, json_char *key, C_TEXT &value);
void json_set_s_for_key(JSONNODE *n, json_char *key, CUTF16String *value);
#if defined(__APPLE__)
void json_set_s_for_key(JSONNODE *n, json_char *key, NSString *value);
void json_set_s_for_key(JSONNODE *n, NSString *key, NSString *value);
void json_set_s_for_key(JSONNODE *n, NSString *name, NSData *value, BOOL isHex);
#endif

void json_set_b_for_key(JSONNODE *n, json_char *key, json_bool_t value);
void json_set_i_for_key(JSONNODE *n, json_char *key, json_int_t value);
#if defined(__APPLE__)
void json_set_i_for_key(JSONNODE *n, NSString *key, json_int_t value);
#endif
void json_set_f_for_key(JSONNODE *n, const wchar_t *name, json_number value);
#if defined(__APPLE__)
void json_set_f_for_key(JSONNODE *n, NSString *name, NSNumber *value);
#endif

#if defined(__APPLE__)
void json_set_d_for_key(JSONNODE *n, NSString *name, NSDate *value, BOOL isGMT = YES);
#endif

/* utf32 to utf16 */
void json_stringify(JSONNODE *json, CUTF16String &t, BOOL pretty = NO);
void json_stringify(JSONNODE *json, C_TEXT &t, BOOL pretty = NO);
