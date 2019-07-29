//
//  GBindingGlobal.cpp
//  webgl
//
//  Created by 对象 on 2019/5/30.
//  Copyright © 2019 Alibaba. All rights reserved.
//

#include "GBindingGlobal.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"
#include "GBindingConvertor.h"

#define GBINDING_CONSOLE_CLASSNAME "kraken"

using namespace Kraken;

const char* GBindingGlobal::classname() {
    return GBINDING_CONSOLE_CLASSNAME;
}

JSValueRef func_log(JSContextRef ctx, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef *exception ) {
    
    std::string logStr;
    if( argc >= 1  ){
        for (size_t i=0; i<argc; i++) {
            std::string tmpStr;
            JSValueRef value = argv[i];
            if( JSValueIsString(ctx, value) ){
                tmpStr = JSValueToString(ctx, value);
            }else if( JSValueIsBoolean(ctx, value) ){
                tmpStr = JSValueToBoolean(ctx, value) ? "true" : "false";
            }else if( JSValueIsNumber(ctx, value) ){
                tmpStr = std::to_string(JSValueToNumberFast(ctx, value));
            }else if( JSValueIsNull(ctx, value) || JSValueIsUndefined(ctx, value) ){
                tmpStr = "undefined";
            }else if( JSValueIsObject(ctx, value) ){
                tmpStr = "[Object]";
            }
            
            if( !tmpStr.empty() ){
                if( i < argc-1 ){
                    tmpStr = tmpStr.append(" ");
                }
                logStr = logStr.append(tmpStr);
            }
        }
        
        printf("[Console.log] %s\n", logStr.c_str());
    }
    
    
    return JSValueMakeUndefined(ctx);
}

JSValueRef func_runApp(JSContextRef ctx, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef *exception ) {
  
  Dart_Handle library = Dart_LookupLibrary(tonic::ToDart("package:flutter_app/main.dart"));
  tonic::LogIfError(tonic::DartInvokeField(library, "__runApp", {}));
  
  return JSValueMakeUndefined(ctx);
}

JSValueRef func_setCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef *exception ) {
    G_BINDING_FUNCTION_PREPARE(GBindingGlobal, 1)
    if (argc == 1) {
        thisObject->setCallback(JSValueToObject(ctx, argv[0], NULL));
    }
    return JSValueMakeUndefined(ctx);
}

JSClassRef GBindingGlobal::createJSClass() {
    JSClassDefinition classDefinition = CreateBindingClassDefinication(GBINDING_CONSOLE_CLASSNAME);
    JSStaticFunction functions[] = {
        G_STATIC_FUNCTION(log)
        G_STATIC_FUNCTION(runApp)
        G_STATIC_FUNCTION(setCallback)
        G_STATIC_FUNCTION_NULL
    };
    classDefinition.staticFunctions = functions;
    return JSClassCreate(&classDefinition);
}
