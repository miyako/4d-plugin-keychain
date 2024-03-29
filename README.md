![version](https://img.shields.io/badge/version-19%2B-5682DF)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-keychain)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-keychain/total)

# 4d-plugin-keychain
Read macOS Keychain (Write, Modify, Delete not implemented)

## Generic Password / Internet Password

create password in keychain

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188371613-8844a4d6-ed4b-4945-b5ee-837b4900a67d.png">

```4d
var $query : Object

$query:=New object

$query.class:="internetPassword"
$query.authentication:="allow"
$query.account:="miyako"
$query.label:="MacBook Pro 2019"
$query.server:="100.64.1.26:5900"  //port is 0 in this case
$query.data:=True
$status:=Keychain Search items($query)
```

or 

```4d
var $query : Object

$query:=New object

$query.class:="genericPassword"
$query.authentication:="allow"
$query.account:="miyako" 
$query.label:="TEST"
$query.data:=True
$status:=Keychain Search items($query)
```

UI is displayed on the first instance. after that, the app is authorised to access this keychain.

<img width="434" alt="" src="https://user-images.githubusercontent.com/1725068/188376499-13275811-fa4d-477c-94f8-fdbb57d262b9.png">

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188371948-999b6c2e-7773-4a04-8da3-527f5f15ea3b.png">

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188376067-2291d625-cebf-4809-80b7-ad35e328eddd.png">

---

## Syntax

```
status:=Keychain Search items (query)
```

Parameter|Type|Description
------------|------------|----
dictionary|Object|
status|Object|

query parameters may include:  

* [``data``](https://developer.apple.com/documentation/security/ksecreturndata)

:possible values: `true`, `false` (as boolean, not string)

* [``class``](https://developer.apple.com/documentation/security/ksecclass?language=objc)

[possible values](https://developer.apple.com/documentation/security/keychain_services/keychain_items/item_class_keys_and_values?language=objc): ``genericPassword``, ``internetPassword``, ``certificate``, ``key``, ``identity``

* [``keyClass``](https://developer.apple.com/documentation/security/ksecattrkeyclass?language=objc) 

possible values: ``public``, ``private``, ``symmetric``

attributes with a string value:

* [``path``](https://developer.apple.com/documentation/security/ksecattrpath?language=objc)

* [``server``](https://developer.apple.com/documentation/security/ksecattrserver?language=objc)

* [``account``](https://developer.apple.com/documentation/security/ksecattraccount?language=objc)

* [``label``](https://developer.apple.com/documentation/security/ksecattrlabel?language=objc)

* [``comment``](https://developer.apple.com/documentation/security/ksecattrcomment?language=objc)

* [``description``](https://developer.apple.com/documentation/security/ksecattrdescription?language=objc)

* [``service``](https://developer.apple.com/documentation/security/ksecattrservice?language=objc)

* [``applicationLabel``](https://developer.apple.com/documentation/security/ksecattrapplicationlabel?language=objc)

* [``issuers``](https://developer.apple.com/documentation/security/ksecmatchissuers?language=objc)

* [``email``](https://developer.apple.com/documentation/security/ksecmatchemailaddressifpresent?language=objc)

* [``subjectContains``](https://developer.apple.com/documentation/security/ksecmatchsubjectcontains?language=objc)

* [``subjectStartsWith``](https://developer.apple.com/documentation/security/ksecmatchsubjectstartswith?language=objc)

* [``subjectEndsWith``](https://developer.apple.com/documentation/security/ksecmatchsubjectendswith?language=objc)

* [``subjectWholeString``](https://developer.apple.com/documentation/security/ksecmatchsubjectwholestring?language=objc)

* [``accessGroup``](https://developer.apple.com/documentation/security/ksecattraccessgroup?language=objc)

* [``securityDomain``](https://developer.apple.com/documentation/security/ksecattrsecuritydomain?language=objc)

* [``generic``](https://developer.apple.com/documentation/security/ksecattrgeneric?language=objc)

* ~~[``syncViewHint``](https://developer.apple.com/documentation/security/ksecattrsyncviewhint?language=objc)~~

* [``protocol``](https://developer.apple.com/documentation/security/ksecattrprotocol?language=objc)

possible values: ``FTP``, ``FTPAccount``, ``HTTP``, ``IRC``, ``NNTP``, ``POP3``, ``SMTP``, ``SOCKS``, ``IMAP``, ``LDAP``, ``AppleTalk``, ``AFP``, ``Telnet``, ``SSH``, ``FTPS``, ``HTTPS``, ``HTTPProxy``, ``HTTPSProxy``, ``FTPProxy``, ``SMB``, ``RTSP``, ``RTSPProxy``, ``DAAP``, ``EPPC``, ``IPP``, ``NNTPS``, ``LDAPS``, ``TelnetS``, ``IMAPS``, ``IRCS``, ``POP3S``

* [`keyType`](https://developer.apple.com/documentation/security/ksecattrkeytype?language=objc)

possible values: ``RSA``, ``DSA``, ``AES``, ``DES``, ``3DES``, ``RC4``, ``RC2``, ``CAST``, ``ECDSA``, ``EC``, ~~``ECSECPrimeRandom``~~     

* [authenticationType](https://developer.apple.com/documentation/security/ksecattrauthenticationtype?language=objc)

possible values: ``NTLM``, ``MSN``, ``DPA``, ``RPA``, ``HTTPBasic``, ``HTTPDigest``, ``HTMLForm``, ``default``

* [`PRF`](https://developer.apple.com/documentation/security/ksecattrprf?language=objc)

possible values: ``SHA1``, ``SHA224``, ``SHA256``, ``SHA384``, ``SHA512``     

* [`synchronizable`](https://developer.apple.com/documentation/security/ksecattrsynchronizable?language=objc)

possible values: ``true``, ``false``, ``any``

* [`accessible`](https://developer.apple.com/documentation/security/ksecattraccessible?language=objc)

possible values: ``whenPasscodeSetThisDeviceOnly``, ``whenUnlockedThisDeviceOnly``, ``whenUnlocked``, ``afterFirstUnlockThisDeviceOnly``, ``afterFirstUnlock``, ``alwaysThisDeviceOnly``, ``always``

* [`authentication`](https://developer.apple.com/documentation/security/ksecuseauthenticationui?language=objc)

possible values: ``allow``, ``fail``, ``skip``

attributes with a binary (`0` or `1`) value:

* [``caseInsensitive``](https://developer.apple.com/documentation/security/ksecmatchcaseinsensitive?language=objc)

* [``diacriticInsensitive``](https://developer.apple.com/documentation/security/ksecmatchdiacriticinsensitive?language=objc)

* [``widthInsensitive``](https://developer.apple.com/documentation/security/ksecmatchwidthinsensitive?language=objc)

* [``trustedOnly``](https://developer.apple.com/documentation/security/ksecmatchtrustedonly?language=objc)

* [``isPermanent``](https://developer.apple.com/documentation/security/ksecattrispermanent?language=objc)

* [``isSensitive``](https://developer.apple.com/documentation/security/ksecattrissensitive?language=objc)

* [``isExtractable``](https://developer.apple.com/documentation/security/ksecattrisextractable?language=objc)

* [``canEncrypt``](https://developer.apple.com/documentation/security/ksecattrcanencrypt?language=objc)

* [``canDecrypt``](https://developer.apple.com/documentation/security/ksecattrcandecrypt?language=objc)

* [``canDerive``](https://developer.apple.com/documentation/security/ksecattrcanderive?language=objc)

* [``canSign``](https://developer.apple.com/documentation/security/ksecattrcansign?language=objc)

* [``canVerify``](https://developer.apple.com/documentation/security/ksecattrcanverify?language=objc)

* [``canWrap``](https://developer.apple.com/documentation/security/ksecattrcanwrap?language=objc)

* [``canUnwrap``](https://developer.apple.com/documentation/security/ksecattrcanunwrap?language=objc)                 

* [``isInvisible``](https://developer.apple.com/documentation/security/ksecattrisinvisible?language=objc)

* [``isNegative``](https://developer.apple.com/documentation/security/ksecattrisnegative?language=objc)

attributes with a numeric value:

* [``port``](https://developer.apple.com/documentation/security/ksecattrport?language=objc)

* [``certificateType``](https://developer.apple.com/documentation/security/ksecattrcertificatetype?language=objc)

* [``certificateEncoding``](https://developer.apple.com/documentation/security/ksecattrcertificateencoding?language=objc)

* [``rounds``](https://developer.apple.com/documentation/security/ksecattrrounds?language=objc)

* [``keySizeInBits``](https://developer.apple.com/documentation/security/ksecattrkeysizeinbits?language=objc)

* [``effectiveKeySize``](https://developer.apple.com/documentation/security/ksecattreffectivekeysize?language=objc)

* [``type``](https://developer.apple.com/documentation/security/ksecattrtype?language=objc)

---

[search attributes](https://developer.apple.com/documentation/security/keychain_services/keychain_items/search_attribute_keys_and_values?language=objc) not supported:

[policy](https://developer.apple.com/documentation/security/ksecmatchpolicy?language=objc)

[itemList](https://developer.apple.com/documentation/security/ksecmatchitemlist?language=objc)

[searchList](https://developer.apple.com/documentation/security/ksecmatchsearchlist?language=objc)

[matchLimit](https://developer.apple.com/documentation/security/ksecmatchlimit?language=objc)

[useItemChain](https://developer.apple.com/documentation/security/ksecuseitemchain?language=objc)

[useKeychain](https://developer.apple.com/documentation/security/ksecusekeychain?language=objc)

[useOperationPrompt](https://developer.apple.com/documentation/security/ksecuseoperationprompt?language=objc)

TODO: [validOnDate](https://developer.apple.com/documentation/security/ksecmatchvalidondate?language=objc)

---

[item attributes](https://developer.apple.com/documentation/security/keychain_services/keychain_items/item_attribute_keys_and_values?language=objc) not supported:

[accessControl](https://developer.apple.com/documentation/security/ksecuseitemlist?language=objc)

[serialNumber](https://developer.apple.com/documentation/security/ksecattrserialnumber?language=objc)

[subjectKeyID](https://developer.apple.com/documentation/security/ksecattrsubjectkeyid?language=objc)

[publicKeyHash](https://developer.apple.com/documentation/security/ksecattrpublickeyhash?language=objc)

[applicationTag](https://developer.apple.com/documentation/security/ksecattrapplicationtag?language=objc)

[salt](https://developer.apple.com/documentation/security/ksecattrsalt?language=objc)

[tokenID](https://developer.apple.com/documentation/security/ksecattrtokenid?language=objc)

TODO: [creationDate](https://developer.apple.com/documentation/security/ksecattrcreationdate?language=objc)

TODO: [modificationDate](https://developer.apple.com/documentation/security/ksecattrmodificationdate?language=objc)

## TODO

* add, delete, modify

* do not run in main thread

* https://github.com/kishikawakatsumi/KeychainAccess
