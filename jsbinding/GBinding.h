//
//  GBinding.h
//  TestOCTests
//
//  Created by 对象 on 2019/1/2.
//  Copyright © 2019 对象. All rights reserved.
//

#ifndef GBinding_h
#define GBinding_h
#include <JavaScriptCore/JavaScriptCore.h>

#include <map>
#include <string>
#include <vector>

#define LOG(name,...) printf("%s : \n",#name);printf(__VA_ARGS__);printf("\n\n");


#define G_STATIC_FUNCTION(NAME) \
{ #NAME, func_##NAME, kJSPropertyAttributeDontDelete|kJSPropertyAttributeReadOnly },

#define G_STATIC_FUNCTION_NAME(NAME, FUNCNAME) \
{ #NAME, FUNCNAME, kJSPropertyAttributeDontDelete|kJSPropertyAttributeReadOnly },

#define G_STATIC_FUNCTION_NULL    {0, 0, 0}

#define G_BINDING_FUNCTION_DEFINE(NAME) \
JSValueRef NAME(JSContextRef ctx, size_t argc, const JSValueRef * argv);

#define G_BINDING_FUNCTION(CLASSNAME, NAME, CTX, ARGC, ARGV) \
JSValueRef func_##NAME(JSContextRef CTX, JSObjectRef function, JSObjectRef thisObject, size_t ARGC, const JSValueRef ARGV[], JSValueRef *exception ) { \
CLASSNAME *object = (CLASSNAME *)JSValueGetPrivate(thisObject); \
return object->NAME(CTX, ARGC, ARGV); \
} \
JSValueRef CLASSNAME::NAME(JSContextRef CTX, size_t ARGC, const JSValueRef * ARGV)

#define G_BINDING_FUNCTION_GET_THIS_OBJECT(CLASSNAME) \
CLASSNAME *thisObject = (CLASSNAME *)JSValueGetPrivate(object);

#define G_BINDING_FUNCTION_ARG_COUNT_ASSERT(MAX_ARG_NUM) \
if (argc < MAX_ARG_NUM) { \
return JSValueMakeUndefined(ctx); \
}

#define G_BINDING_FUNCTION_PREPARE(CLASSNAME, MAX_ARG_NUM) \
G_BINDING_FUNCTION_ARG_COUNT_ASSERT(MAX_ARG_NUM) \
G_BINDING_FUNCTION_GET_THIS_OBJECT(CLASSNAME)

#define G_BINDING_FUNCTION_TRANS_ARG(CLASSNAME, NEW_ARG_NAME, VALUE, REQUIRED) \
CLASSNAME * NEW_ARG_NAME = JSValueToBindingObject<CLASSNAME>(ctx, VALUE); \
if(REQUIRED && NEW_ARG_NAME == NULL) { \
    /* throw error  */\
    return JSValueMakeUndefined(ctx); \
}



//StaticValue Sturct
#define G_STATIC_VALUE_READ_ONLY(NAME) \
{ #NAME, prop_get_##NAME, NULL, kJSPropertyAttributeDontDelete|kJSPropertyAttributeReadOnly },

#define G_STATIC_VALUE_READ_ONLY_NUMBER(NAME, VALUE) \
{ #NAME, [](JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception) -> JSValueRef { return JSValueMakeNumber(ctx, VALUE); }, NULL, kJSPropertyAttributeDontDelete|kJSPropertyAttributeReadOnly },

#define G_STATIC_VALUE_READ_ONLY_COPY(NAME, COPY) \
{ #NAME, prop_get_##COPY, NULL, kJSPropertyAttributeDontDelete|kJSPropertyAttributeReadOnly },

#define G_STATIC_VALUE_READ_WRITE(NAME) \
{ #NAME, prop_get_##NAME, prop_set_##NAME, kJSPropertyAttributeDontDelete },

#define G_STATIC_VALUE_READ_WRITE_VALUE(NAME, VALUE) \
{ #NAME, prop_get_##VALUE, prop_set_##VALUE, kJSPropertyAttributeDontDelete },

#define G_STATIC_VALUE_NULL    { 0, 0, 0, 0 }

namespace Kraken {
    class GBridge;
    
    class GBinding {
        static std::map<std::string, JSClassRef> jsClassMap;
    public:
        static void remoteCacheJSClass();
        virtual void destroy();
        
        GBinding();
        GBinding(JSContextRef ctx);
        virtual ~GBinding();
        
        virtual const char* classname();
        virtual JSClassRef createJSClass() = 0;
        
        JSClassRef getJSClass();
    public:
        GBridge *m_bridge{NULL};
    };
    
    
    JSClassDefinition CreateBindingClassDefinication(const char * classname);
    
    JSObjectRef CreateJSObjectWithContext(JSContextRef ctx, GBinding *binding);
}




#endif /* GBinding_h */
