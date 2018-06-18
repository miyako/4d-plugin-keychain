# 4d-plugin-keychain
Access macOS Keychain

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" /> <img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

### Releases


![preemption xx](https://user-images.githubusercontent.com/1725068/41327179-4e839948-6efd-11e8-982b-a670d511e04f.png)

* ``Keychain Delete items``

## Syntax

```
result:=Keychain Search items (request;option;data)
```

Parameter|Type|Description
------------|------------|----
value|TEXT|``json``
option|LONGINT|
data|ARRAY BLOB|
result|TEXT|``json``

``option`` should be a combination of  

* ``Keychain return data``
* ``Keychain return attributes``
* ``Keychain data as index``
* ``Keychain data as base64``
* ``Keychain data as hex``

``Keychain data as*`` specifies how binary data should be returned; as index of the ``data`` array, as base64 or hex. 

``Keychain return attributes`` requests public attributes, in which case ``result`` is a collection. 

``Keychain return data`` requests secure attributes (UI is displayed), in which case ``result`` is an object.
