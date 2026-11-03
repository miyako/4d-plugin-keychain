![version](https://img.shields.io/badge/version-19%2B-5682DF)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-keychain)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-keychain/total)

# 4d-plugin-keychain

Read items from the macOS Keychain. The plugin turns a 4D `Object` into a Keychain Services query, runs it with `SecItemCopyMatching`, and returns the matching items' attributes (and, on request, the secret itself) as an `Object` holding a `Collection`. Writing, modifying and deleting items are not implemented.

Command | Returns | Purpose
---|---|---
[Keychain Search items](#keychain-search-items) | `Object` | Search the keychain for generic passwords, internet passwords, certificates, keys or identities

**Platforms:** macOS only (Apple silicon and Intel). The bundle's Windows binary is a placeholder from the plugin template; the command is not implemented on Windows.

Releases: [GitHub releases](https://github.com/miyako/4d-plugin-keychain/releases)

---

## Requirements & platform notes

- **macOS 11.5 or later** for builds from this repository (deployment target of the Xcode project).
- **Keychain access prompt.** The first time 4D reads an item that belongs to another application, macOS asks the user to allow access. After that, the 4D application is authorised for that item.

  <img width="434" alt="" src="https://user-images.githubusercontent.com/1725068/188376499-13275811-fa4d-477c-94f8-fdbb57d262b9.png">

- **Query values must be text.** Apart from `data` (Boolean), the plugin only reads query properties whose value is a `Text`. A number or Boolean is silently ignored: write `$query.port:="5900"` and `$query.caseInsensitive:="1"`, not `5900` or `True`.
- **`data:True` returns one item only.** Asking for the secret switches the search to "first match only". Without `data`, every match is returned, but without secrets.
- **Results are flat.** Each attribute of each matching item becomes a separate single-property object in `items`. With several matches, all attributes are in one collection with nothing marking where one item ends and the next begins (see [Keychain Search items](#keychain-search-items)).
- **Thread-safe.** The command is declared thread-safe and can be called from a preemptive process.
- **Secrets are returned as Base64 text** in a 4D object. Clear the variables that hold them when you are done, and don't log them.

---

## Keychain Search items

### Syntax

```
status:=Keychain Search items ( query )
```

Parameter | Type | Description
---|---|---
`query` | Object | Search criteria (see the tables below)
Result | Object | `error`, `errorMessage` and `items` (see [Result](#result))

### Description

The plugin builds a `SecItemCopyMatching` query from the properties of `query`. Property names the plugin doesn't know are ignored, and so are known names with a value it doesn't recognise (for example `class:"password"`). The plugin always asks for item attributes (`kSecReturnAttributes`).

#### Controlling the search

Property | Type | Description
---|---|---
`data` | Boolean | `True`: return the secret (`kSecReturnData`) of the **first** match only (`kSecMatchLimitOne`). `False` or omitted: return the attributes of **all** matches (`kSecMatchLimitAll`), no secret.
`class` | Text | [Item class](https://developer.apple.com/documentation/security/ksecclass?language=objc): `genericPassword`, `internetPassword`, `certificate`, `key`, `identity`
`authentication` | Text | [`kSecUseAuthenticationUI`](https://developer.apple.com/documentation/security/ksecuseauthenticationui?language=objc): `allow` (show the access prompt when needed), `fail`, `skip` (leave out items that need a prompt). Deprecated by Apple since macOS 11, still honoured.
`synchronizable` | Text | [`kSecAttrSynchronizable`](https://developer.apple.com/documentation/security/ksecattrsynchronizable?language=objc): `"true"`, `"false"` or `"any"` (as text)

#### Text attributes

Each of these is passed to the query as the text you give.

Property | Keychain key
---|---
`path` | [`kSecAttrPath`](https://developer.apple.com/documentation/security/ksecattrpath?language=objc)
`server` | [`kSecAttrServer`](https://developer.apple.com/documentation/security/ksecattrserver?language=objc)
`account` | [`kSecAttrAccount`](https://developer.apple.com/documentation/security/ksecattraccount?language=objc)
`label` | [`kSecAttrLabel`](https://developer.apple.com/documentation/security/ksecattrlabel?language=objc)
`comment` | [`kSecAttrComment`](https://developer.apple.com/documentation/security/ksecattrcomment?language=objc)
`description` | [`kSecAttrDescription`](https://developer.apple.com/documentation/security/ksecattrdescription?language=objc)
`service` | [`kSecAttrService`](https://developer.apple.com/documentation/security/ksecattrservice?language=objc)
`applicationLabel` | [`kSecAttrApplicationLabel`](https://developer.apple.com/documentation/security/ksecattrapplicationlabel?language=objc)
`issuers` | [`kSecMatchIssuers`](https://developer.apple.com/documentation/security/ksecmatchissuers?language=objc)
`email` | [`kSecMatchEmailAddressIfPresent`](https://developer.apple.com/documentation/security/ksecmatchemailaddressifpresent?language=objc)
`subjectContains` | [`kSecMatchSubjectContains`](https://developer.apple.com/documentation/security/ksecmatchsubjectcontains?language=objc)
`subjectStartsWith` | [`kSecMatchSubjectStartsWith`](https://developer.apple.com/documentation/security/ksecmatchsubjectstartswith?language=objc)
`subjectEndsWith` | [`kSecMatchSubjectEndsWith`](https://developer.apple.com/documentation/security/ksecmatchsubjectendswith?language=objc)
`subjectWholeString` | [`kSecMatchSubjectWholeString`](https://developer.apple.com/documentation/security/ksecmatchsubjectwholestring?language=objc)
`accessGroup` | [`kSecAttrAccessGroup`](https://developer.apple.com/documentation/security/ksecattraccessgroup?language=objc)
`securityDomain` | [`kSecAttrSecurityDomain`](https://developer.apple.com/documentation/security/ksecattrsecuritydomain?language=objc)
`generic` | [`kSecAttrGeneric`](https://developer.apple.com/documentation/security/ksecattrgeneric?language=objc)

#### Attributes with a list of values

Property | Values
---|---
`keyClass` | [`kSecAttrKeyClass`](https://developer.apple.com/documentation/security/ksecattrkeyclass?language=objc): `public`, `private`, `symmetric`
`keyType` | [`kSecAttrKeyType`](https://developer.apple.com/documentation/security/ksecattrkeytype?language=objc): `RSA`, `DSA`, `AES`, `DES`, `3DES`, `RC4`, `RC2`, `CAST`, `ECDSA`, `EC`
`protocol` | [`kSecAttrProtocol`](https://developer.apple.com/documentation/security/ksecattrprotocol?language=objc): `FTP`, `FTPAccount`, `HTTP`, `IRC`, `NNTP`, `POP3`, `SMTP`, `SOCKS`, `IMAP`, `LDAP`, `AppleTalk`, `AFP`, `Telnet`, `SSH`, `FTPS`, `HTTPS`, `HTTPProxy`, `HTTPSProxy`, `FTPProxy`, `SMB`, `RTSP`, `RTSPProxy`, `DAAP`, `EPPC`, `IPP`, `NNTPS`, `LDAPS`, `TelnetS`, `IMAPS`, `IRCS`, `POP3S`

#### Numeric attributes

Pass the number as text (`"5900"`); it is converted to an integer.

Property | Keychain key
---|---
`port` | [`kSecAttrPort`](https://developer.apple.com/documentation/security/ksecattrport?language=objc)
`certificateType` | [`kSecAttrCertificateType`](https://developer.apple.com/documentation/security/ksecattrcertificatetype?language=objc)
`certificateEncoding` | [`kSecAttrCertificateEncoding`](https://developer.apple.com/documentation/security/ksecattrcertificateencoding?language=objc)
`rounds` | [`kSecAttrRounds`](https://developer.apple.com/documentation/security/ksecattrrounds?language=objc)
`keySizeInBits` | [`kSecAttrKeySizeInBits`](https://developer.apple.com/documentation/security/ksecattrkeysizeinbits?language=objc)
`effectiveKeySize` | [`kSecAttrEffectiveKeySize`](https://developer.apple.com/documentation/security/ksecattreffectivekeysize?language=objc)
`type` | [`kSecAttrType`](https://developer.apple.com/documentation/security/ksecattrtype?language=objc)

#### On/off attributes

Pass `"1"` (on) or `"0"` (off) as text. Any text that doesn't start with a non-zero number counts as off.

Property | Keychain key actually set
---|---
`caseInsensitive` | [`kSecMatchCaseInsensitive`](https://developer.apple.com/documentation/security/ksecmatchcaseinsensitive?language=objc)
`diacriticInsensitive` | [`kSecMatchDiacriticInsensitive`](https://developer.apple.com/documentation/security/ksecmatchdiacriticinsensitive?language=objc)
`widthInsensitive` | [`kSecMatchWidthInsensitive`](https://developer.apple.com/documentation/security/ksecmatchwidthinsensitive?language=objc)
`trustedOnly` | [`kSecMatchTrustedOnly`](https://developer.apple.com/documentation/security/ksecmatchtrustedonly?language=objc)
`isPermanent` | **`kSecAttrIsSensitive`** (shifted, see below)
`isSensitive` | **`kSecAttrIsExtractable`**
`isExtractable` | **`kSecAttrCanEncrypt`**
`canEncrypt` | **`kSecAttrCanDecrypt`**
`canDecrypt` | **`kSecAttrCanDerive`**
`canDerive` | **`kSecAttrCanSign`**
`canSign` | **`kSecAttrCanVerify`**
`canVerify` | **`kSecAttrCanWrap`**
`canWrap` | **`kSecAttrCanUnwrap`**
`canUnwrap` | **`kSecAttrIsInvisible`**
`isInvisible` | **`kSecAttrIsNegative`**
`isNegative` | ignored

From `isPermanent` on, the plugin's list of names and its list of Keychain keys are one entry out of step (the key for `isPermanent` is missing), so each name sets the key meant for the next name. The table shows what the current code really does. Before the fixes in this version, `isNegative` made the command fail without returning a result; it is now ignored.

#### Properties that have no effect

- **`authenticationType`** (`NTLM`, `MSN`, `DPA`, `RPA`, `HTTPBasic`, `HTTPDigest`, `HTMLForm`, `default`) and **`PRF`** (`SHA1`, `SHA224`, `SHA256`, `SHA384`, `SHA512`): the code looks for these values under the name `keyClass` instead, so both properties are ignored. A `keyClass` of `SHA1` … `SHA512` sets `kSecAttrKeyClass` to a PRF constant, which matches nothing.
- **`accessible`**: implemented but never called, so it is ignored.
- Not supported at all: `policy`, `itemList`, `searchList`, `matchLimit`, `useItemChain`, `useKeychain`, `useOperationPrompt`, `validOnDate`, `accessControl`, `serialNumber`, `subjectKeyID`, `publicKeyHash`, `applicationTag`, `salt`, `tokenID`, `creationDate`, `modificationDate`.

#### Result

Property | Type | Description
---|---|---
`error` | Number | The `OSStatus` of `SecItemCopyMatching`. `0` on success; for example `-25300` (`errSecItemNotFound`) when nothing matches.
`errorMessage` | Text | macOS's text for that status (`SecCopyErrorMessageString`), for example `"No error."` or `"The specified item could not be found in the keychain."`. Set on success too.
`items` | Collection | One object per attribute, each with a single property (see below). Empty when the search fails.

The property names in `items` are the raw Keychain attribute keys, mostly four-character codes: `labl` (label), `acct` (account), `svce` (service), `srvr` (server), `port`, `ptcl` (protocol), `desc`, `icmt` (comment), `path`, `agrp` (access group), `cdat`/`mdat` (creation/modification date), `class`, and `v_Data` for the secret when `data` is `True`. Values are converted as follows:

Keychain value | 4D value
---|---
number | Number
string | Text
date | Text, UTC, `yyyy-MM-ddTHH:mm:ssZ`
data (secret, certificate subject, key hash, …) | Text, Base64
null | Null
anything else (Boolean, array, …) | left out

Because the list is flat, use `extract` (as the test method below does) or a `For each` loop to pick the property you want. With several matching items, the attributes of all of them are in the same collection.

If `query` is missing or isn't an object, the result is an empty object (no `error`, no `items`).

### Example

Create a password in Keychain Access first:

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188371613-8844a4d6-ed4b-4945-b5ee-837b4900a67d.png">

From the plugin's own test method (`TEST_2.4dm`), reading a generic password and its secret:

```4d
var $query : Object

$query:=New object:C1471

$query.class:="genericPassword"
//possible values = genericPassword,internetPassword,certificate,key,identity
//https://developer.apple.com/documentation/security/keychain_services/keychain_items/item_class_keys_and_values

$query.authentication:="allow"
$query.account:="miyako"
$query.label:="TEST"
$query.data:=True:C214

//possible attributes for internetPassword = path,server,account,label,comment,description,port
//possible attributes for genericPassword += service
//possible attributes for certificate = label
//possible attributes for key = label,applicationLabel,keyClass[=public,private,symmetric]

$status:=Keychain Search items($query)
```

From `TEST_3.4dm`, an internet password. The server name includes the port, so the item's `port` attribute is `0`:

```4d
$query.class:="internetPassword"
$query.authentication:="allow"
$query.account:="miyako"
$query.label:="MacBook Pro 2019"
$query.server:="100.64.1.26:5900"  //port is 0 in this case
$query.data:=True:C214

$status:=Keychain Search items($query)
```

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188371948-999b6c2e-7773-4a04-8da3-527f5f15ea3b.png">

<img width="534" alt="" src="https://user-images.githubusercontent.com/1725068/188376067-2291d625-cebf-4809-80b7-ad35e328eddd.png">

From `TEST_1.4dm`, certificates. `caseInsensitive` is given as a number here, so it is ignored (it would have to be `"0"`):

```4d
var $query : Object

$query:=New object:C1471

$query.class:="certificate"
$query.subjectContains:="Apple"
$query.caseInsensitive:=0
$query.data:=False:C215  //default

$status:=Keychain Search items($query)

$labl:=$status.items.extract("labl")

$query:=New object:C1471

$query.class:="certificate"
$query.label:="Apple Worldwide Developer Relations Certification Authority"
$query.data:=True:C214

$status:=Keychain Search items($query)
```

Checking for an error and picking the secret out of the flat result:

```4d
var $query; $status; $item : Object
var $secret : Text

$query:=New object
$query.class:="genericPassword"
$query.service:="my-service"
$query.account:="miyako"
$query.authentication:="allow"
$query.data:=True

$status:=Keychain Search items($query)

If ($status.error=0)
	For each ($item; $status.items)
		If ($item.v_Data#Null)
			$secret:=$item.v_Data  // Base64 text
		End if
	End for each
Else
	ALERT($status.errorMessage)
End if
```

`$secret` holds the secret Base64-encoded. Decode it with 4D's Base64 commands and, for a text password, convert it from UTF-8 (check the Language Reference for your version for the exact syntax).

---

## Error handling & troubleshooting

- **Nothing found although the item exists.** Check that every criterion is text: numbers and Booleans other than `data` are dropped without warning, so the search may be wider or narrower than you think. Also check `authentication`: with `skip`, items that need the access prompt are left out.
- **Only one item comes back.** `data:True` limits the search to the first match. Search without `data` to list all matches, then narrow the query and search again with `data:True`.
- **`error` is `-25300`.** `errSecItemNotFound`: no item matches the whole query. `items` is empty.
- **`error` is `0` and `errorMessage` is "No error."** That's success; the message is always filled in.
- **Can't tell items apart.** All attributes of all matches are in one flat `items` collection. Make the query specific enough to match one item, or use `extract` on a single attribute (one value per item).
- **`authenticationType`, `PRF` or `accessible` has no effect.** Known limitation of the current code (see [Keychain Search items](#keychain-search-items)).
- **On/off attributes from `isPermanent` on set the wrong key.** Known limitation; see the on/off table in [Keychain Search items](#keychain-search-items).
- **Boolean attributes are missing from the result.** Keychain Boolean values (and arrays) are not converted and don't appear in `items`.
- **An access prompt appears.** Expected the first time 4D reads an item created by another application. In unattended processes, try `authentication:"fail"` or `"skip"`; whether macOS honours them for the classic keychain access prompt depends on the item and OS version (not verified here), so authorise 4D for the item beforehand.

---

## Quick reference

```4d
$query:=New object("class"; "internetPassword"; "server"; "example.com"; "account"; "me"; "port"; "443"; "authentication"; "allow")
$status:=Keychain Search items($query)  // all matches, attributes only
$labels:=$status.items.extract("labl")

$query.data:=True
$status:=Keychain Search items($query)  // first match, with v_Data (Base64)
If ($status.error#0)
	ALERT($status.errorMessage)
End if
```

---

## TODO

* add, delete, modify
* https://github.com/kishikawakatsumi/KeychainAccess
