/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Keychain
 #	author : miyako
 #	2018/06/07
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		
		switch(pProcNum)
		{
				// --- Keychain
				
			case 1 :
				Keychain_Search_items(params);
				break;
				
			case 2 :
				Keychain_Add_item(params);
				break;
				
			case 3 :
				Keychain_Modify_items(params);
				break;
				
			case 4 :
				Keychain_Delete_items(params);
				break;
			default :
			{
				sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
				PackagePtr pParams = (PackagePtr)params->fParameters;
				CommandDispatcher(pProcNum, pResult, pParams);
			}
				break;
				
		}
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
		default :

			break;
	}
}

#pragma mark -

// ----------------------------------- Keychain -----------------------------------

//TODO: https://developer.apple.com/documentation/security/ksecuseauthenticationui?language=objc

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

void json_set_param(JSONNODE *node, NSMutableDictionary **query)
{
	if(node)
	{
		json_char *key = json_name(node);
		
		if (key)
		{
			NSString *val_str = json_copy_s(node);
			NSString *key_str = json_copy_s(key);
			
			if(!set_param_class(query, key_str, val_str))
			if(!set_param_key_class(query, key_str, val_str))
			if(!set_param_string(query, key_str, val_str))
			if(!set_param_numeric(query, key_str, val_str))
			if(!set_param_boolean(query, key_str, val_str))
			if(!set_param_prf(query, key_str, val_str))
			if(!set_param_protocol(query, key_str, val_str))
			if(!set_param_key_type(query, key_str, val_str))
			if(!set_param_authentication_type(query, key_str, val_str))
			if(!set_param_synchronizable(query, key_str, val_str))
			if(!set_param_authentication(query, key_str, val_str))
			
			[val_str release];
			[key_str release];
			
			json_free(key);
		}
	}
}

#pragma mark -

void json_set_data(JSONNODE *json, NSString *key, NSData *obj, PA_Variable *Param3)
{
	PA_Variable element = PA_CreateVariable(eVK_Blob);
	NSData *data = (NSData *)obj;
	PA_SetBlobVariable(&element, (void *)[data bytes], [data length]);
	PA_long32 i = PA_GetArrayNbElements(*Param3);
	PA_ResizeArray(Param3, ++i);
	json_set_i_for_key(json, (NSString *)key, i);
	PA_SetBlobInArray(*Param3, i, element.uValue.fBlob);
}

void Keychain_Search_items(PA_PluginParameters Params)
{
	PackagePtr pParams = (PackagePtr)Params->fParameters;
	sLONG_PTR *pResult = (sLONG_PTR *)Params->fResult;
	
	C_TEXT Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;

	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);

	PA_Variable Param3;
	Param3 = PA_CreateVariable(eVK_ArrayBlob);
	PA_Variable *pParam3 = &Param3;
	
	NSMutableDictionary *query = [[NSMutableDictionary alloc]init];
	
	JSONNODE *params = json_parse(Param1);

	/* query params */
	if (params)
	{
		if (json_type(params) == JSON_NODE)
		{
			JSONNODE_ITERATOR i = json_begin(params);
			while (i != json_end(params))
			{
				json_set_param(*i, &query);

				++i;
			}
		}
		
		json_delete(params);
	}

	NSArray *results;
	OSStatus status;
	
	int flags = Param2.getIntValue();
	
	BOOL data_as_hex = flags & Keychain_data_as_hex;

	[query setValue:(NSNumber *)kCFBooleanTrue forKey:(NSString *)kSecReturnAttributes];
	
	if(flags & Keychain_return_attributes)
	{
		[query setValue:(NSNumber *)kSecMatchLimitAll forKey:(NSString *)kSecMatchLimit];
		[query setValue:(NSNumber *)kCFBooleanFalse forKey:(NSString *)kSecReturnData];
	}else
	{
		[query setValue:(NSNumber *)kSecMatchLimitOne forKey:(NSString *)kSecMatchLimit];
		[query setValue:(NSNumber *)kCFBooleanTrue forKey:(NSString *)kSecReturnData];
	}

	status = SecItemCopyMatching((CFDictionaryRef)query, (CFTypeRef *)&results);	
	
	if(status == noErr)
	{
		if(flags & Keychain_return_attributes)
		{
			JSONNODE *items = json_new(JSON_ARRAY);
			for(NSUInteger i = 0; i < [results count];++i)
			{
				NSDictionary *result = [results objectAtIndex:i];
				JSONNODE *json = json_new(JSON_NODE);
				/* NSJSONSerialization can't be used here (isValidJSONObject=NO) */
				[result enumerateKeysAndObjectsUsingBlock:^void(id key, id obj, BOOL *stop)
				 {
					 /* isKindofClass can't be used here */
					 if(CFGetTypeID(obj)==CFNumberGetTypeID())
					 {
						 json_set_f_for_key(json, (NSString *)key, (NSNumber *)obj);
					 }else
						 if(CFGetTypeID(obj)==CFStringGetTypeID())
						 {
							 json_set_s_for_key(json, (NSString *)key, (NSString *)obj);
						 }else
							 if(CFGetTypeID(obj)==CFNullGetTypeID())
							 {
								 json_set_n_for_key(json, (NSString *)key);
							 }else
								 if(CFGetTypeID(obj)==CFDateGetTypeID())
								 {
									 json_set_d_for_key(json, (NSString *)key, (NSDate *)obj);
								 }else
									 if(CFGetTypeID(obj)==CFDataGetTypeID())
									 {
										 if(flags & Keychain_data_as_index)
										 {
											 json_set_data(json, (NSString *)key, (NSData *)obj, pParam3);
										 }else
										 {
											 json_set_s_for_key(json, (NSString *)key, (NSData *)obj, data_as_hex);
										 }
									 }
					 
				 }];/* enumerateKeysAndObjectsUsingBlock */
				
				json_push_back(items, json);
			}
			
			JSONNODE *json = json_new(JSON_NODE);
			CFStringRef errorMessage = SecCopyErrorMessageString(status, NULL);
			if(errorMessage)
			{
				json_set_i_for_key(json, L"error", status);
				json_set_s_for_key(json, L"errorMessage", (NSString *)errorMessage);
				CFRelease(errorMessage);
			}
			
			json_set_name(items, L"items");
			json_push_back(json, items);
			
			CUTF16String json_str;
			json_stringify(json, json_str);
			json_delete(json);
			
			returnValue.setUTF16String(&json_str);
			
		}else
		{
			NSDictionary *result = (NSDictionary *)results;
			JSONNODE *item = json_new(JSON_NODE);
			/* NSJSONSerialization can't be used here (isValidJSONObject=NO) */
			[result enumerateKeysAndObjectsUsingBlock:^void(id key, id obj, BOOL *stop)
			 {
				 /* isKindofClass can't be used here */
				 if(CFGetTypeID(obj)==CFNumberGetTypeID())
				 {
					 json_set_f_for_key(item, (NSString *)key, (NSNumber *)obj);
				 }else
					 if(CFGetTypeID(obj)==CFStringGetTypeID())
					 {
						 json_set_s_for_key(item, (NSString *)key, (NSString *)obj);
					 }else
						 if(CFGetTypeID(obj)==CFNullGetTypeID())
						 {
							 json_set_n_for_key(item, (NSString *)key);
						 }else
							 if(CFGetTypeID(obj)==CFDateGetTypeID())
							 {
								 json_set_d_for_key(item, (NSString *)key, (NSDate *)obj);
							 }else
								 if(CFGetTypeID(obj)==CFDataGetTypeID())
								 {
									 if(flags & Keychain_data_as_index)
									 {
										 json_set_data(item, (NSString *)key, (NSData *)obj, pParam3);
									 }else
									 {
										 json_set_s_for_key(item, (NSString *)key, (NSData *)obj, data_as_hex);
									 }
								 }
				 
			 }]; /* enumerateKeysAndObjectsUsingBlock */
			
			JSONNODE *json = json_new(JSON_NODE);
			CFStringRef errorMessage = SecCopyErrorMessageString(status, NULL);
			if(errorMessage)
			{
				json_set_i_for_key(json, L"error", status);
				json_set_s_for_key(json, L"errorMessage", (NSString *)errorMessage);
				CFRelease(errorMessage);
			}
			
			json_set_name(item, L"item");
			json_push_back(json, item);
			
			CUTF16String json_str;
			json_stringify(json, json_str);
			json_delete(json);
			
			returnValue.setUTF16String(&json_str);
		}
	}
	
	[query release];
	
	PA_SetVariableParameter(Params, 3, Param3, 0);
	
	returnValue.setReturn(pResult);
}

void Keychain_Add_item(PA_PluginParameters Params)
{
	PackagePtr pParams = (PackagePtr)Params->fParameters;
	sLONG_PTR *pResult = (sLONG_PTR *)Params->fResult;
	
	C_TEXT Param1;
	C_BLOB Param2;
	C_TEXT returnValue;
}

void Keychain_Modify_items(PA_PluginParameters Params)
{
	PackagePtr pParams = (PackagePtr)Params->fParameters;
	sLONG_PTR *pResult = (sLONG_PTR *)Params->fResult;
	
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT returnValue;
}

void Keychain_Delete_items(PA_PluginParameters Params)
{
	PackagePtr pParams = (PackagePtr)Params->fParameters;
	sLONG_PTR *pResult = (sLONG_PTR *)Params->fResult;
	
	C_TEXT Param1;
	C_REAL returnValue;
}

