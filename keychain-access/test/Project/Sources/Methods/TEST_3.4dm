//%attributes = {}
var $query : Object

$query:=New object:C1471

$query.class:="internetPassword"
//possible values = genericPassword,internetPassword,certificate,key,identity
//https://developer.apple.com/documentation/security/keychain_services/keychain_items/item_class_keys_and_values

$query.authentication:="allow"
$query.account:="miyako"
$query.label:="MacBook Pro 2019"
$query.server:="100.64.1.26:5900"  //port is 0 in this case
$query.data:=True:C214

//possible attributes for internetPassword = path,server,account,label,comment,description,port
//possible attributes for genericPassword += service
//possible attributes for certificate = label
//possible attributes for key = label,applicationLabel,keyClass[=public,private,symmetric]

$status:=Keychain Search items($query)

