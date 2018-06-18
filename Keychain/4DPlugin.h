/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.h
 #	source generated by 4D Plugin Wizard
 #	Project : Keychain
 #	author : miyako
 #	2018/06/07
 #
 # --------------------------------------------------------------------------------*/

#include "libjson.h"
#include "libjson_methods.h"
#include <Security/Security.h>

// --- Keychain
void Keychain_Search_items(PA_PluginParameters params);
void Keychain_Add_item(PA_PluginParameters params);
void Keychain_Modify_items(PA_PluginParameters params);
void Keychain_Delete_items(PA_PluginParameters params);

#define Keychain_return_data 0
#define Keychain_return_attributes 1

#define Keychain_data_as_index 2
#define Keychain_data_as_base64 4
#define Keychain_data_as_hex 8
