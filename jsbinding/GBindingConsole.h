//
//  GBindingConsole.hpp
//  sources
//
//  Created by 对象 on 2019/8/2.
//

#ifndef GBindingConsole_hpp
#define GBindingConsole_hpp

#include "GBinding.h"

namespace Kraken {
  class GBindingConsole : public GBinding {
  public:
    virtual const char* classname();
    virtual JSClassRef createJSClass();
    
  };
}


#endif /* GBindingConsole_hpp */
