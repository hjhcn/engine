//
//  GBridge.h
//  TestOCTests
//
//  Created by 对象 on 2019/1/2.
//  Copyright © 2019 对象. All rights reserved.
//

#ifndef GBridge_h
#define GBridge_h

#include <stdio.h>
#include <JavaScriptCore/JavaScriptCore.h>
#include <string>
#include "GBinding.h"
#include "GBindingGlobal.h"

namespace Kraken {
    
    class GBridge {
    public:
        static GBridge* sharedInstance();
        GBridge();
        ~GBridge();
        void Init();
        void Uninit();
        JSValueRef evaluate(const std::string& script);
        
        JSObjectRef createJSObject(GBinding *binding, size_t argc, const JSValueRef *argv);
        JSValueRef invokeFunction(JSObjectRef funcObject, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[]);
        
        JSValueRef invokeKrakenCallback(size_t argumentCount, const JSValueRef arguments[]);
    protected:
        void addGlobalObject(std::string name, GBinding *binding);
    private:
        GBridge(const GBridge &) = delete;
        GBridge &operator=(const GBridge &) = delete;
    private:
        JSGlobalContextRef m_jsGlobalContext;
        JSObjectRef m_jsGlobalObject;
        GBindingGlobal *m_global;
    };
}

#endif /* GBridge_h */
