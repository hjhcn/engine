//
//  GBindingGlobal.h
//  webgl
//
//  Created by 对象 on 2019/5/30.
//  Copyright © 2019 Alibaba. All rights reserved.
//

#ifndef GBindingGlobal_h
#define GBindingGlobal_h


#include "GBinding.h"

namespace Kraken {
    class GBindingGlobal : public GBinding {
    public:
        virtual const char* classname();
        virtual JSClassRef createJSClass();
        
        JSObjectRef callback(){ return m_callback;};
        void setCallback(JSObjectRef ref){m_callback = ref;};
        JSObjectRef jsApp(){ return m_jsApp;};
        void setJSApp(JSObjectRef ref){m_jsApp = ref;};
        JSValueRef invokeKrakenCallback(JSContextRef ctx, const std::string& arg);
    private:
        JSObjectRef m_callback;
        JSObjectRef m_jsApp;
    };
}


#endif /* GBindingConsole_h */
