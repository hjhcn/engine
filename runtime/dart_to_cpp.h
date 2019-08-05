//
//  dart_to_cpp.hpp
//  sources
//
//  Created by 对象 on 2019/7/31.
//

#ifndef dart_to_cpp_hpp
#define dart_to_cpp_hpp

#include "flutter/fml/macros.h"
#include "flutter/common/settings.h"

namespace Kraken {
  
  class DartToCpp {
  public:
    
    /**
     * 初始化Dart绑定环境
     */
    static void InitBinding(const flutter::Settings& settings);
    
    /**
     * 初始化Dart绑定环境
     */
    static void DidCreateIsolate();
    
    /**
     * Dart绑定环境销毁
     */
    static void DidDesIsolate();
    
    /**
     * cpp -> dart
     * name: dart方法
     * arg: 参数，所有参数在js端封装成json字符串
     */
    static void invokeDartFromCpp(const char* name, const std::string& arg);
    
  private:
    FML_DISALLOW_IMPLICIT_CONSTRUCTORS(DartToCpp);
  };
  
}  // namespace flutter

#endif /* dart_to_cpp_hpp */
