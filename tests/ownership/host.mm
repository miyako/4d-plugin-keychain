// Synthetic engine boundary; never link the credential dispatcher or Security.
#import <Carbon/Carbon.h>
#include "4DPluginAPI.h"
#include "PrivateTypes.h"
#include "EntryPoints.h"
#include "C_TEXT.h"
#include "ARRAY_TEXT.h"
#include "support/4DPlugin-JSON.h"
#include <codecvt>
#include <cstring>
#include <iostream>
#include <locale>
#include <stdexcept>

static std::map<PA_Unichar*, size_t> strings;
static std::map<PA_Handle, size_t> handles;
struct Node { int refs = 1; std::map<CUTF16String, PA_Variable> values; };
static std::map<void*, Node*> nodes;
static bool nullEmpty, failHandle, failGet, failHas, failCommand, failRead, failSet;
static int failCreate, creations, clearCalls;
static PA_Variable commandValue;

static void check(bool valid, const char* message)
{
    if (!valid) { std::cerr << message << "\n"; std::exit(1); }
}

static CUTF16String utf16(const char* text)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> codec;
    auto converted = codec.from_bytes(text);
    return CUTF16String(reinterpret_cast<const PA_Unichar*>(converted.data()), converted.size());
}

static PA_Unistring makeString(const PA_Unichar* text)
{
    size_t units = 0;
    while (text && text[units]) ++units;
    PA_Unistring value = {};
    if (units || !nullEmpty) {
        value.fString = new PA_Unichar[units + 1];
        if (units) std::memcpy(value.fString, text, units * sizeof(PA_Unichar));
        value.fString[units] = 0;
        value.fLength = static_cast<PA_long32>(units);
        strings[value.fString] = units;
    }
    return value;
}

static void disposeString(PA_Unistring value)
{
    if (!value.fString) { check(value.fLength == 0, "null string length"); return; }
    check(strings.count(value.fString), "duplicate/unowned string disposal");
    check(strings.at(value.fString) == static_cast<size_t>(value.fLength), "allocation length");
    strings.erase(value.fString);
    delete[] value.fString;
}

static CUTF16String text(PA_Unistring value)
{
    if (!value.fString) { check(value.fLength == 0, "null text length"); return {}; }
    check(strings.count(value.fString), "read of failed/released string output");
    check(strings.at(value.fString) == static_cast<size_t>(value.fLength), "exact UTF16 units");
    check(value.fString[value.fLength] == 0, "UTF16 terminator");
    return CUTF16String(value.fString, value.fLength);
}

static void clear(PA_Variable*);

static void* makeNode()
{
    auto node = new Node;
    nodes[node] = node;
    return node;
}

static void releaseNode(void* ref)
{
    if (!ref) return;
    check(nodes.count(ref), "release dead object/collection");
    auto node = nodes.at(ref);
    if (--node->refs == 0) {
        nodes.erase(ref);
        for (auto& entry : node->values) clear(&entry.second);
        delete node;
    }
}

static PA_Variable copy(PA_Variable value)
{
    if (value.fType == eVK_Unistring) {
        auto u = text(value.uValue.fString);
        value.uValue.fString = makeString(u.c_str());
    } else if (value.fType == eVK_Object || value.fType == eVK_Collection) {
        auto ref = value.uValue.fObject;
        if (ref) {
            check(nodes.count(ref), "retain dead object/collection");
            ++nodes.at(ref)->refs;
        }
    }
    return value;
}

static void clear(PA_Variable* value)
{
    if (value->fType == eVK_Unistring) disposeString(value->uValue.fString);
    else if (value->fType == eVK_Object || value->fType == eVK_Collection)
        releaseNode(value->uValue.fObject);
    else if (value->fType == eVK_ArrayUnicode) {
        auto h = value->uValue.fArray.fData;
        check(handles.count(h), "clear invalid array handle");
        auto values = reinterpret_cast<PA_Unistring*>(*h);
        for (PA_long32 i = 0; i <= value->uValue.fArray.fNbElements; ++i)
            disposeString(values[i]);
        std::free(*h);
        delete h;
        handles.erase(h);
    } else check(value->fType == eVK_Undefined || value->fType == eVK_Boolean ||
                 value->fType == eVK_Real || value->fType == eVK_Longint ||
                 value->fType == static_cast<char>(eVK_Null),
                 "clear failed/raw-kind0/unsupported output");
    value->fType = eVK_Undefined;
    value->fFiller = 0;
}

static void poison(PA_Variable* value)
{
    value->fType = eVK_Unistring;
    value->uValue.fString = {};
    value->uValue.fString.fString = reinterpret_cast<PA_Unichar*>(1);
    value->uValue.fString.fLength = 100;
}

static void host(short selector, EngineBlock* eb)
{
    switch (selector) {
    case EX_CREATE_UNISTRING:
        if (++creations == failCreate) {
            eb->fUniString1 = {};
            eb->fUniString1.fString = reinterpret_cast<PA_Unichar*>(1);
            eb->fUniString1.fLength = 100;
            eb->fError = eER_IncompatibleTypes;
        } else eb->fUniString1 = makeString(reinterpret_cast<PA_Unichar*>(eb->fHandle));
        break;
    case EX_DISPOSE_UNISTRING: disposeString(eb->fUniString1); break;
    case EX_SET_UNISTRING: {
        auto replacement = makeString(reinterpret_cast<PA_Unichar*>(eb->fHandle));
        disposeString(eb->fUniString1);
        eb->fUniString1 = replacement;
        break;
    }
    case EX_VARIABLE_TO_STRING: {
        auto value = reinterpret_cast<PA_Variable*>(eb->fHandle);
        check(value->fType == eVK_Unistring, "read failed/raw-kind0 string output");
        if (failRead) {
            eb->fUniString1 = {};
            eb->fUniString1.fString = reinterpret_cast<PA_Unichar*>(1);
            eb->fUniString1.fLength = 100;
            eb->fError = eER_IncompatibleTypes;
        } else {
            text(value->uValue.fString);
            eb->fUniString1 = value->uValue.fString; // Borrowed from owning variable.
        }
        break;
    }
    case EX_CLEAR_VARIABLE:
        ++clearCalls;
        clear(static_cast<PA_Variable*>(eb->fPtr1));
        break;
    case EX_GET_OBJ_VALUE: {
        auto value = static_cast<PA_Variable*>(eb->fPtr2);
        if (failGet) { poison(value); eb->fError = eER_IncompatibleTypes; break; }
        check(nodes.count(eb->fPtr1), "get dead object");
        auto key = text(eb->fUniString1);
        *value = copy(nodes.at(eb->fPtr1)->values.at(key));
        break;
    }
    case EX_SET_OBJ_VALUE: {
        check(nodes.count(eb->fPtr1), "set dead object");
        if (failSet) { eb->fError = eER_IncompatibleTypes; break; }
        auto key = text(eb->fUniString1);
        auto& values = nodes.at(eb->fPtr1)->values;
        if (values.count(key)) clear(&values.at(key));
        values[key] = copy(*static_cast<PA_Variable*>(eb->fPtr2));
        break;
    }
    case EX_EXECUTE_COMMAND_BY_ID: {
        auto args = static_cast<PA_Variable**>(eb->fPtr1);
        auto result = static_cast<PA_Variable*>(eb->fPtr2);
        check(result->fType == eVK_Undefined, "command return initialization");
        check(eb->fParam2 >= 1 && args[0]->fType == eVK_Object, "borrowed object argument");
        check(nodes.count(args[0]->uValue.fObject), "borrowed argument survival");
        if (eb->fParam1 == 1231) {
            if (failHas) { poison(result); eb->fError = eER_IncompatibleTypes; }
            else PA_SetBooleanVariable(result,
                nodes.at(args[0]->uValue.fObject)->values.count(text(args[1]->uValue.fString)));
        } else if (eb->fParam1 == 1217) {
            if (failCommand) { poison(result); eb->fError = eER_IncompatibleTypes; }
            else *result = copy(commandValue);
        } else check(false, "unexpected command");
        break;
    }
    case EX_HANDLE_MANAGER: {
        auto h = eb->fHandle;
        switch (eb->fParam1) {
        case 1:
            if (failHandle) { eb->fHandle = nullptr; eb->fError = eER_IncompatibleTypes; break; }
            h = new char*;
            *h = static_cast<char*>(std::calloc(1, std::max<sLONG_PTR>(1, eb->fParam2)));
            check(*h, "host allocation failure");
            handles[h] = eb->fParam2;
            eb->fHandle = h;
            break;
        case 2:
            check(handles.count(h), "dispose dead handle");
            std::free(*h);
            delete h;
            handles.erase(h);
            break;
        case 3: {
            check(handles.count(h), "resize dead handle");
            auto data = static_cast<char*>(std::realloc(*h, std::max<sLONG_PTR>(1, eb->fParam2)));
            check(data, "host resize failure");
            *h = data;
            handles[h] = eb->fParam2;
            break;
        }
        case 4: check(handles.count(h), "size dead handle"); eb->fParam2 = handles[h]; break;
        case 5:
            check(handles.count(h), "lock dead handle");
            eb->fParam3 = reinterpret_cast<sLONG_PTR>(*h);
            break;
        case 6: check(handles.count(h), "unlock dead handle"); break;
        default: check(false, "unexpected handle operation");
        }
        break;
    }
    default: check(false, "unexpected engine selector");
    }
}

// Required SDK link symbol; the real credential entry point is not compiled.
void PluginMain(PA_long32, PA_PluginParameters) { check(false, "dispatcher forbidden"); }

static PA_Variable stringValue(const CUTF16String& wanted)
{
    auto value = PA_CreateVariable(eVK_Undefined);
    auto u = PA_CreateUnistring(const_cast<PA_Unichar*>(wanted.c_str()));
    PA_SetStringVariable(&value, &u);
    return value;
}

static void callGetter(const std::string& kind, PA_ObjectRef obj, CUTF8String* output,
                       void** handoff = nullptr)
{
    if (kind == "a") check(ob_get_a(obj, L"key", output), "present string property");
    else if (kind == "b") check(ob_get_b(obj, L"key"), "boolean content");
    else if (kind == "n") check(ob_get_n(obj, L"key") == 42.5, "numeric content");
    else if (kind == "o") { auto ref = ob_get_o(obj, L"key"); if (handoff) *handoff = ref; }
    else if (kind == "c") { auto ref = ob_get_c(obj, L"key"); if (handoff) *handoff = ref; }
    else check(false, "unknown getter");
}

static void getCase(const std::string& kind, const char* fixture, bool mismatch)
{
    auto obj = makeNode();
    auto wanted = utf16(fixture);
    auto value = stringValue(wanted);
    if (!mismatch && kind != "a") {
        clear(&value);
        if (kind == "b") PA_SetBooleanVariable(&value, true);
        if (kind == "n") PA_SetRealVariable(&value, 42.5);
        if (kind == "o") PA_SetObjectVariable(&value, makeNode());
        if (kind == "c") PA_SetCollectionVariable(&value, makeNode());
    } else if (mismatch && kind == "a") {
        clear(&value);
        PA_SetObjectVariable(&value, makeNode());
    }
    nodes.at(obj)->values[utf16("key")] = value;
    auto before = strings.size();
    auto clearsBefore = clearCalls;
    CUTF8String output(reinterpret_cast<const uint8_t*>("unchanged"));
    void* handoff = nullptr;
    if (mismatch) {
        if (kind == "b") check(!ob_get_b(obj, L"key"), "boolean mismatch");
        else if (kind == "n") check(ob_get_n(obj, L"key") == 0.0, "numeric mismatch");
        else callGetter(kind, obj, &output, &handoff);
        check(!handoff, "mismatched owning handoff");
    } else callGetter(kind, obj, &output, &handoff);
    check(clearCalls - clearsBefore == (mismatch || (kind != "o" && kind != "c") ? 1 : 0),
          "successful scalar/mismatch must clear once; owning handoff must not clear");
    check(strings.size() == before, "getter leaked owned scalar/mismatched result");
    if (kind == "a") {
        auto expected = mismatch ? "unchanged" : fixture;
        check(output == CUTF8String(reinterpret_cast<const uint8_t*>(expected)), "exact UTF8 bytes");
        check(output.c_str()[output.size()] == 0, "UTF8 terminator/capacity");
    }
    if (!mismatch && (kind == "o" || kind == "c")) {
        check(handoff && nodes.at(handoff)->refs == 2, "owning getter handoff");
    }
    releaseNode(obj);
    if (handoff) {
        check(nodes.count(handoff) && nodes.at(handoff)->refs == 1, "handoff survives original");
        releaseNode(handoff);
    }
}

template<class F> static void mustThrow(F action)
{
    bool threw = false;
    try { action(); } catch (const std::runtime_error&) { threw = true; }
    check(threw, "SDK failure did not use explicit exception channel");
}

static void failureCase(const std::string& mode, const char* fixture)
{
    auto obj = makeNode();
    nodes.at(obj)->values[utf16("key")] = stringValue(utf16(fixture));
    CUTF8String output(reinterpret_cast<const uint8_t*>("unchanged"));
    if (mode == "get-failure" || mode == "has-failure" || mode == "read-failure") {
        failGet = mode == "get-failure";
        failHas = mode == "has-failure";
        failRead = mode == "read-failure";
        for (const auto& kind : {"a", "b", "n", "o", "c"}) {
            if (failRead && std::string(kind) != "a") continue;
            mustThrow([&] { callGetter(kind, obj, &output); });
        }
    } else if (mode == "stringify-handle-failure" || mode == "stringify-failure" ||
               mode == "stringify-read-failure") {
        commandValue = stringValue(utf16(fixture));
        failHandle = mode == "stringify-handle-failure";
        failCommand = mode == "stringify-failure";
        failRead = mode == "stringify-read-failure";
        mustThrow([&] { ob_stringify(obj, &output); });
        clear(&commandValue);
    } else if (mode == "create-value-failure" || mode == "create-key-failure") {
        failCreate = creations + (mode == "create-value-failure" ? 1 : 2);
        mustThrow([&] { ob_set_s(obj, "key", fixture); });
        check(text(nodes.at(obj)->values.at(utf16("key")).uValue.fString) == utf16(fixture),
              "failed creation changed caller property");
    } else if (mode == "getter-create-failure") {
        failCreate = creations + 1;
        mustThrow([&] { ob_get_a(obj, L"key", &output); });
    } else if (mode == "set-failure") {
        failSet = true;
        mustThrow([&] { ob_set_s(obj, "key", "replacement"); });
        check(text(nodes.at(obj)->values.at(utf16("key")).uValue.fString) == utf16(fixture),
              "failed set changed caller property");
    } else check(false, "unknown failure case");
    check(output == CUTF8String(reinterpret_cast<const uint8_t*>("unchanged")),
          "failure lost caller UTF8 bytes");
    releaseNode(obj);
}

static void stringifyCase(const char* fixture, bool mismatch)
{
    auto obj = makeNode();
    commandValue = stringValue(utf16(fixture));
    if (mismatch) { clear(&commandValue); PA_SetObjectVariable(&commandValue, makeNode()); }
    CUTF8String output(reinterpret_cast<const uint8_t*>("unchanged"));
    ob_stringify(obj, &output);
    auto expected = mismatch ? "unchanged" : fixture;
    check(output == CUTF8String(reinterpret_cast<const uint8_t*>(expected)), "stringify exact UTF8");
    check(nodes.at(obj)->refs == 1, "stringify released borrowed object");
    clear(&commandValue);
    releaseNode(obj);
}

static void setterCase(const char* fixture)
{
    auto obj = makeNode();
    for (int i = 0; i < 10; ++i) ob_set_s(obj, "key", fixture);
    CUTF8String output;
    check(ob_get_a(obj, L"key", &output), "setter property exists");
    check(output == CUTF8String(reinterpret_cast<const uint8_t*>(fixture)), "setter exact UTF8");
    auto wanted = utf16(fixture);
    ob_set_a(obj, L"wide", &wanted);
    ob_set_a(obj, L"literal", L"\u65e5\U0001f600");
    check(text(nodes.at(obj)->values.at(utf16("wide")).uValue.fString) == wanted, "UTF16 setter units");
    ob_set_b(obj, L"boolean", true);
    ob_set_n(obj, L"real", 42.5);
    ob_set_i(obj, L"integer", 7);
    ob_set_0(obj, "null");
    auto child = makeNode();
    ob_set_o(obj, L"object", child);
    check(nodes.at(child)->refs == 1, "object setter consuming handoff");
    auto collection = makeNode();
    ob_set_c(obj, L"collection", collection);
    check(nodes.at(collection)->refs == 1, "collection setter consuming handoff");
    check(ob_is_defined(obj, L"key") && !ob_is_defined(obj, L"missing"), "property presence");
    check(!ob_get_a(obj, L"missing", &output), "missing property");
    check(!ob_get_b(obj, L"missing") && ob_get_n(obj, L"missing") == 0.0, "missing scalars");
    check(!ob_get_o(obj, L"missing") && !ob_get_c(obj, L"missing"), "missing handoffs");
    releaseNode(obj);
}

static void scalarCase(const char* fixture)
{
    auto wanted = utf16(fixture);
    auto input = PA_CreateUnistring(const_cast<PA_Unichar*>(wanted.c_str()));
    auto output = PA_CreateUnistring(nullptr);
    auto result = PA_CreateUnistring(nullptr);
    {
        C_TEXT wrapper;
        BytePtr params[] = {reinterpret_cast<BytePtr>(&input)};
        wrapper.fromParamAtIndex(params, 1);
        auto changed = utf16("changed");
        PA_SetUnistring(&input, const_cast<PA_Unichar*>(changed.c_str()));
        params[0] = reinterpret_cast<BytePtr>(&output);
        wrapper.toParamAtIndex(params, 1);
        auto resultPtr = &result;
        wrapper.setReturn(reinterpret_cast<sLONG_PTR*>(&resultPtr));
        CUTF8String u8;
        wrapper.copyUTF8String(&u8);
        check(u8 == CUTF8String(reinterpret_cast<const uint8_t*>(fixture)), "scalar exact UTF8");
    }
    check(text(result) == wanted && text(output) == wanted, "independent scalar content copies");
    auto resultPtr = &result;
    PluginBlock block = {};
    block.fResult = &resultPtr;
    PA_ReturnString(&block, output.fString);
    PA_DisposeUnistring(&output);
    check(text(result) == wanted, "content return survives source disposal");
    PA_DisposeUnistring(&input);
    PA_DisposeUnistring(&result);
}

static void arrayCase()
{
    auto arr = PA_CreateVariable(eVK_ArrayUnicode);
    PA_ResizeArray(&arr, 4);
    const char* fixtures[] = {"ASCII", "\xe6\x97\xa5", "\xf0\x9f\x98\x80", "", ""};
    for (int i = 0; i <= 4; ++i) {
        auto wanted = utf16(fixtures[i]);
        auto u = PA_CreateUnistring(const_cast<PA_Unichar*>(wanted.c_str()));
        PA_SetStringInArray(arr, i, &u);
    }
    BytePtr params[] = {reinterpret_cast<BytePtr>(&arr)};
    ARRAY_TEXT wrapper;
    wrapper.fromParamAtIndex(params, 1);
    auto replacement = utf16("replacement");
    auto u = PA_CreateUnistring(const_cast<PA_Unichar*>(replacement.c_str()));
    PA_SetStringInArray(arr, 0, &u);
    wrapper.toParamAtIndex(params, 1);
    for (int i = 0; i <= 4; ++i)
        check(text(PA_GetStringInArray(arr, i)) == utf16(fixtures[i]), "array borrowed copy/transfer");
    check(PA_GetStringInArray(arr, 0).fString != PA_GetStringInArray(arr, 1).fString,
          "array independent output owners");
    auto rejected = PA_CreateUnistring(const_cast<PA_Unichar*>(replacement.c_str()));
    PA_SetStringInArray(arr, -1, &rejected);
    PA_SetStringInArray(arr, 5, &rejected);
    auto wrong = PA_CreateVariable(eVK_Longint);
    PA_SetStringInArray(wrong, 0, &rejected);
    auto missing = arr;
    missing.uValue.fArray.fData = nullptr;
    PA_SetStringInArray(missing, 0, &rejected);
    PA_SetStringInArray(arr, 0, nullptr);
    check(text(rejected) == replacement, "rejected transfer must retain caller owner");
    PA_DisposeUnistring(&rejected);
    wrapper.setSize(2);
    wrapper.toParamAtIndex(params, 1);
    wrapper.setSize(0);
    wrapper.toParamAtIndex(params, 1);
    PA_ClearVariable(&arr);
    auto scalar = stringValue(utf16("independent scalar"));
    PA_ClearVariable(&scalar);
    failHandle = true;
    auto failure = PA_ExecuteCommandByID(1217, nullptr, 0);
    check(failure.fType == 0 && PA_GetLastError() != eER_NoErr, "command raw failure sentinel");
    check(PA_GetVariableKind(failure) == eVK_Unistring, "raw sentinel normalization");
    auto method = PA_ExecuteMethodByID(1, nullptr, 0);
    check(method.fType == eVK_Undefined, "method failure differs from command failure");
}

int main(int argc, char** argv)
{
    check(argc == 4, "case fixture empty-mode required");
    gCall4D = host;
    nullEmpty = std::string(argv[3]) == "null";
    std::string name = argv[1];
    if (name.compare(0, 4, "get-") == 0 && name.size() == 5)
        getCase(name.substr(4), argv[2], false);
    else if (name.compare(0, 9, "mismatch-") == 0)
        getCase(name.substr(9), argv[2], true);
    else if (name.find("failure") != std::string::npos) failureCase(name, argv[2]);
    else if (name == "stringify" || name == "stringify-mismatch")
        stringifyCase(argv[2], name == "stringify-mismatch");
    else if (name == "setters") setterCase(argv[2]);
    else if (name == "scalar") scalarCase(argv[2]);
    else if (name == "arrays") arrayCase();
    else check(false, "unknown test");
    check(strings.empty(), "unbalanced string allocations");
    check(handles.empty(), "unbalanced handles");
    check(nodes.empty(), "unbalanced object/collection references");
    std::cout << "PASS " << name << " strings=0 handles=0 refs=0\n";
}
