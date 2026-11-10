# Native string ownership evidence

This is a bounded first-party ownership audit, not a security review or a
keychain integration test. All fixtures are synthetic public text. No actual
keychain, credential API, password, identity, certificate, clipboard, private
file, UI, or device is accessed.

## Reproduction and immutable inputs

On macOS:

```sh
python3 tests/ownership/run.py -v
```

The runner compiles the **actual** `support/4DPlugin-JSON.cpp`, all four
`4D Plugin API/Classes/*.cpp` wrappers and the **complete unchanged**
`4D Plugin API/4DPluginAPI.c`. It replaces only `gCall4D` with a synthetic
engine, and supplies a forbidden-dispatch `PluginMain` stub. It does **not**
compile `4DPlugin-Keychain-Access.cpp`, link Security or jsoncpp, run Xcode,
load the plugin, execute the credential dispatcher, or invoke repository
publication scripts. `nm` and `otool` reject direct Security dependencies.

Baseline: `7adea02808aded8519d1f54dcdc8a8373b435bca`, verified to be `master`
at audit intake. Export just the native test inputs, not the repository's
sample database or binary bundle:

```sh
mkdir baseline
git archive 7adea02808aded8519d1f54dcdc8a8373b435bca -- \
  '4D Plugin API' support | tar -x -C baseline
OWNERSHIP_SOURCE_ROOT="$PWD/baseline" python3 tests/ownership/run.py -v
```

Use the **same final runner and host** for baseline and fixed sources.
Compilation must succeed in both cases; baseline exits 1 for the named
assertions below, not for a compilation failure. Temporary executables are
removed by the runner. Export outside tracked source paths when collecting
evidence.

The exact local SDK C blob is
`62f745e91d81ef2c3c901c656ea8e175d3b173e8` (SHA-256
`5bac571c5c4cafd280fa7f23d775458a23701287e8735be27d4a2190002f4e3f`).
Its last path-changing repository commit is
`371d7ea7bf238e939f4fa5458f67697df9a317bf`. The runner independently pins
the Git blob IDs of this C file and all five engine/type/API headers.
The source's `v14` and headers' `v13` comments are **not** verified upstream
release identifiers. Local immutable bytes, not a presumed upstream version,
are the contract for this regression.

Guidance: `miyako/skills` at
`e6b492fcb81e5e1a7f7db723594b420e63bf6eb9`,
`4d-skills/skills/4dplugin/SKILL.md`.
The synthetic host's string/handle tracking and scalar/array controls were
adapted after reading immutable
`miyako/4d-plugin-jpegoptim` at
`e49adcd32bd5500cf3de680faa3649b32463ff5e`,
`.github/tests/{ownership_host.mm,test_ownership.py}`.
The alternate `miyako/4d-plugin-jsmin` reference at
`d12035643f874ec57ae634194ec0f1206b05a5f2` was tree-verified at
`tests/ownership/{host.mm,run.py,README.md}`; its implementation and results
were not substituted for this target's evidence.

## Complete tracked native inventory and reachability

| Surface | Actual reachability and ownership conclusion |
|---|---|
| `4DPlugin-Keychain-Access.cpp`, `.h` | Sole entry/dispatcher and credential implementation. Selector 1 maps to `Keychain_Search_items`; no other plugin selectors. Static inspection only. `parseObject` calls `ob_stringify`; search reads Boolean `data` via `ob_get_b`; output builders use `ob_set_n`, `ob_set_s`, `ob_set_0`, `ob_set_c`. `appendObjectToCollection` gives each new object to a temporary variable, collection storage retains it, and the temporary is cleared. `PA_ReturnObject` hands the new result to 4D. |
| Native query helpers in that entry file | `set_param_class`, `set_param_authentication`, `set_param_accessible`, `set_param_authentication_type`, `set_param_prf`, `set_param_protocol`, `set_param_key_type`, `set_param_key_class`, `set_param_string`, `set_param_boolean`, `set_param_synchronizable`, `set_param_numeric` manipulate Foundation dictionaries/strings. `set_param_accessible` is not called by the dispatch chain. Owned query-key/value NSStrings are released after the chain. Dictionary/string ownership was inspected statically; no Security calls were executed. Existing exception/resource behavior outside the shared helpers is not certified. |
| `support/4DPlugin-JSON.cpp`, `.h` | Compiled by Xcode even for helpers without current entry call sites. Every setter overload, scalar/reference getter, `ob_is_defined`, `ob_stringify`, narrow/wide conversion, and their internal helpers was inventoried. Successful scalar and mismatched getter results need cleanup; matching object/collection getter results transfer their owned reference. String setters already start Undefined, avoiding initial empty-string overwrite. Setters consume supplied object/collection/picture references as before. No JSON Parse call exists in this helper. |
| `4D Plugin API/Classes/C_TEXT.cpp`, `.h`, `C_TYPES.h` | Xcode-compiled; used through C_BLOB's Base64 output in the entry. UTF-16 getters are copied into C++ storage. Parameter and scalar returns use the engine content setter, not descriptor transfer. UTF-8/UTF-16 conversion lengths are explicit. Existing deleted copy operations preserve the raw C++ owner. Synthetic scalar controls exercise this actual source. |
| `4D Plugin API/Classes/C_BLOB.cpp`, `.h` | Xcode-compiled; `setBytes` / `toB64Text` reach C_TEXT from entry. Other hex/Base64/blob parameter helpers are compiled but dormant from this dispatcher. No PA_Unistring owner is created here. Non-string CBytes allocation/empty-buffer behavior is outside this string audit. |
| `4D Plugin API/Classes/ARRAY_TEXT.cpp`, `.h` | Tracked but not compiled or included by the actual plugin targets. Explicitly included in synthetic controls, not silently excluded because the manifest is scalar-only. Getter descriptors are copied; each accepted output gets an independent descriptor, with no post-transfer disposal. Undefined creation / allocation-failure behavior is deferred below; normal-array tests are not OOM certification. |
| `4D Plugin API/Classes/C_LONGINT.cpp`, `.h` | Tracked, not compiled/included by the plugin. Integer-only wrapper, no string allocation. Included in the standalone compilation inventory, not used as evidence about other helpers. |
| `4D Plugin API/4DPluginAPI.c`, `.h`, `PublicTypes.h`, `PrivateTypes.h`, `Flags.h`, `EntryPoints.h`, `4DPluginAPI.def` | Bundled SDK engine/types/export boundary. Both projects compile this C implementation; `.def` exports `FourDPackex`, which reaches `PluginMain`. Xcode compiles SDK/entry/helpers as Objective-C++. All bytes and modes remain unchanged; SDK-only ownership ambiguities are not silently fixed. |
| `include/json/{allocator,assertions,config,forwards,json,json_features,reader,value,version,writer}.h` | Bundled jsoncpp third-party headers, reachable from the entry's `json/json.h`; static libraries `a/libjsoncpp.a`, `lib/windows/jsoncpp.lib`, `lib/windows64/jsoncpp.lib` are preserved. No production edits or third-party certification. The synthetic helper build needs none of them. |

`manifest.json` and the bundled resource copy each declare only
`Keychain Search items(&J):J`, thread-safe, object parameter/result.
There is no manifest text-array command, but the array wrapper and internal
scalar call sites were inspected independently. `constants.json` / `.xlf`
do not add entry points.

Xcode's Sources phase lists exactly SDK, entry, JSON helper, C_TEXT and C_BLOB.
Its include groups resolve the root SDK/Classes and support headers;
`include/**` resolves jsoncpp. Its link phase includes Cocoa/Carbon/Security
and jsoncpp. The Visual Studio project lists only SDK and entry source files;
the macOS command is not implemented on Windows, and the checked-in Windows
binary is a template placeholder. This audit does not repair or certify that
Windows topology.

Existing release/bump workflows are manual-only and unchanged. Xcode has a
Deployment-only signing/notarization script, which is not executed. The new
path-scoped `macos-latest` workflow compiles only the synthetic host, is
read-only, and has no packaging, signing, release, upload, tag or manual
dispatch. The existing workflow contract test was narrowly updated to allow
this third, non-publication workflow while retaining both manual contracts.

## Source-derived contracts

Baseline SDK line references:

| API / lines | Contract and caller treatment |
|---|---|
| `PA_CreateUnistring` 265-273 | Engine creates caller-owned UTF-16 descriptor. Check error before adoption; failed output is unspecified. Shared helpers now throw a fixed, non-data-bearing `std::runtime_error` without publishing/disposing a poisoned failed descriptor. |
| `PA_SetStringVariable` 4254-4259 | Shallow descriptor assignment, not a copy or retain. Start string variables Undefined; clearing the variable releases the adopted allocation exactly once. |
| `PA_GetObjectProperty` 636-650; collection getter 586-602 | SDK explicitly requires clearing successful returned variables. Shared scalar getters copy/read then clear, including mismatches. Matching object/collection getters transfer the owned result without clearing; consumers must dispose it. Check error before type/content/cleanup. |
| `PA_SetObjectProperty` 654-667 | Forwards supplied variable to engine storage. Synthetic storage independently copies string content / retains reference values before caller cleanup. This matches the shared setter's pre-existing clear-after-store contract; it is not proof of actual 4D engine internals. |
| `PA_GetStringVariable` 4105-4119 | Engine-backed descriptor access. Treat as a borrowed view of the owning variable for these string-kind calls; copy before clearing the variable. Conversion errors are checked before reading the output. |
| `PA_ExecuteCommandByID` 5778-5813 | Allocates argument-pointer handle, invokes synchronously, disposes that handle. Allocation failure returns zeroed raw kind 0, not Undefined. `UnifyVarKind` 1713-1736 maps raw 0 to string. Error, not normalized kind, gates all result inspection and cleanup. Borrowed stringify argument is detached without clearing; successful return is owned and cleared. |
| `PA_ExecuteMethodByID` 5710 onward | Failure initializes Undefined, unlike command failure. Control verifies the distinction without reading/clearing command failure output. |
| `PA_GetStringInArray` 4738-4755 | Borrowed descriptor. ARRAY_TEXT copies content before any array replacement/cleanup. |
| `PA_SetStringInArray` 4933-4947 | Accepts Unicode array, non-null handle and descriptor, and `0 <= index <= count`; disposes old descriptor and shallow-transfers new descriptor. Index 0 is accepted. Rejected transfer retains caller ownership. |
| `PA_SetUnistring` 289-299 / `PA_ReturnString` 2600-2603 | Content setter / return forwarding, distinct from array transfer. Synthetic controls retain independent parameter/return content after source disposal. |
| `PA_ReturnObject` 2641-2645 | Direct reference handoff to host result; not a new retained copy. Entry's result is intentionally not locally disposed afterward. |

SDK exceptions now use explicit native exceptions from shared helpers; output
buffers and stored properties remain unchanged on tested failed creates,
gets, conversion reads, command dispatch and writes. The entry's existing
catch-all policy is unchanged and does not expose a new 4D error schema.
The native exception channel is not a claim that every outer command failure
is surfaced to a 4D caller.

## Differential results

Local platform: macOS arm64, Apple clang 21.0.0, C++14 Objective-C++.
ASan and UBSan are enabled; alignment instrumentation is excluded for legacy
packed SDK types. macOS LeakSanitizer is disabled; explicit string-allocation,
handle-allocation and object/collection-reference accounting is mandatory.
Every passing process ends `strings=0 handles=0 refs=0`.

The same final production sources and unchanged SDK pass **25 groups / 200
executions**: four fixtures (ASCII, U+65E5, U+1F600, empty), each with allocated
and null empty descriptors. Baseline fails **151/200 executions in 19 groups**;
49 executions retain their existing correct behavior.

| Exact baseline case | Failed executions (out of 8) |
|---|---:|
| `get-a`, `get-b`, `get-n` | 8 each |
| `mismatch-a`, `mismatch-b`, `mismatch-n`, `mismatch-o`, `mismatch-c` | 8 each |
| `get-failure`, `has-failure`, `read-failure` | 8 each |
| `stringify-failure`, `stringify-handle-failure`, `stringify-read-failure` | 8 each |
| `create-value-failure`, `create-key-failure`, `getter-create-failure`, `set-failure` | 8 each |
| `setters` | 7 (the null-empty scalar case has no heap allocation to leak) |

All of these exact groups pass after the fix. Successful `get-o` / `get-c`,
`stringify`, `stringify-mismatch`, scalar content controls and normal array
controls also pass in both trees.

Assertions distinguish omitted scalar clears even for non-allocating results,
wrong-kind retained references, unbalanced owning string results, poisoned
failed outputs, raw-kind0 normalization, initial/create-key failure without
caller data loss, old-property preservation on failed writes, exact UTF-8
bytes and UTF-16 code units, terminators, and supplementary surrogate pairs.
Array controls cover index 0, independent output allocations, borrowed-copy
survival across replacement, repeated output, shrink/clear, and negative,
past-end, wrong-type, missing-handle and null-source rejections. Owning
object/collection handoffs survive disposal of the original container;
stringify's borrowed object survives both success and failure.

## Explicit limits and deferrals

- No real 4D host, Windows execution, plugin loading, real keychain operation,
  credential dispatcher or Security runtime boundary is tested or certified.
  The original Windows Poppler crash is not reproduced by this repository.
- Normal-array success does not certify SDK resize OOM: the unchanged
  `PA_ResizeArray` updates count after failed handle resize. The dormant
  ARRAY_TEXT void parameter API also lacks a safe error-publication contract
  for initial create/resize failure. No SDK production edits or fake OOM
  success fallback are made.
- SDK-only `PA_CreateObject`, `PA_HasObjectProperty` and related wrappers
  inspect their own command outputs internally. Shared helpers now gate
  their own adoption on returned errors, but this does not repair or certify
  those SDK internals under allocation failure. SDK JSON Parse input in the
  old-version object-creation branch is disposed after its synchronous call;
  no first-party JSON Parse call was introduced.
- No CPicture wrapper exists in this repository. SDK picture-data strings,
  application-path strings and C_POINTER metadata have no relevant
  first-party string-owner call site here; ambiguous SDK-only ownership is
  deferred. The pre-existing shared picture setter is preserved, not certified
  by synthetic picture tests.
- Non-string CBytes allocation/empty-buffer behavior and general Foundation
  exception unwinding in the credential entry are outside this bounded fix.
  CoreFoundation/Windows conversion allocation failures beyond the checked
  PA_CreateUnistring boundary are not fault-injected or certified.
- The engine's deep-copy/reference-retain and borrowed string-view semantics
  are explicit host-shim contracts derived from SDK forwarding and existing
  caller lifetimes. They are not independent observation of a real 4D host.
