// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.plugin.common;

import java.util.Map;
import org.json.JSONObject;

/**
 * Command object representing a method call on a {@link MethodChannel}.
 */
public final class MethodCall {
    /**
     * The name of the called method.
     */
    public final String method;

    /**
     * Arguments for the call.
     *
     * Consider using {@link #arguments()} for cases where a particular run-time type is expected.
     * Consider using {@link #argument(String)} when that run-time type is a {@link Map}
     * or a {@link JSONObject}.
     */
    public final Object arguments;

    /**
     * Creates a {@link MethodCall} with the specified method name and arguments.
     *
     * @param method the method name String, not null.
     * @param arguments the arguments, a value supported by the channel's message codec.
     */
    public MethodCall(String method, Object arguments) {
        assert method != null;
        this.method = method;
        this.arguments = arguments;
    }

    /**
     * Returns the arguments of this method call with a static type determined by the call-site.
     *
     * @param <T> the intended type of the arguments.
     * @return the arguments with static type T
     */
    @SuppressWarnings("unchecked")
    public <T> T arguments() {
        return (T) arguments;
    }

    /**
     * Returns a String-keyed argument of this method call, assuming {@link #arguments} is a
     * {@link Map} or a {@link JSONObject}. The static type of the returned result is determined
     * by the call-site.
     *
     * @param <T> the intended type of the argument.
     * @param key the String key.
     * @return the argument value at the specified key, with static type T, or {@code null}, if
     * such an entry is not present.
     * @throws ClassCastException if {@link #arguments} can be cast to neither {@link Map} nor
     * {@link JSONObject}.
     */
    @SuppressWarnings("unchecked")
    public <T> T argument(String key) {
        if (arguments == null) {
            return null;
        } else if (arguments instanceof Map) {
            return (T) ((Map<?, ?>) arguments).get(key);
        } else if (arguments instanceof JSONObject) {
            return (T) ((JSONObject) arguments).opt(key);
        } else {
            throw new ClassCastException();
        }
    }
}
