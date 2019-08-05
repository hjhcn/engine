//
//  GBindingGlobal.cpp
//  webgl
//
//  Created by 对象 on 2019/5/30.
//  Copyright © 2019 Alibaba. All rights reserved.
//

#include "GBindingGlobal.h"
#include "flutter/runtime/dart_to_cpp.h"
#include "GBindingConvertor.h"

#define GBINDING_GLOBAL_CLASSNAME "kraken"

using namespace Kraken;

const char* GBindingGlobal::classname() {
    return GBINDING_GLOBAL_CLASSNAME;
}

JSValueRef func_runApp(JSContextRef ctx, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef *exception ) {
  if (argc > 0) {
    std::string widgetData = JSValueToString(ctx, argv[0]);
    DartToCpp::invokeDartFromCpp("runKraken", widgetData);
  }
  return JSValueMakeUndefined(ctx);
}

JSValueRef func_rebuild(JSContextRef ctx, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef *exception ) {
  DartToCpp::invokeDartFromCpp("rebuildKraken", "");
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
    JSClassDefinition classDefinition = CreateBindingClassDefinication(GBINDING_GLOBAL_CLASSNAME);
    JSStaticFunction functions[] = {
        G_STATIC_FUNCTION(runApp)
        G_STATIC_FUNCTION(rebuild)
        G_STATIC_FUNCTION(setCallback)
        G_STATIC_FUNCTION_NULL
    };
    classDefinition.staticFunctions = functions;
    return JSClassCreate(&classDefinition);
}
