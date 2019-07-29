//
//  GBindingConvertor.h
//  webgl_testapp
//
//  Created by 对象 on 2019/5/23.
//  Copyright © 2019 Alibaba. All rights reserved.
//

#ifndef GBindingConvertor_h
#define GBindingConvertor_h

#include <JavaScriptCore/JavaScriptCore.h>
#include <typeinfo>
#include <string>
#include <vector>

namespace Kraken {
    
    std::string JSValueToString(JSContextRef ctx, JSValueRef value );
    
    JSValueRef StringToJSValue(JSContextRef ctx, std::string str);
    
    double JSValueToNumberFast(JSContextRef ctx, JSValueRef v);
    
    bool JSValueIsArrayPrivate(JSContextRef ctx, JSValueRef value);
    
    template<typename T>
    std::vector<T> JSValueToNumberVector(JSContextRef ctx, JSValueRef value) {
        std::vector<T> array;
        if( JSValueIsArrayPrivate(ctx, value) ){
            JSStringRef lengthName = JSStringCreateWithUTF8CString("length");
            int count = JSValueToNumberFast(ctx, JSObjectGetProperty(ctx, (JSObjectRef)value, lengthName, NULL));
            JSStringRelease(lengthName);
            for( int i = 0; i < count; i++ ) {
                T item = (T)JSValueToNumberFast(ctx, JSObjectGetPropertyAtIndex(ctx, (JSObjectRef)value, i, NULL));
                array.push_back(item);
            }
        }
        return array;
    }
    
    template<typename T>
    JSValueRef NumberVectorToJSValue(JSContextRef ctx, std::vector<T> v) {
        JSValueRef values[v.size()];
        for (int i=0; i<v.size(); i++) {
            values[i] = JSValueMakeNumber(ctx, v[i]);
        }
        return JSObjectMakeArray(ctx, v.size(), values, NULL);
    }
    
    std::vector<std::string> JSValueToStringVector(JSContextRef ctx, JSValueRef value);
    
    void *JSValueGetPrivate(JSValueRef val);
    
//    template<typename Class>
//    Class * JSValueToBindingObject(JSContextRef ctx, JSValueRef value) {
//        JSObjectRef ref = JSValueToObject(ctx, value, NULL);
//        Class *object = static_cast<Class*>(JSValueGetPrivate(ref));
//        if (typeid(*object).name() == typeid(Class).name()) {
//            return object;
//        }
//        else {
//            return NULL;
//        }
//    }
    
}

#endif /* GBindingConvertor_h */
