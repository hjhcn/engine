//
//  GBridge.cc
//  TestOCTests
//
//  Created by 对象 on 2019/1/2.
//  Copyright © 2019 对象. All rights reserved.
//

#include "GBridge.h"

using namespace Kraken;


GBridge* GBridge::sharedInstance() {
    static GBridge *s{new GBridge};
    return s;
}

GBridge::GBridge() {
    
}

GBridge::~GBridge() {
    GBinding::remoteCacheJSClass();
    JSGlobalContextRelease(m_jsGlobalContext);
}

void GBridge::Init() {
    m_jsGlobalContext = JSGlobalContextCreateInGroup(NULL, NULL);
    m_jsGlobalObject = JSContextGetGlobalObject(m_jsGlobalContext);
    m_global = new GBindingGlobal();
    this->addGlobalObject("kraken", m_global);
}

void GBridge::Uninit() {
    GBinding::remoteCacheJSClass();
    JSGlobalContextRelease(m_jsGlobalContext);
    delete m_global;
}

JSValueRef GBridge::evaluate(const std::string& script) {
    
    if(script.length() == 0 ) {
        printf("script is NULL\n");
        return NULL;
    }
    
    JSStringRef scriptJS = JSStringCreateWithUTF8CString(script.c_str());
    
    JSValueRef ret = JSEvaluateScript(m_jsGlobalContext, scriptJS, NULL, NULL, 0, NULL);
    
    JSStringRelease(scriptJS);
    
    return ret;
}

void GBridge::addGlobalObject(std::string name, GBinding *binding) {
    binding->m_bridge = this;
    JSObjectRef jsObject = JSObjectMake(m_jsGlobalContext, binding->getJSClass(), binding);
    JSStringRef jsName = JSStringCreateWithUTF8CString(name.c_str());
    JSObjectSetProperty(m_jsGlobalContext, m_jsGlobalObject, jsName, jsObject, (kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly), NULL);
    JSStringRelease(jsName);
}

JSObjectRef GBridge::createJSObject(GBinding *binding, size_t argc, const JSValueRef *argv) {
    JSClassRef jsClass = binding->getJSClass();
    JSObjectRef jsObject = JSObjectMake(m_jsGlobalContext, jsClass, binding);
    return jsObject;
}

JSValueRef GBridge::invokeFunction(JSObjectRef funcObject, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[]) {
    return JSObjectCallAsFunction(m_jsGlobalContext, funcObject, thisObject, argumentCount, arguments, NULL);
}

JSValueRef GBridge::invokeKrakenCallback(size_t argumentCount, const JSValueRef *arguments) {
    return JSObjectCallAsFunction(m_jsGlobalContext, m_global->callback(), NULL, argumentCount, arguments, NULL);
}
