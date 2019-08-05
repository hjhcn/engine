//
//  GBindingConsole.cpp
//  sources
//
//  Created by 对象 on 2019/8/2.
//

#include "GBindingConsole.h"
#include "GBindingConvertor.h"

#define GBINDING_CONSOLE_CLASSNAME "console"

using namespace Kraken;

const char* GBindingConsole::classname() {
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

JSClassRef GBindingConsole::createJSClass() {
  JSClassDefinition classDefinition = CreateBindingClassDefinication(GBINDING_CONSOLE_CLASSNAME);
  JSStaticFunction functions[] = {
    G_STATIC_FUNCTION(log)
    G_STATIC_FUNCTION_NULL
  };
  classDefinition.staticFunctions = functions;
  return JSClassCreate(&classDefinition);
}
