//
//  GBinding.cc
//  TestOCTests
//
//  Created by 对象 on 2019/1/2.
//  Copyright © 2019 对象. All rights reserved.
//

#include "GBinding.h"
#include "GBindingConvertor.h"

using namespace Kraken;

void BindingFinalize(JSObjectRef object){
    GBinding* bindingInst = (GBinding*)JSValueGetPrivate(object);
//    const char* name = typeid(*bindingInst).name();
//    LOG(GBinding-Finalize, "%s", name);
    bindingInst->destroy();
    delete bindingInst;
}

std::map<std::string, JSClassRef> GBinding::jsClassMap;

void GBinding::remoteCacheJSClass() {
    std::map<std::string, JSClassRef>::iterator iter = jsClassMap.begin();
    while(iter != jsClassMap.end()) {
        JSClassRelease(iter->second);
        iter++;
    }
    jsClassMap.clear();
}

GBinding::GBinding() {
    
}

GBinding::GBinding(JSContextRef ctx) {
    
}

GBinding::~GBinding() {
    LOG(GBinding, "dealloc");
}

void GBinding::destroy() {
}

JSClassRef GBinding::getJSClass() {
    std::string classname = this->classname();
    JSClassRef jsClass = GBinding::jsClassMap[classname];
    if (!jsClass) {
        jsClass = this->createJSClass();
        GBinding::jsClassMap[classname] = jsClass;
    }
    return jsClass;
}

const char* GBinding::classname() {
    return "GBinding";
}

JSClassDefinition Kraken::CreateBindingClassDefinication(const char * classname) {
    JSClassDefinition classDefinition = kJSClassDefinitionEmpty;
    classDefinition.className = classname;
    classDefinition.finalize = BindingFinalize;
    return classDefinition;
}

JSObjectRef Kraken::CreateJSObjectWithContext(JSContextRef ctx, GBinding *binding) {
    return JSObjectMake(ctx, binding->getJSClass(), binding);
}


