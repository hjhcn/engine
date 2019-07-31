//
//  dart_to_cpp.cc
//  sources
//
//  Created by 对象 on 2019/7/31.
//

#include "dart_to_cpp.h"
#include "flutter/fml/logging.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_class_library.h"
#include "third_party/tonic/dart_class_provider.h"
#include "third_party/tonic/dart_message_handler.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/file_loader/file_loader.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/scopes/dart_api_scope.h"
#include "third_party/tonic/scopes/dart_isolate_scope.h"
#include "third_party/tonic/dart_library_natives.h"

using tonic::ToDart;

namespace Kraken {
  
  static Dart_Handle getKrakenLirary() {
    static Dart_Handle s = Dart_LookupLibrary(tonic::ToDart("package:kraken/main.dart"));
    return s;
  }
  
  namespace {
    static fml::closure root_isolate_native_callback_;
    void SystemRand(Dart_NativeArguments args) {
      Dart_Handle result = Dart_NewInteger(5234);
      Dart_SetReturnValue(args, result);
      if (root_isolate_native_callback_) {
        printf("-----native callback----\n");
        root_isolate_native_callback_();
      }
    }
    
    static tonic::DartLibraryNatives* g_natives;
    
    Dart_NativeFunction GetNativeFunction(Dart_Handle name,
    int argument_count,
    bool* auto_setup_scope) {
      return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
    }
    
    const uint8_t* GetSymbol(Dart_NativeFunction native_function) {
      return g_natives->GetSymbol(native_function);
    }
    
  }
  
  void DartToCpp::InitBinding(const flutter::Settings& settings) {
    if (!g_natives) {
      g_natives = new tonic::DartLibraryNatives();
      root_isolate_native_callback_ = settings.root_isolate_native_callback;
      g_natives->Register({
        {"SystemRand", SystemRand, 0, true},
      });
      Dart_Handle result = Dart_SetNativeResolver(getKrakenLirary(), GetNativeFunction, GetSymbol);
      if (Dart_IsError(result)) {
        Dart_PropagateError(result);
      }
    }
  }
  
  void DartToCpp::invokeDartFromCpp(const char* name, std::string arg) {
    tonic::LogIfError(tonic::DartInvokeField(getKrakenLirary(), name, {}));
  }

  
}
