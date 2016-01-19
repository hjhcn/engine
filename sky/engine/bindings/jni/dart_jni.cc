// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/bindings/jni/dart_jni.h"

#include "base/logging.h"
#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "sky/engine/bindings/jni/jni_array.h"
#include "sky/engine/bindings/jni/jni_class.h"
#include "sky/engine/bindings/jni/jni_object.h"
#include "sky/engine/tonic/dart_args.h"
#include "sky/engine/tonic/dart_binding_macros.h"
#include "sky/engine/tonic/dart_converter.h"

namespace blink {

using base::android::ScopedJavaLocalRef;
using base::android::ScopedJavaGlobalRef;

namespace {

DartLibraryNatives* g_natives = nullptr;

Dart_NativeFunction GetNativeFunction(Dart_Handle name,
                                      int argument_count,
                                      bool* auto_setup_scope) {
  return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
}

const uint8_t* GetSymbol(Dart_NativeFunction native_function) {
  return g_natives->GetSymbol(native_function);
}

// Data cached for each Dart isolate.
struct DartJniIsolateData : public base::SupportsUserData::Data {
  Dart_PersistentHandle jni_object_type;
  Dart_PersistentHandle jni_float_type;
};

// Data cached from the Java VM.
struct DartJniJvmData {
  ScopedJavaGlobalRef<jobject> class_loader;
  ScopedJavaGlobalRef<jclass> class_clazz;
  jmethodID class_loader_load_class_method_id;
  jmethodID class_get_name_method_id;
};

DartJniJvmData* g_jvm_data = nullptr;

void* UserDataKey() {
  // Return a unique key for our per-isolate data.
  static int data_key = 0;
  return reinterpret_cast<void*>(&data_key);
}

void CreateIsolateData() {
  DartState::Current()->SetUserData(UserDataKey(), new DartJniIsolateData());
}

DartJniIsolateData* IsolateData() {
  base::SupportsUserData::Data* user_data =
      DartState::Current()->GetUserData(UserDataKey());
  DCHECK(user_data);
  return static_cast<DartJniIsolateData*>(user_data);
}

} // anonymous namespace

// Check if a JNI API has thrown an exception.  If so, convert it to a
// Dart exception.
bool CheckJniException(JNIEnv* env, Dart_Handle *exception) {
  if (env->ExceptionCheck() == JNI_FALSE)
    return false;

  jthrowable java_throwable = env->ExceptionOccurred();
  env->ExceptionClear();
  std::string info = base::android::GetJavaExceptionInfo(
      env, java_throwable);

  *exception = StdStringToDart(info);
  return true;
}

// Check if a Dart API returned an error handle.
bool CheckDartException(Dart_Handle result, Dart_Handle* exception) {
  if (!Dart_IsError(result))
    return false;

  *exception = result;
  return true;
}

DART_NATIVE_CALLBACK_STATIC(JniClass, FromName);

#define FOR_EACH_BINDING(V) \
  V(JniArray, GetLength) \
  V(JniClass, CallStaticBooleanMethod) \
  V(JniClass, CallStaticByteMethod) \
  V(JniClass, CallStaticCharMethod) \
  V(JniClass, CallStaticDoubleMethod) \
  V(JniClass, CallStaticFloatMethod) \
  V(JniClass, CallStaticIntMethod) \
  V(JniClass, CallStaticLongMethod) \
  V(JniClass, CallStaticObjectMethod) \
  V(JniClass, CallStaticShortMethod) \
  V(JniClass, CallStaticVoidMethod) \
  V(JniClass, NewObject) \
  V(JniClass, GetFieldId) \
  V(JniClass, GetMethodId) \
  V(JniClass, GetStaticBooleanField) \
  V(JniClass, GetStaticByteField) \
  V(JniClass, GetStaticCharField) \
  V(JniClass, GetStaticDoubleField) \
  V(JniClass, GetStaticFieldId) \
  V(JniClass, GetStaticFloatField) \
  V(JniClass, GetStaticIntField) \
  V(JniClass, GetStaticLongField) \
  V(JniClass, GetStaticMethodId) \
  V(JniClass, GetStaticObjectField) \
  V(JniClass, GetStaticShortField) \
  V(JniClass, SetStaticBooleanField) \
  V(JniClass, SetStaticByteField) \
  V(JniClass, SetStaticCharField) \
  V(JniClass, SetStaticDoubleField) \
  V(JniClass, SetStaticFloatField) \
  V(JniClass, SetStaticIntField) \
  V(JniClass, SetStaticLongField) \
  V(JniClass, SetStaticObjectField) \
  V(JniClass, SetStaticShortField) \
  V(JniObject, CallBooleanMethod) \
  V(JniObject, CallIntMethod) \
  V(JniObject, CallObjectMethod) \
  V(JniObject, GetIntField) \
  V(JniObjectArray, GetArrayElement) \
  V(JniObjectArray, SetArrayElement) \
  V(JniString, GetText)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void DartJni::InitForGlobal() {
  if (!g_natives) {
    g_natives = new DartLibraryNatives();

    g_natives->Register({
      DART_REGISTER_NATIVE_STATIC(JniClass, FromName)
      FOR_EACH_BINDING(DART_REGISTER_NATIVE)
    });
  }
}

void DartJni::InitForIsolate() {
  DCHECK(g_natives);

  Dart_Handle jni_library = Dart_LookupLibrary(ToDart("dart:jni"));
  DART_CHECK_VALID(jni_library)

  DART_CHECK_VALID(Dart_SetNativeResolver(
      jni_library, GetNativeFunction, GetSymbol));

  CreateIsolateData();

  Dart_Handle object_type = Dart_GetType(
      jni_library, ToDart("JniObject"), 0, nullptr);
  DART_CHECK_VALID(object_type);
  IsolateData()->jni_object_type = Dart_NewPersistentHandle(object_type);
  DART_CHECK_VALID(IsolateData()->jni_object_type);

  Dart_Handle float_type = Dart_GetType(
      jni_library, ToDart("JniFloat"), 0, nullptr);
  DART_CHECK_VALID(float_type);
  IsolateData()->jni_float_type = Dart_NewPersistentHandle(float_type);
  DART_CHECK_VALID(IsolateData()->jni_float_type);
}

bool DartJni::InitJni() {
  JNIEnv* env = base::android::AttachCurrentThread();

  DCHECK(!g_jvm_data);
  g_jvm_data = new DartJniJvmData();

  g_jvm_data->class_loader.Reset(base::android::GetClassLoader(env));

  ScopedJavaLocalRef<jclass> class_loader_clazz(
      env, env->FindClass("java/lang/ClassLoader"));
  CHECK(!base::android::ClearException(env));

  g_jvm_data->class_loader_load_class_method_id = env->GetMethodID(
      class_loader_clazz.obj(),
      "loadClass",
      "(Ljava/lang/String;)Ljava/lang/Class;");
  CHECK(!base::android::ClearException(env));

  g_jvm_data->class_clazz.Reset(env, env->FindClass("java/lang/Class"));
  CHECK(!base::android::ClearException(env));

  g_jvm_data->class_get_name_method_id = env->GetMethodID(
      g_jvm_data->class_clazz.obj(),
      "getName",
      "()Ljava/lang/String;");
  CHECK(!base::android::ClearException(env));

  return true;
}

ScopedJavaLocalRef<jclass> DartJni::GetClass(JNIEnv* env, const char* name) {
  jobject clazz = env->CallObjectMethod(
      g_jvm_data->class_loader.obj(),
      g_jvm_data->class_loader_load_class_method_id,
      base::android::ConvertUTF8ToJavaString(env, name).obj());

  return ScopedJavaLocalRef<jclass>(env, static_cast<jclass>(clazz));
}

std::string DartJni::GetObjectClassName(JNIEnv* env, jobject obj) {
  jclass clazz = env->GetObjectClass(obj);
  DCHECK(clazz);
  jstring name = static_cast<jstring>(
      env->CallObjectMethod(clazz, g_jvm_data->class_get_name_method_id));
  DCHECK(name);

  return base::android::ConvertJavaStringToUTF8(env, name);
}

jclass DartJni::class_clazz() {
  return g_jvm_data->class_clazz.obj();
}

Dart_Handle DartJni::jni_object_type() {
  Dart_Handle object_type = Dart_HandleFromPersistent(
      IsolateData()->jni_object_type);
  DCHECK(!Dart_IsError(object_type));
  return object_type;
}

Dart_Handle DartJni::jni_float_type() {
  Dart_Handle float_type = Dart_HandleFromPersistent(
      IsolateData()->jni_float_type);
  DCHECK(!Dart_IsError(float_type));
  return float_type;
}

void JniMethodArgs::Convert(JNIEnv* env,
                            const std::vector<Dart_Handle>& dart_args,
                            Dart_Handle* exception) {
  jvalues_.reserve(dart_args.size());

  for (Dart_Handle dart_arg : dart_args) {
    jvalue value = DartToJavaValue(env, dart_arg, exception);
    if (*exception) return;
    jvalues_.push_back(value);
  }
}

jvalue JniMethodArgs::DartToJavaValue(JNIEnv* env,
                                      Dart_Handle dart_value,
                                      Dart_Handle* exception) {
  jvalue java_value = jvalue();

  if (Dart_IsBoolean(dart_value)) {
    java_value.z = DartConverter<bool>::FromDart(dart_value);
    return java_value;
  }

  if (Dart_IsInteger(dart_value)) {
    java_value.j = DartConverter<jlong>::FromDart(dart_value);
    return java_value;
  }

  if (Dart_IsDouble(dart_value)) {
    java_value.d = DartConverter<jdouble>::FromDart(dart_value);
    return java_value;
  }

  if (Dart_IsString(dart_value)) {
    intptr_t length;
    Dart_Handle result = Dart_StringLength(dart_value, &length);
    if (CheckDartException(result, exception)) return java_value;

    std::vector<uint16_t> string_data(length);
    result = Dart_StringToUTF16(dart_value, string_data.data(), &length);
    if (CheckDartException(result, exception)) return java_value;

    java_value.l = env->NewString(string_data.data(), length);
    CheckJniException(env, exception);

    return java_value;
  }

  if (Dart_IsNull(dart_value)) {
    java_value.l = nullptr;
    return java_value;
  }

  bool is_object;
  Dart_Handle result = Dart_ObjectIsType(
      dart_value, DartJni::jni_object_type(), &is_object);
  if (CheckDartException(result, exception)) return java_value;

  if (is_object) {
    JniObject* jni_object = DartConverter<JniObject*>::FromDart(dart_value);
    if (jni_object != nullptr) {
      java_value.l = jni_object->java_object();
    } else {
      *exception = ToDart("Invalid JniObject argument");
    }
    return java_value;
  }

  bool is_float;
  result = Dart_ObjectIsType(dart_value, DartJni::jni_float_type(), &is_float);
  if (CheckDartException(result, exception)) return java_value;

  if (is_float) {
    Dart_Handle value_handle = Dart_GetField(dart_value, ToDart("value"));
    if (CheckDartException(value_handle, exception)) return java_value;

    double double_value;
    result = Dart_DoubleValue(value_handle, &double_value);
    if (CheckDartException(result, exception)) return java_value;

    java_value.f = static_cast<jfloat>(double_value);
    return java_value;
  }

  *exception = ToDart("Argument has unsupported data type");
  return java_value;
}

} // namespace blink
