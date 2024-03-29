/* --------------------------------------------------------------------------------
 #
 #  4DPlugin-Keychain-Access.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Keychain Access
 #	author : miyako
 #	2022/09/05
 #  
 # --------------------------------------------------------------------------------*/

#include "4DPlugin-Keychain-Access.h"

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params) {
    
	try
	{
        switch(selector)
        {
			// --- Keychain Access
            
			case 1 :
				Keychain_Search_items(params);
				break;

        }

	}
	catch(...)
	{

	}
}

#pragma mark -

BOOL set_param_class(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"class"])
    {
        NSUInteger f;
        
        NSArray *classes = @[@"genericPassword",
                                                 @"internetPassword",
                                                 @"certificate",
                                                 @"key",
                                                 @"identity"];
        
        NSArray *class_constants = @[(NSString *)kSecClassGenericPassword,
                                                                 (NSString *)kSecClassInternetPassword,
                                                                 (NSString *)kSecClassCertificate,
                                                                 (NSString *)kSecClassKey,
                                                                 (NSString *)kSecClassIdentity];
        
        f = [classes indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                     if ([(NSString*)obj isEqualToString:val_str])
                     {
                         return YES;
                     }
                     return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[class_constants objectAtIndex:f]
                                forKey:(NSString *)kSecClass];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_authentication(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"authentication"])
    {
        NSUInteger f;
        
        NSArray *authentications = @[@"allow",
                                                                 @"fail",
                                                                 @"skip"];
        
        NSArray *authentication_constants = @[(NSString *)kSecUseAuthenticationUIAllow,
                                                                                    (NSString *)kSecUseAuthenticationUIFail,
                                                                                    (NSString *)kSecUseAuthenticationUISkip];
        
        f = [authentications indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[authentication_constants objectAtIndex:f]
                                forKey:(NSString *)kSecUseAuthenticationUI];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_accessible(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"accessible"])
    {
        NSUInteger f;
        
        NSArray *accessibles = @[@"whenPasscodeSetThisDeviceOnly",
                                                         @"whenUnlockedThisDeviceOnly",
                                                         @"whenUnlocked",
                                                         @"afterFirstUnlockThisDeviceOnly",
                                                         @"afterFirstUnlock",
                                                         @"alwaysThisDeviceOnly",
                                                         @"always"];
        
        NSArray *accessibles_constants = @[(NSString *)kSecAttrAccessibleWhenPasscodeSetThisDeviceOnly,
                                                                             (NSString *)kSecAttrAccessibleWhenUnlockedThisDeviceOnly,
                                                                             (NSString *)kSecAttrAccessibleWhenUnlocked,
                                                                             (NSString *)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly,
                                                                             (NSString *)kSecAttrAccessibleAfterFirstUnlock,
                                                                             (NSString *)kSecAttrAccessibleAlwaysThisDeviceOnly,
                                                                             (NSString *)kSecAttrAccessibleAlways];
        
        f = [accessibles indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[accessibles_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrAccessible];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_authentication_type(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"keyClass"])
    {
        NSUInteger f;
        
        NSArray *authentication_types = @[@"NTLM",
                                                                            @"MSN",
                                                                            @"DPA",
                                                                            @"RPA",
                                                                            @"HTTPBasic",
                                                                            @"HTTPDigest",
                                                                            @"HTMLForm",
                                                                            @"default"];
        
        NSArray *authentication_type_constants = @[(NSString *)kSecAttrAuthenticationTypeNTLM,
                                                                                             (NSString *)kSecAttrAuthenticationTypeMSN,
                                                                                             (NSString *)kSecAttrAuthenticationTypeDPA,
                                                                                             (NSString *)kSecAttrAuthenticationTypeRPA,
                                                                                             (NSString *)kSecAttrAuthenticationTypeHTTPBasic,
                                                                                             (NSString *)kSecAttrAuthenticationTypeHTTPDigest,
                                                                                             (NSString *)kSecAttrAuthenticationTypeHTMLForm,
                                                                                             (NSString *)kSecAttrAuthenticationTypeDefault];
        
        f = [authentication_types indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[authentication_type_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrAuthenticationType];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_prf(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"keyClass"])
    {
        NSUInteger f;
        
        NSArray *prfs = @[@"SHA1",
                                            @"SHA224",
                                            @"SHA256",
                                            @"SHA384",
                                            @"SHA512"];
        
        NSArray *prf_constants = @[(NSString *)kSecAttrPRFHmacAlgSHA1,
                                                             (NSString *)kSecAttrPRFHmacAlgSHA224,
                                                             (NSString *)kSecAttrPRFHmacAlgSHA256,
                                                             (NSString *)kSecAttrPRFHmacAlgSHA384,
                                                             (NSString *)kSecAttrPRFHmacAlgSHA512];
        
        f = [prfs indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[prf_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrKeyClass];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_protocol(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"protocol"])
    {
        NSUInteger f;
        
        NSArray *protocols = @[@"FTP",
                                                     @"FTPAccount",
                                                     @"HTTP",
                                                     @"IRC",
                                                     @"NNTP",
                                                     @"POP3",
                                                     @"SMTP",
                                                     @"SOCKS",
                                                     @"IMAP",
                                                     @"LDAP",
                                                     @"AppleTalk",
                                                     @"AFP",
                                                     @"Telnet",
                                                     @"SSH",
                                                     @"FTPS",
                                                     @"HTTPS",
                                                     @"HTTPProxy",
                                                     @"HTTPSProxy",
                                                     @"FTPProxy",
                                                     @"SMB",
                                                     @"RTSP",
                                                     @"RTSPProxy",
                                                     @"DAAP",
                                                     @"EPPC",
                                                     @"IPP",
                                                     @"NNTPS",
                                                     @"LDAPS",
                                                     @"TelnetS",
                                                     @"IMAPS",
                                                     @"IRCS",
                                                     @"POP3S"];
        
        NSArray *protocol_constants = @[(NSString *)kSecAttrProtocolFTP,
                                                                        (NSString *)kSecAttrProtocolFTPAccount,
                                                                        (NSString *)kSecAttrProtocolHTTP,
                                                                        (NSString *)kSecAttrProtocolIRC,
                                                                        (NSString *)kSecAttrProtocolNNTP,
                                                                        (NSString *)kSecAttrProtocolPOP3,
                                                                        (NSString *)kSecAttrProtocolSMTP,
                                                                        (NSString *)kSecAttrProtocolSOCKS,
                                                                        (NSString *)kSecAttrProtocolIMAP,
                                                                        (NSString *)kSecAttrProtocolLDAP,
                                                                        (NSString *)kSecAttrProtocolAppleTalk,
                                                                        (NSString *)kSecAttrProtocolAFP,
                                                                        (NSString *)kSecAttrProtocolTelnet,
                                                                        (NSString *)kSecAttrProtocolSSH,
                                                                        (NSString *)kSecAttrProtocolFTPS,
                                                                        (NSString *)kSecAttrProtocolHTTPS,
                                                                        (NSString *)kSecAttrProtocolHTTPProxy,
                                                                        (NSString *)kSecAttrProtocolHTTPSProxy,
                                                                        (NSString *)kSecAttrProtocolFTPProxy,
                                                                        (NSString *)kSecAttrProtocolSMB,
                                                                        (NSString *)kSecAttrProtocolRTSP,
                                                                        (NSString *)kSecAttrProtocolRTSPProxy,
                                                                        (NSString *)kSecAttrProtocolDAAP,
                                                                        (NSString *)kSecAttrProtocolEPPC,
                                                                        (NSString *)kSecAttrProtocolIPP,
                                                                        (NSString *)kSecAttrProtocolNNTPS,
                                                                        (NSString *)kSecAttrProtocolLDAPS,
                                                                        (NSString *)kSecAttrProtocolTelnetS,
                                                                        (NSString *)kSecAttrProtocolIMAPS,
                                                                        (NSString *)kSecAttrProtocolIRCS,
                                                                        (NSString *)kSecAttrProtocolPOP3S];
        
        f = [protocols indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[protocol_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrProtocol];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_key_type(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"keyType"])
    {
        NSUInteger f;
        
        NSArray *key_types = @[@"RSA",
                                                     @"DSA",
                                                     @"AES",
                                                     @"DES",
                                                     @"3DES",
                                                     @"RC4",
                                                     @"RC2",
                                                     @"CAST",
                                                     @"ECDSA",
                                                     @"EC"];
        
        NSArray *key_type_constants = @[(NSString *)kSecAttrKeyTypeRSA,
                                                                        (NSString *)kSecAttrKeyTypeDSA,
                                                                        (NSString *)kSecAttrKeyTypeAES,
                                                                        (NSString *)kSecAttrKeyTypeDES,
                                                                        (NSString *)kSecAttrKeyType3DES,
                                                                        (NSString *)kSecAttrKeyTypeRC4,
                                                                        (NSString *)kSecAttrKeyTypeRC2,
                                                                        (NSString *)kSecAttrKeyTypeCAST,
                                                                        (NSString *)kSecAttrKeyTypeECDSA,
                                                                        (NSString *)kSecAttrKeyTypeEC];
        
        f = [key_types indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                 if ([(NSString*)obj isEqualToString:val_str])
                 {
                 return YES;
                 }
                 return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[key_type_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrKeyType];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_key_class(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"keyClass"])
    {
        NSUInteger f;
        
        NSArray *key_classes = @[@"public",
                                                         @"private",
                                                         @"symmetric"];
        
        NSArray *key_class_constants = @[(NSString *)kSecAttrKeyClassPublic,
                                                                         (NSString *)kSecAttrKeyClassPrivate,
                                                                         (NSString *)kSecAttrKeyClassSymmetric];
        
        f = [key_classes indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                 {
                     if ([(NSString*)obj isEqualToString:val_str])
                     {
                         return YES;
                     }
                     return NO;
                 }];
        
        if(f != NSNotFound)
        {
            [*query setValue:(NSString *)[key_class_constants objectAtIndex:f]
                                forKey:(NSString *)kSecAttrKeyClass];
            
            return YES;
        }
        
    }
    
    return NO;
}

BOOL set_param_string(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    NSUInteger f;
    
    NSArray *string_keys = @[@"path",
                                                     @"server",
                                                     @"account",
                                                     @"label",
                                                     @"comment",
                                                     @"description",
                                                     @"service",
                                                     @"applicationLabel",
                                                     @"issuers",
                                                     @"email",
                                                     @"subjectContains",
                                                     @"subjectStartsWith",
                                                     @"subjectEndsWith",
                                                     @"subjectWholeString",
                                                     @"accessGroup",
                                                     @"securityDomain",
                                                     @"generic"];
    
    NSArray *string_key_constants = @[(NSString *)kSecAttrPath,
                                                                        (NSString *)kSecAttrServer,
                                                                        (NSString *)kSecAttrAccount,
                                                                        (NSString *)kSecAttrLabel,
                                                                        (NSString *)kSecAttrComment,
                                                                        (NSString *)kSecAttrDescription,
                                                                        (NSString *)kSecAttrService,
                                                                        (NSString *)kSecAttrApplicationLabel,
                                                                        (NSString *)kSecMatchIssuers,
                                                                        (NSString *)kSecMatchEmailAddressIfPresent,
                                                                        (NSString *)kSecMatchSubjectContains,
                                                                        (NSString *)kSecMatchSubjectStartsWith,
                                                                        (NSString *)kSecMatchSubjectEndsWith,
                                                                        (NSString *)kSecMatchSubjectWholeString,
                                                                        (NSString *)kSecAttrAccessGroup,
                                                                        (NSString *)kSecAttrSecurityDomain,
                                                                        (NSString *)kSecAttrGeneric];
    
    f = [string_keys indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
                {
                    if ([(NSString*)obj isEqualToString:key_str])
                    {
                        return YES;
                    }
                    return NO;
                }];
    
    if(f != NSNotFound)
    {
        [*query setValue:val_str
                            forKey:(NSString *)[string_key_constants objectAtIndex:f]];
        
        return YES;
    }
    
    return NO;
}

BOOL set_param_boolean(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    NSUInteger f;
    
    NSArray *keys = @[@"caseInsensitive",
                                        @"diacriticInsensitive",
                                        @"widthInsensitive",
                                        @"trustedOnly",
                                        @"isPermanent",
                                        @"isSensitive",
                                        @"isExtractable",
                                        @"canEncrypt",
                                        @"canDecrypt",
                                        @"canDerive",
                                        @"canSign",
                                        @"canVerify",
                                        @"canWrap",
                                        @"canUnwrap",
                                        @"isInvisible",
                                        @"isNegative"];
    
    NSArray *key_constants = @[(NSString *)kSecMatchCaseInsensitive,
                                                         (NSString *)kSecMatchDiacriticInsensitive,
                                                         (NSString *)kSecMatchWidthInsensitive,
                                                         (NSString *)kSecMatchTrustedOnly,
                                                         (NSString *)kSecAttrIsSensitive,
                                                         (NSString *)kSecAttrIsExtractable,
                                                         (NSString *)kSecAttrCanEncrypt,
                                                         (NSString *)kSecAttrCanDecrypt,
                                                         (NSString *)kSecAttrCanDerive,
                                                         (NSString *)kSecAttrCanSign,
                                                         (NSString *)kSecAttrCanVerify,
                                                         (NSString *)kSecAttrCanWrap,
                                                         (NSString *)kSecAttrCanUnwrap,
                                                         (NSString *)kSecAttrIsInvisible,
                                                         (NSString *)kSecAttrIsNegative];
    
    f = [keys indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
             {
             if ([(NSString*)obj isEqualToString:key_str])
             {
             return YES;
             }
             return NO;
             }];
    
    if(f != NSNotFound)
    {
        if([val_str intValue])
        {
            [*query setValue:(NSNumber *)kCFBooleanTrue
                                forKey:(NSString *)[key_constants objectAtIndex:f]];
        }else
        {
            [*query setValue:(NSNumber *)kCFBooleanFalse
                                forKey:(NSString *)[key_constants objectAtIndex:f]];
        }
        
        return YES;
    }
    
    return NO;
}

BOOL set_param_synchronizable(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    if([key_str isEqualToString:@"synchronizable"])
    {
        if([val_str isEqualToString:@"any"])
        {
            [*query setValue:(NSString *)kSecAttrSynchronizableAny
                                forKey:(NSString *)kSecAttrSynchronizable];
        }else
        if([val_str isEqualToString:@"true"])
        {
            [*query setValue:(NSNumber *)kCFBooleanTrue
                                forKey:(NSString *)kSecAttrSynchronizable];
        }else
        if([val_str isEqualToString:@"false"])
        {
            [*query setValue:(NSNumber *)kCFBooleanFalse
                                forKey:(NSString *)kSecAttrSynchronizable];
        }
    }
    
    return NO;
}

BOOL set_param_numeric(NSMutableDictionary **query, NSString *key_str, NSString *val_str)
{
    NSUInteger f;
    
    NSArray *keys = @[@"port",
                                        @"certificateType",
                                        @"certificateEncoding",
                                        @"rounds",
                                        @"keySizeInBits",
                                        @"effectiveKeySize",
                                        @"type"];
    
    NSArray *key_constants = @[(NSString *)kSecAttrPort,
                                                         (NSString *)kSecAttrCertificateType,
                                                         (NSString *)kSecAttrCertificateEncoding,
                                                         (NSString *)kSecAttrRounds,
                                                         (NSString *)kSecAttrKeySizeInBits,
                                                         (NSString *)kSecAttrEffectiveKeySize,
                                                         (NSString *)kSecAttrType];
    
    f = [keys indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop)
             {
                 if ([(NSString*)obj isEqualToString:key_str])
                 {
                     return YES;
                 }
                 return NO;
             }];
    
    if(f != NSNotFound)
    {
        [*query setValue:(NSNumber *)[NSNumber numberWithInt:[val_str intValue]]
                            forKey:(NSString *)[key_constants objectAtIndex:f]];
        
        return YES;
    }
    
    return NO;
}

#pragma mark -
static bool parseObject(PA_ObjectRef arg, Json::Value& jsonValue) {
    
    if(arg) {
        
        std::string jsonString;
        ob_stringify(arg, &jsonString);
                        
        Json::CharReaderBuilder builder;
        std::string errors;
        Json::Value root;
        
        Json::CharReader *reader = builder.newCharReader();
        bool parse = reader->parse(jsonString.c_str(),
                                   jsonString.c_str() + jsonString.size(),
                                   &jsonValue,
                                   &errors);
        delete reader;
        return parse;
    }
    
    return false;
}

static void appendObjectToCollection(PA_CollectionRef col, PA_ObjectRef obj) {
    
    PA_Variable v = PA_CreateVariable(eVK_Object);
    PA_SetObjectVariable(&v, obj);
    PA_SetCollectionElement(col, PA_GetCollectionLength(col), v);
    PA_ClearVariable(&v);
}

void Keychain_Search_items(PA_PluginParameters params) {

    PA_ObjectRef returnValue = PA_CreateObject();
    PA_ObjectRef arg1 = PA_GetObjectParameter(params, 1);
    
    Json::Value jsonValue;
    
    if(parseObject(arg1, jsonValue)) {
        
        NSMutableDictionary *query = [[NSMutableDictionary alloc]init];
        
        for(Json::Value::const_iterator it = jsonValue.begin() ; it != jsonValue.end() ; it++)
        {
            JSONCPP_STRING _name = it.name();
            if(_name.length() != 0) {
                if(it->isString())
                {
                    JSONCPP_STRING _value = it->asString();
                    
                    NSString *key = [[NSString alloc]initWithUTF8String:_name.c_str()];
                    NSString *value = [[NSString alloc]initWithUTF8String:_value.c_str()];
                    
                    if(!set_param_class(&query, key, value))
                    if(!set_param_key_class(&query, key, value))
                    if(!set_param_string(&query, key, value))
                    if(!set_param_numeric(&query, key, value))
                    if(!set_param_boolean(&query, key, value))
                    if(!set_param_prf(&query, key, value))
                    if(!set_param_protocol(&query, key, value))
                    if(!set_param_key_type(&query, key, value))
                    if(!set_param_authentication_type(&query, key, value))
                    if(!set_param_synchronizable(&query, key, value))
                    if(!set_param_authentication(&query, key, value))
                                    
                    [key release];
                    [value release];
                }
            }
        }
        
        OSStatus status;
        NSArray *results;
                
        [query setValue:(NSNumber *)kCFBooleanTrue forKey:(NSString *)kSecReturnAttributes];

        bool returnData = ob_get_b(arg1, L"data");
        
        if(returnData) {
            //return data
            [query setValue:(NSNumber *)kSecMatchLimitOne forKey:(NSString *)kSecMatchLimit];
            [query setValue:(NSNumber *)kCFBooleanTrue forKey:(NSString *)kSecReturnData];
        }else{
            //return attributes
            [query setValue:(NSNumber *)kSecMatchLimitAll forKey:(NSString *)kSecMatchLimit];
            [query setValue:(NSNumber *)kCFBooleanFalse forKey:(NSString *)kSecReturnData];
        }
        
        status = SecItemCopyMatching((CFDictionaryRef)query, (CFTypeRef *)&results);
        
        NSString *errorMessage = (NSString *)SecCopyErrorMessageString(status, NULL);
        
        if(errorMessage)
        {
            ob_set_n(returnValue, "error", status);
            ob_set_s(returnValue, "errorMessage", [errorMessage UTF8String]);
            [errorMessage release];
        }

        PA_CollectionRef items;
        
        items = PA_CreateCollection();

        if(returnData) {
            [(NSDictionary *)results enumerateKeysAndObjectsUsingBlock:^void(id key, id obj, BOOL *stop)
            {
                /* isKindofClass can't be used here */
                if(CFGetTypeID(obj)==CFNumberGetTypeID())
                {
                    PA_ObjectRef item = PA_CreateObject();
                    ob_set_n(item, [key UTF8String], [obj doubleValue]);
                    appendObjectToCollection(items, item);
                }else
                    if(CFGetTypeID(obj)==CFStringGetTypeID())
                    {
                        PA_ObjectRef item = PA_CreateObject();
                        ob_set_s(item, [key UTF8String], [obj UTF8String]);
                        appendObjectToCollection(items, item);
                    }else
                        if(CFGetTypeID(obj)==CFNullGetTypeID())
                        {
                            PA_ObjectRef item = PA_CreateObject();
                            ob_set_0(item, [key UTF8String]);
                            appendObjectToCollection(items, item);
                        }else
                            if(CFGetTypeID(obj)==CFDateGetTypeID())
                            {
                                PA_ObjectRef item = PA_CreateObject();
                                NSDateFormatter *GMT = [[NSDateFormatter alloc]init];
                                [GMT setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
                                [GMT setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
                                ob_set_s(item, [key UTF8String],[[GMT stringFromDate:obj]UTF8String]);
                                [GMT release];
                                appendObjectToCollection(items, item);
                            }else
                                if(CFGetTypeID(obj)==CFDataGetTypeID())
                                {
                                    PA_ObjectRef item = PA_CreateObject();
                                    C_BLOB b;
                                    b.setBytes((const uint8_t *)[obj bytes], (uint32_t)[obj length]);
                                    C_TEXT t;
                                    b.toB64Text(&t);
                                    CUTF8String u8;
                                    t.copyUTF8String(&u8);
                                    ob_set_s(item, [key UTF8String], (const char *)u8.c_str());
                                    appendObjectToCollection(items, item);
                                }
            
            }];/* enumerateKeysAndObjectsUsingBlock */

        }else{
            for(NSUInteger i = 0; i < [results count];++i)
            {
                NSDictionary *result = [results objectAtIndex:i];
                /* NSJSONSerialization can't be used here (isValidJSONObject=NO) */
                [result enumerateKeysAndObjectsUsingBlock:^void(id key, id obj, BOOL *stop)
                 {
                     /* isKindofClass can't be used here */
                     if(CFGetTypeID(obj)==CFNumberGetTypeID())
                     {
                         PA_ObjectRef item = PA_CreateObject();
                         ob_set_n(item, [key UTF8String], [obj doubleValue]);
                         appendObjectToCollection(items, item);
                     }else
                         if(CFGetTypeID(obj)==CFStringGetTypeID())
                         {
                             PA_ObjectRef item = PA_CreateObject();
                             ob_set_s(item, [key UTF8String], [obj UTF8String]);
                             appendObjectToCollection(items, item);
                         }else
                             if(CFGetTypeID(obj)==CFNullGetTypeID())
                             {
                                 PA_ObjectRef item = PA_CreateObject();
                                 ob_set_0(item, [key UTF8String]);
                                 appendObjectToCollection(items, item);
                             }else
                                 if(CFGetTypeID(obj)==CFDateGetTypeID())
                                 {
                                     PA_ObjectRef item = PA_CreateObject();
                                     NSDateFormatter *GMT = [[NSDateFormatter alloc]init];
                                     [GMT setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
                                     [GMT setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
                                     ob_set_s(item, [key UTF8String],[[GMT stringFromDate:obj]UTF8String]);
                                     [GMT release];
                                     appendObjectToCollection(items, item);
                                 }else
                                     if(CFGetTypeID(obj)==CFDataGetTypeID())
                                     {
                                         PA_ObjectRef item = PA_CreateObject();
                                         C_BLOB b;
                                         b.setBytes((const uint8_t *)[obj bytes], (uint32_t)[obj length]);
                                         C_TEXT t;
                                         b.toB64Text(&t);
                                         CUTF8String u8;
                                         t.copyUTF8String(&u8);
                                         ob_set_s(item, [key UTF8String], (const char *)u8.c_str());
                                         appendObjectToCollection(items, item);
                                     }
                     
                 }];/* enumerateKeysAndObjectsUsingBlock */
                
            }
        }
        
        ob_set_c(returnValue, L"items", items);
        
        [query release];
    }
    
    PA_ReturnObject(params, returnValue);
}

