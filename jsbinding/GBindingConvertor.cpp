//
//  GBindingConvertor.cpp
//  webgl_testapp
//
//  Created by 对象 on 2019/5/23.
//  Copyright © 2019 Alibaba. All rights reserved.
//

#include "GBindingConvertor.h"
#include <typeinfo>
#include <functional>
#include <sys/time.h>
#include <iostream>
#include <vector>

namespace Kraken {
    
    
    bool JSValueIsArrayPrivate(JSContextRef ctx, JSValueRef value)
    {
        if (JSValueIsObject(ctx, value))
        {
            JSStringRef name = JSStringCreateWithUTF8CString("Array");
            JSObjectRef array = (JSObjectRef)JSObjectGetProperty(ctx, JSContextGetGlobalObject(ctx), name, NULL);
            JSStringRelease(name);
            name = JSStringCreateWithUTF8CString("isArray");
            JSObjectRef isArray = (JSObjectRef)JSObjectGetProperty(ctx, array, name, NULL);
            JSStringRelease(name);
            JSValueRef retval = JSObjectCallAsFunction(ctx, isArray, NULL, 1, &value, NULL);
            if (JSValueIsBoolean(ctx, retval))
            return JSValueToBoolean(ctx, retval);
        }
        return false;
    }
    
    std::string JSValueToString(JSContextRef ctx, JSValueRef value ){
        JSStringRef jsString = JSValueToStringCopy( ctx, value, NULL );
        if( !jsString ) return NULL;
        
        size_t maxBufferSize = JSStringGetMaximumUTF8CStringSize(jsString);
        char* utf8Buffer = new char[maxBufferSize];
        size_t bytesWritten = JSStringGetUTF8CString(jsString, utf8Buffer, maxBufferSize);
        std::string utf_string = std::string(utf8Buffer, bytesWritten -1);
        delete [] utf8Buffer;
        return utf_string;
    }
    
    JSValueRef StringToJSValue(JSContextRef ctx, std::string str) {
        JSStringRef strRef = JSStringCreateWithUTF8CString(str.c_str());
        JSValueRef value = JSValueMakeString(ctx, strRef);
        JSStringRelease(strRef);
        return value;
    }
    
    double JSValueToNumberFast(JSContextRef ctx, JSValueRef v) {
#if __LP64__ // arm64 version
        union {
            int64_t asInt64;
            double asDouble;
            struct { int32_t asInt; int32_t tag; } asBits;
        } taggedValue = { .asInt64 = (int64_t)v };
        
#define DoubleEncodeOffset 0x1000000000000ll
#define TagTypeNumber 0xffff0000
#define ValueTrue 0x7
        
        if( (taggedValue.asBits.tag & TagTypeNumber) == TagTypeNumber ) {
            return taggedValue.asBits.asInt;
        }
        else if( taggedValue.asBits.tag & TagTypeNumber ) {
            taggedValue.asInt64 -= DoubleEncodeOffset;
            return taggedValue.asDouble;
        }
        else if( taggedValue.asBits.asInt == ValueTrue ) {
            return 1.0;
        }
        else {
            return 0; // false, undefined, null, object
        }
#else // armv7 version
        return JSValueToNumber(ctx, v, NULL);
#endif
    }
    
    std::vector<std::string> JSValueToStringVector(JSContextRef ctx, JSValueRef value) {
        std::vector<std::string> array;
        if( JSValueIsArrayPrivate(ctx, value) ){
            JSStringRef lengthName = JSStringCreateWithUTF8CString("length");
            int count = JSValueToNumberFast(ctx, JSObjectGetProperty(ctx, (JSObjectRef)value, lengthName, NULL));
            JSStringRelease(lengthName);
            for( int i = 0; i < count; i++ ) {
                std::string item = JSValueToString(ctx, value);
                array.push_back(item);
            }
        }
        return array;
    }
    
    void *JSValueGetPrivate(JSValueRef val) {
        // On 64bit systems we can not safely call JSObjectGetPrivate with any
        // JSValueRef. Doing so with immediate values (numbers, null, bool,
        // undefined) will crash the app. So we check for these first.
#if __LP64__
        return !((int64_t)val & 0xffff000000000002ll) ? JSObjectGetPrivate((JSObjectRef)val) : NULL;
#else
        return JSObjectGetPrivate((JSObjectRef)val);
#endif
    }
    
}


