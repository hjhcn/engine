// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.view;

/**
 * A class representing information for a callback registered using
 * `PluginUtilities` from `dart:ui`.
 */
public final class FlutterCallbackInformation {
  final public String callbackName;
  final public String callbackClassName;
  final public String callbackLibraryPath;

  /**
   * Get callback information for a given handle.
   * @param handle the handle for the callback, generated by
   * `PluginUtilities.getCallbackHandle` in `dart:ui`.
   * @return an instance of FlutterCallbackInformation for the provided handle.
   */
  public static FlutterCallbackInformation lookupCallbackInformation(long handle) {
    return nativeLookupCallbackInformation(handle);
  }

  private FlutterCallbackInformation(String callbackName,
      String callbackClassName, String callbackLibraryPath) {
    this.callbackName = callbackName;
    this.callbackClassName = callbackClassName;
    this.callbackLibraryPath = callbackLibraryPath;
  }

  private static native FlutterCallbackInformation nativeLookupCallbackInformation(long handle);
}
