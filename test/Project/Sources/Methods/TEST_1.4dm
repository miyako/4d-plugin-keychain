//%attributes = {}
/*

get certificate from keychain

*/

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