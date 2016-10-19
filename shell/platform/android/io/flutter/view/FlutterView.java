// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.view;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.res.Configuration;
import android.opengl.Matrix;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowInsets;
import android.view.accessibility.AccessibilityManager;
import android.view.accessibility.AccessibilityNodeInfo;
import android.view.accessibility.AccessibilityNodeProvider;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import org.json.JSONException;
import org.json.JSONObject;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;
import org.chromium.mojo.bindings.Interface.Binding;
import org.chromium.mojo.bindings.InterfaceRequest;
import org.chromium.mojo.system.Core;
import org.chromium.mojo.system.MessagePipeHandle;
import org.chromium.mojo.system.MojoException;
import org.chromium.mojo.system.Pair;
import org.chromium.mojo.system.impl.CoreImpl;
import org.chromium.mojom.editing.Keyboard;
import org.chromium.mojom.flutter.platform.ApplicationMessages;
import org.chromium.mojom.mojo.ServiceProvider;
import org.chromium.mojom.sky.AppLifecycleState;
import org.chromium.mojom.sky.ServicesData;
import org.chromium.mojom.sky.SkyEngine;
import org.chromium.mojom.sky.ViewportMetrics;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import io.flutter.plugin.common.ActivityLifecycleListener;
import io.flutter.plugin.editing.TextInputPlugin;
import io.flutter.plugin.platform.PlatformPlugin;

import org.domokit.editing.KeyboardImpl;
import org.domokit.editing.KeyboardViewState;

/**
 * An Android view containing a Flutter app.
 */
@JNINamespace("shell")
public class FlutterView extends SurfaceView
  implements AccessibilityManager.AccessibilityStateChangeListener,
             AccessibilityManager.TouchExplorationStateChangeListener {
    private static final String TAG = "FlutterView";

    private static final String ACTION_DISCOVER = "io.flutter.view.DISCOVER";

    private long mNativePlatformView;
    private TextInputPlugin mTextInputPlugin;

    private SkyEngine.Proxy mSkyEngine;
    private ServiceProviderImpl mPlatformServiceProvider;
    private Binding mPlatformServiceProviderBinding;
    private ServiceProviderImpl mViewServiceProvider;
    private Binding mViewServiceProviderBinding;
    private ServiceProvider.Proxy mDartServiceProvider;
    private ApplicationMessages.Proxy mFlutterAppMessages;
    private HashMap<String, OnMessageListener> mOnMessageListeners;
    private HashMap<String, OnMessageListenerAsync> mAsyncOnMessageListeners;
    private final SurfaceHolder.Callback mSurfaceCallback;
    private final ViewportMetrics mMetrics;
    private final KeyboardViewState mKeyboardState;
    private final AccessibilityManager mAccessibilityManager;
    private BroadcastReceiver discoveryReceiver;
    private List<ActivityLifecycleListener> mActivityLifecycleListeners;

    public FlutterView(Context context) {
        this(context, null);
    }

    public FlutterView(Context context, AttributeSet attrs) {
        super(context, attrs);

        mMetrics = new ViewportMetrics();
        mMetrics.devicePixelRatio = context.getResources().getDisplayMetrics().density;
        setFocusable(true);
        setFocusableInTouchMode(true);

        attach();
        assert mNativePlatformView != 0;

        int color = 0xFF000000;
        TypedValue typedValue = new TypedValue();
        context.getTheme().resolveAttribute(android.R.attr.colorBackground, typedValue, true);
        if (typedValue.type >= TypedValue.TYPE_FIRST_COLOR_INT && typedValue.type <= TypedValue.TYPE_LAST_COLOR_INT)
          color = typedValue.data;
        // TODO(abarth): Consider letting the developer override this color.
        final int backgroundColor = color;

        mSurfaceCallback = new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                assert mNativePlatformView != 0;
                nativeSurfaceCreated(mNativePlatformView, holder.getSurface(), backgroundColor);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                assert mNativePlatformView != 0;
                nativeSurfaceChanged(mNativePlatformView, width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                assert mNativePlatformView != 0;
                nativeSurfaceDestroyed(mNativePlatformView);
            }
        };
        getHolder().addCallback(mSurfaceCallback);

        mKeyboardState = new KeyboardViewState(this);

        Core core = CoreImpl.getInstance();

        mPlatformServiceProvider = new ServiceProviderImpl(core, this, ServiceRegistry.SHARED);

        ServiceRegistry localRegistry = new ServiceRegistry();
        configureLocalServices(localRegistry);
        mViewServiceProvider = new ServiceProviderImpl(core, this, localRegistry);

        mAccessibilityManager = (AccessibilityManager)getContext().getSystemService(Context.ACCESSIBILITY_SERVICE);

        mOnMessageListeners = new HashMap<String, OnMessageListener>();
        mAsyncOnMessageListeners = new HashMap<String, OnMessageListenerAsync>();
        mActivityLifecycleListeners = new ArrayList<ActivityLifecycleListener>();

        setLocale(getResources().getConfiguration().locale);

        // Configure the platform plugin.
        PlatformPlugin platformPlugin = new PlatformPlugin((Activity)getContext());
        addOnMessageListener("flutter/platform", platformPlugin);
        addActivityLifecycleListener(platformPlugin);
        mTextInputPlugin = new TextInputPlugin((Activity)getContext());
        addOnMessageListener("flutter/textinput", mTextInputPlugin);

        if ((context.getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0) {
            discoveryReceiver = new DiscoveryReceiver();
            context.registerReceiver(discoveryReceiver, new IntentFilter(ACTION_DISCOVER));
        }
    }

    private void encodeKeyEvent(KeyEvent event, JSONObject message) throws JSONException {
        message.put("flags", event.getFlags());
        message.put("codePoint", event.getUnicodeChar());
        message.put("keyCode", event.getKeyCode());
        message.put("scanCode", event.getScanCode());
        message.put("metaState", event.getMetaState());
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        try {
            JSONObject message = new JSONObject();
            message.put("type", "keyup");
            message.put("keymap", "android");
            encodeKeyEvent(event, message);
            sendPlatformMessage("flutter/keyevent", message.toString(), null);
        } catch (JSONException e) {
            Log.e(TAG, "Failed to serialize key event", e);
        }
        return true;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        try {
            JSONObject message = new JSONObject();
            message.put("type", "keydown");
            message.put("keymap", "android");
            encodeKeyEvent(event, message);
            sendPlatformMessage("flutter/keyevent", message.toString(), null);
        } catch (JSONException e) {
            Log.e(TAG, "Failed to serialize key event", e);
        }
        return true;
    }

    SkyEngine getEngine() {
        return mSkyEngine;
    }

    public void addActivityLifecycleListener(ActivityLifecycleListener listener) {
        mActivityLifecycleListeners.add(listener);
    }

    public void onPause() {
        mSkyEngine.onAppLifecycleStateChanged(AppLifecycleState.PAUSED);
    }

    public void onPostResume() {
        for (ActivityLifecycleListener listener : mActivityLifecycleListeners)
            listener.onPostResume();

        mSkyEngine.onAppLifecycleStateChanged(AppLifecycleState.RESUMED);
    }

    public void pushRoute(String route) {
        mSkyEngine.pushRoute(route);
    }

    public void popRoute() {
        mSkyEngine.popRoute();
    }

    private void setLocale(Locale locale) {
        mSkyEngine.onLocaleChanged(locale.getLanguage(), locale.getCountry());
    }

    @Override
    protected void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setLocale(newConfig.locale);
    }

    float getDevicePixelRatio() {
        return mMetrics.devicePixelRatio;
    }

    public void destroy() {
        if (discoveryReceiver != null) {
            getContext().unregisterReceiver(discoveryReceiver);
        }

        if (mPlatformServiceProviderBinding != null) {
            mPlatformServiceProviderBinding.unbind().close();
            mPlatformServiceProvider.unbindServices();
        }

        if (mViewServiceProviderBinding != null) {
            mViewServiceProviderBinding.unbind().close();
            mViewServiceProvider.unbindServices();
        }

        getHolder().removeCallback(mSurfaceCallback);
        nativeDetach(mNativePlatformView);
        mNativePlatformView = 0;

        mSkyEngine.close();
        mDartServiceProvider.close();
        mFlutterAppMessages.close();
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        InputConnection connection = mKeyboardState.createInputConnection(outAttrs);
        if (connection == null)
          connection = mTextInputPlugin.createInputConnection(this, outAttrs);
        return connection;
    }

    // Must match the PointerChange enum in pointer.dart.
    private static final int kPointerChangeCancel = 0;
    private static final int kPointerChangeAdd = 1;
    private static final int kPointerChangeRemove = 2;
    private static final int kPointerChangeDown = 3;
    private static final int kPointerChangeMove = 4;
    private static final int kPointerChangeUp = 5;

    // Must match the PointerDeviceKind enum in pointer.dart.
    private static final int kPointerDeviceKindTouch = 0;
    private static final int kPointerDeviceKindMouse = 1;
    private static final int kPointerDeviceKindStylus = 2;
    private static final int kPointerDeviceKindInvertedStylus = 3;

    private int getPointerChangeForAction(int maskedAction) {
        // Primary pointer:
        if (maskedAction == MotionEvent.ACTION_DOWN) {
            return kPointerChangeDown;
        }
        if (maskedAction == MotionEvent.ACTION_UP) {
            return kPointerChangeUp;
        }
        // Secondary pointer:
        if (maskedAction == MotionEvent.ACTION_POINTER_DOWN) {
            return kPointerChangeDown;
        }
        if (maskedAction == MotionEvent.ACTION_POINTER_UP) {
            return kPointerChangeUp;
        }
        // All pointers:
        if (maskedAction == MotionEvent.ACTION_MOVE) {
            return kPointerChangeMove;
        }
        if (maskedAction == MotionEvent.ACTION_CANCEL) {
            return kPointerChangeCancel;
        }
        return -1;
    }

    private int getPointerDeviceTypeForToolType(int toolType) {
        switch (toolType) {
            case MotionEvent.TOOL_TYPE_FINGER:
                return kPointerDeviceKindTouch;
            case MotionEvent.TOOL_TYPE_STYLUS:
                return kPointerDeviceKindStylus;
            case MotionEvent.TOOL_TYPE_MOUSE:
                return kPointerDeviceKindMouse;
            default:
                // MotionEvent.TOOL_TYPE_UNKNOWN will reach here.
                return -1;
        }
    }

    private void addPointerForIndex(MotionEvent event, int pointerIndex,
                                    ByteBuffer packet) {
        int pointerChange = getPointerChangeForAction(event.getActionMasked());
        if (pointerChange == -1) {
            return;
        }

        int pointerKind = event.getToolType(pointerIndex);
        if (pointerKind == -1) {
            return;
        }

        long timeStamp = event.getEventTime() * 1000; // Convert from milliseconds to microseconds.

        packet.putLong(timeStamp); // time_stamp
        packet.putLong(event.getPointerId(pointerIndex)); // pointer
        packet.putLong(pointerChange); // change
        packet.putLong(pointerKind); // kind
        packet.putDouble(event.getX(pointerIndex)); // physical_x
        packet.putDouble(event.getY(pointerIndex)); // physical_y

        if (pointerKind == kPointerDeviceKindMouse) {
          packet.putLong(event.getButtonState() & 0x1F); // buttons
        } else if (pointerKind == kPointerDeviceKindStylus) {
          packet.putLong((event.getButtonState() >> 4) & 0xF); // buttons
        } else {
          packet.putLong(0); // buttons
        }

        packet.putLong(0); // obscured

        // TODO(eseidel): Could get the calibrated range if necessary:
        // event.getDevice().getMotionRange(MotionEvent.AXIS_PRESSURE)
        packet.putDouble(event.getPressure(pointerIndex)); // presure
        packet.putDouble(0.0); // pressure_min
        packet.putDouble(1.0); // pressure_max

        if (pointerKind == kPointerDeviceKindStylus) {
          packet.putDouble(event.getAxisValue(MotionEvent.AXIS_DISTANCE, pointerIndex)); // distance
          packet.putDouble(0.0); // distance_max
        } else {
          packet.putDouble(0.0); // distance
          packet.putDouble(0.0); // distance_max
        }

        packet.putDouble(event.getToolMajor(pointerIndex)); // radius_major
        packet.putDouble(event.getToolMinor(pointerIndex)); // radius_minor

        packet.putDouble(0.0); // radius_min
        packet.putDouble(0.0); // radius_max

        packet.putDouble(event.getAxisValue(MotionEvent.AXIS_ORIENTATION, pointerIndex)); // orientation

        if (pointerKind == kPointerDeviceKindStylus) {
          packet.putDouble(event.getAxisValue(MotionEvent.AXIS_TILT, pointerIndex)); // tilt
        } else {
          packet.putDouble(0.0); // tilt
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // TODO(abarth): This version check might not be effective in some
        // versions of Android that statically compile code and will be upset
        // at the lack of |requestUnbufferedDispatch|. Instead, we should factor
        // version-dependent code into separate classes for each supported
        // version and dispatch dynamically.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            requestUnbufferedDispatch(event);
        }

        // These values must match the unpacking code in hooks.dart.
        final int kPointerDataFieldCount = 19;
        final int kBytePerField = 8;

        int pointerCount = event.getPointerCount();

        ByteBuffer packet = ByteBuffer.allocateDirect(pointerCount * kPointerDataFieldCount * kBytePerField);
        packet.order(ByteOrder.LITTLE_ENDIAN);

        int maskedAction = event.getActionMasked();
        // ACTION_UP, ACTION_POINTER_UP, ACTION_DOWN, and ACTION_POINTER_DOWN
        // only apply to a single pointer, other events apply to all pointers.
        if (maskedAction == MotionEvent.ACTION_UP
                || maskedAction == MotionEvent.ACTION_POINTER_UP
                || maskedAction == MotionEvent.ACTION_DOWN
                || maskedAction == MotionEvent.ACTION_POINTER_DOWN) {
            addPointerForIndex(event, event.getActionIndex(), packet);
        } else {
            // ACTION_MOVE may not actually mean all pointers have moved
            // but it's the responsibility of a later part of the system to
            // ignore 0-deltas if desired.
            for (int p = 0; p < pointerCount; p++) {
                addPointerForIndex(event, p, packet);
            }
        }

        assert packet.position() % (kPointerDataFieldCount * kBytePerField) == 0;
        nativeDispatchPointerDataPacket(mNativePlatformView, packet, packet.position());
        return true;
    }

    @Override
    public boolean onHoverEvent(MotionEvent event) {
        boolean handled = handleAccessibilityHoverEvent(event);
        if (!handled) {
            // TODO(ianh): Expose hover events to the platform,
            // implementing ADD, REMOVE, etc.
        }
        return handled;
    }

    @Override
    protected void onSizeChanged(int width, int height, int oldWidth, int oldHeight) {
        mMetrics.physicalWidth = width;
        mMetrics.physicalHeight = height;
        mSkyEngine.onViewportMetricsChanged(mMetrics);
        super.onSizeChanged(width, height, oldWidth, oldHeight);
    }

    @Override
    public final WindowInsets onApplyWindowInsets(WindowInsets insets) {
        mMetrics.physicalPaddingTop = insets.getSystemWindowInsetTop();
        mMetrics.physicalPaddingRight = insets.getSystemWindowInsetRight();
        mMetrics.physicalPaddingBottom = insets.getSystemWindowInsetBottom();
        mMetrics.physicalPaddingLeft = insets.getSystemWindowInsetLeft();
        mSkyEngine.onViewportMetricsChanged(mMetrics);
        return super.onApplyWindowInsets(insets);
    }

    private void configureLocalServices(ServiceRegistry registry) {
        registry.register(Keyboard.MANAGER.getName(), new ServiceFactory() {
            @Override
            public Binding connectToService(FlutterView view, Core core, MessagePipeHandle pipe) {
                return Keyboard.MANAGER.bind(new KeyboardImpl(view.getContext(), mKeyboardState), pipe);
            }
        });

        registry.register(ApplicationMessages.MANAGER.getName(), new ServiceFactory() {
            @Override
            public Binding connectToService(FlutterView view, Core core, MessagePipeHandle pipe) {
                return ApplicationMessages.MANAGER.bind(new ApplicationMessagesImpl(), pipe);
            }
        });
    }

    private void attach() {
        Core core = CoreImpl.getInstance();
        Pair<SkyEngine.Proxy, InterfaceRequest<SkyEngine>> engine =
                SkyEngine.MANAGER.getInterfaceRequest(core);
        mSkyEngine = engine.first;
        mNativePlatformView =
            nativeAttach(engine.second.passHandle().releaseNativeHandle(), this);
    }

    private void preRun() {
        if (mPlatformServiceProviderBinding != null) {
            mPlatformServiceProviderBinding.unbind().close();
            mPlatformServiceProvider.unbindServices();
        }
        if (mViewServiceProviderBinding != null) {
            mViewServiceProviderBinding.unbind().close();
            mViewServiceProvider.unbindServices();
        }
        if (mDartServiceProvider != null) {
            mDartServiceProvider.close();
        }

        Core core = CoreImpl.getInstance();

        Pair<ServiceProvider.Proxy, InterfaceRequest<ServiceProvider>> dartServiceProvider =
                ServiceProvider.MANAGER.getInterfaceRequest(core);
        mDartServiceProvider = dartServiceProvider.first;

        Pair<ServiceProvider.Proxy, InterfaceRequest<ServiceProvider>> platformServiceProvider =
                ServiceProvider.MANAGER.getInterfaceRequest(core);
        mPlatformServiceProviderBinding = ServiceProvider.MANAGER.bind(
                mPlatformServiceProvider, platformServiceProvider.second);

        Pair<ServiceProvider.Proxy, InterfaceRequest<ServiceProvider>> viewServiceProvider =
                ServiceProvider.MANAGER.getInterfaceRequest(core);
        mViewServiceProviderBinding = ServiceProvider.MANAGER.bind(
                mViewServiceProvider, viewServiceProvider.second);

        ServicesData services = new ServicesData();
        services.incomingServices = platformServiceProvider.first;
        services.outgoingServices = dartServiceProvider.second;
        services.viewServices = viewServiceProvider.first;
        mSkyEngine.setServices(services);

        resetAccessibilityTree();
    }

    private void postRun() {
        Core core = CoreImpl.getInstance();
        // Connect to the ApplicationMessages service exported by the Flutter framework
        Pair<ApplicationMessages.Proxy, InterfaceRequest<ApplicationMessages>> appMessages =
                  ApplicationMessages.MANAGER.getInterfaceRequest(core);
        mDartServiceProvider.connectToService(ApplicationMessages.MANAGER.getName(),
                                              appMessages.second.passHandle());
        mFlutterAppMessages = appMessages.first;
    }

    public void runFromBundle(String bundlePath, String snapshotPath) {
        preRun();

        if (FlutterMain.isRunningPrecompiledCode()) {
            mSkyEngine.runFromPrecompiledSnapshot(bundlePath);
        } else {
            String scriptUri = "file://" + bundlePath;
            if (snapshotPath != null) {
                mSkyEngine.runFromBundleAndSnapshot(scriptUri, bundlePath, snapshotPath);
            } else {
                mSkyEngine.runFromBundle(scriptUri, bundlePath);
            }
        }

        postRun();
    }

    public void runFromSource(final String main,
                              final String packages,
                              final String assetsDirectory) {
        Runnable runnable = new Runnable() {
            public void run() {
                preRun();
                mSkyEngine.runFromFile(main,
                                       packages,
                                       assetsDirectory);
                postRun();
                synchronized (this) {
                    notify();
                }
            }
        };

        try {
            synchronized (runnable) {
                // Post to the Android UI thread and wait for the response.
                post(runnable);
                runnable.wait();
            }
        } catch (InterruptedException e) {
            Log.e(TAG, "Thread got interrupted waiting for " +
                       "RunFromSourceRunnable to finish", e);
        }
    }

    /** Return the most recent frame as a bitmap. */
    public Bitmap getBitmap() {
        return nativeGetBitmap(mNativePlatformView);
    }

    private static native long nativeAttach(int skyEngineHandle,
                                            FlutterView view);
    private static native int nativeGetObservatoryPort();
    private static native void nativeDetach(long nativePlatformViewAndroid);
    private static native void nativeSurfaceCreated(long nativePlatformViewAndroid,
                                                    Surface surface,
                                                    int backgroundColor);
    private static native void nativeSurfaceChanged(long nativePlatformViewAndroid,
                                                    int width,
                                                    int height);
    private static native void nativeSurfaceDestroyed(long nativePlatformViewAndroid);
    private static native Bitmap nativeGetBitmap(long nativePlatformViewAndroid);

    // Send a platform message to Dart.
    private static native void nativeDispatchPlatformMessage(long nativePlatformViewAndroid, String name, String message, int responseId);
    private static native void nativeDispatchPointerDataPacket(long nativePlatformViewAndroid, ByteBuffer buffer, int position);
    private static native void nativeDispatchSemanticsAction(long nativePlatformViewAndroid, int id, int action);
    private static native void nativeSetSemanticsEnabled(long nativePlatformViewAndroid, boolean enabled);

    // Send a response to a platform message received from Dart.
    private static native void nativeInvokePlatformMessageResponseCallback(long nativePlatformViewAndroid, int responseId, String message);

    // Called by native to send us a platform message.
    @CalledByNative
    private void handlePlatformMessage(String name, String message, final int responseId) {
        OnMessageListener listener = mOnMessageListeners.get(name);
        if (listener != null) {
            nativeInvokePlatformMessageResponseCallback(mNativePlatformView, responseId, listener.onMessage(this, message));
            return;
        }

        OnMessageListenerAsync asyncListener = mAsyncOnMessageListeners.get(name);
        if (asyncListener != null) {
            asyncListener.onMessage(this, message, new MessageResponse() {
                @Override
                public void send(String response) {
                    nativeInvokePlatformMessageResponseCallback(mNativePlatformView, responseId, response);
                }
            });
            return;
        }

        nativeInvokePlatformMessageResponseCallback(mNativePlatformView, responseId, null);
    }

    private int mNextResponseId = 1;
    private final Map<Integer, MessageReplyCallback> mPendingResponses = new HashMap<Integer, MessageReplyCallback>();

    // Called by native to respond to a platform message that we sent.
    @CalledByNative
    private void handlePlatformMessageResponse(int responseId, String response) {
        MessageReplyCallback callback = mPendingResponses.remove(responseId);
        if (callback != null)
            callback.onReply(response);
    }

    @CalledByNative
    private void updateSemantics(ByteBuffer buffer, String[] strings) {
        if (mAccessibilityNodeProvider != null) {
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            mAccessibilityNodeProvider.updateSemantics(buffer, strings);
        }
    }

    // ACCESSIBILITY

    private boolean mAccessibilityEnabled = false;
    private boolean mTouchExplorationEnabled = false;

    protected void dispatchSemanticsAction(int id, int action) {
        nativeDispatchSemanticsAction(mNativePlatformView, id, action);
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        mAccessibilityEnabled = mAccessibilityManager.isEnabled();
        mTouchExplorationEnabled = mAccessibilityManager.isTouchExplorationEnabled();
        if (mAccessibilityEnabled || mTouchExplorationEnabled)
          ensureAccessibilityEnabled();
        resetWillNotDraw();
        mAccessibilityManager.addAccessibilityStateChangeListener(this);
        mAccessibilityManager.addTouchExplorationStateChangeListener(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mAccessibilityManager.removeAccessibilityStateChangeListener(this);
        mAccessibilityManager.removeTouchExplorationStateChangeListener(this);
    }

    private void resetWillNotDraw() {
        setWillNotDraw(!(mAccessibilityEnabled || mTouchExplorationEnabled));
    }

    @Override
    public void onAccessibilityStateChanged(boolean enabled) {
        if (enabled) {
            mAccessibilityEnabled = true;
            ensureAccessibilityEnabled();
        } else {
            mAccessibilityEnabled = false;
        }
        if (mAccessibilityNodeProvider != null) {
            mAccessibilityNodeProvider.setAccessibilityEnabled(mAccessibilityEnabled);
        }
        resetWillNotDraw();
    }

    @Override
    public void onTouchExplorationStateChanged(boolean enabled) {
        if (enabled) {
            mTouchExplorationEnabled = true;
            ensureAccessibilityEnabled();
        } else {
            mTouchExplorationEnabled = false;
            if (mAccessibilityNodeProvider != null) {
                mAccessibilityNodeProvider.handleTouchExplorationExit();
            }
        }
        resetWillNotDraw();
    }

    @Override
    public AccessibilityNodeProvider getAccessibilityNodeProvider() {
        ensureAccessibilityEnabled();
        return mAccessibilityNodeProvider;
    }

    private AccessibilityBridge mAccessibilityNodeProvider;

    void ensureAccessibilityEnabled() {
        if (mAccessibilityNodeProvider == null) {
            mAccessibilityNodeProvider = new AccessibilityBridge(this);
            nativeSetSemanticsEnabled(mNativePlatformView, true);
        }
    }

    void resetAccessibilityTree() {
        if (mAccessibilityNodeProvider != null) {
            mAccessibilityNodeProvider.reset();
        }
    }

    private boolean handleAccessibilityHoverEvent(MotionEvent event) {
        if (!mTouchExplorationEnabled)
            return false;
        if (event.getAction() == MotionEvent.ACTION_HOVER_ENTER ||
                   event.getAction() == MotionEvent.ACTION_HOVER_MOVE) {
            mAccessibilityNodeProvider.handleTouchExploration(event.getX(), event.getY());
        } else if (event.getAction() == MotionEvent.ACTION_HOVER_EXIT) {
            mAccessibilityNodeProvider.handleTouchExplorationExit();
        } else {
            Log.d("flutter", "unexpected accessibility hover event: " + event);
            return false;
        }
        return true;
    }

    /**
     * Send a message to the Flutter application. The Flutter application can
     * register a platform message handler that will receive these messages with
     * the PlatformMessages object.
     */
    public void sendPlatformMessage(String name, String message, MessageReplyCallback callback) {
        int responseId = 0;
        if (callback != null) {
            responseId = mNextResponseId++;
            mPendingResponses.put(responseId, callback);
        }
        nativeDispatchPlatformMessage(mNativePlatformView, name, message, responseId);
    }

    /**
     * Send a message to the Flutter application.  The Flutter Dart code can register a
     * host message handler that will receive these messages.
     */
    public void sendToFlutter(String messageName, String message,
                              final MessageReplyCallback callback) {
        // TODO(abarth): Switch to dispatchPlatformMessage once the framework
        // side has been converted.
        mFlutterAppMessages.sendString(messageName, message,
            new ApplicationMessages.SendStringResponse() {
                @Override
                public void call(String reply) {
                    if (callback != null) {
                        callback.onReply(reply);
                    }
                }
            });
    }

    public void sendToFlutter(String messageName, String message) {
        sendToFlutter(messageName, message, null);
    }

    /** Callback invoked when the app replies to a message sent with sendToFlutter. */
    public interface MessageReplyCallback {
        void onReply(String reply);
    }

    /**
     * Register a callback to be invoked when the Flutter application sends a message
     * to its host.
     */
    public void addOnMessageListener(String messageName, OnMessageListener listener) {
        mOnMessageListeners.put(messageName, listener);
    }

    /**
     * Register a callback to be invoked when the Flutter application sends a message
     * to its host.  The reply to the message can be provided asynchronously.
     */
    public void addOnMessageListenerAsync(String messageName, OnMessageListenerAsync listener) {
        mAsyncOnMessageListeners.put(messageName, listener);
    }

    public interface OnMessageListener {
        /**
         * Called when a message is received from the Flutter app.
         * @return the reply to the message (can be null)
         */
        String onMessage(FlutterView view, String message);
    };

    public interface OnMessageListenerAsync {
        /**
         * Called when a message is received from the Flutter app.
         * @param response Used to send a reply back to the app.
         */
        void onMessage(FlutterView view, String message, MessageResponse response);
    }

    public interface MessageResponse {
        void send(String reply);
    }

    private class ApplicationMessagesImpl implements ApplicationMessages {
        @Override
        public void close() {}

        @Override
        public void onConnectionError(MojoException e) {}

        @Override
        public void sendString(String messageName, String message, SendStringResponse callback) {
            OnMessageListener listener = mOnMessageListeners.get(messageName);
            if (listener != null) {
                callback.call(listener.onMessage(FlutterView.this, message));
                return;
            }

            OnMessageListenerAsync asyncListener = mAsyncOnMessageListeners.get(messageName);
            if (asyncListener != null) {
                asyncListener.onMessage(FlutterView.this, message, new MessageResponseAdapter(callback));
                return;
            }

            callback.call(null);
        }
    }

    /**
     * This class wraps the raw Mojo callback object in an interface that is owned
     * by Flutter and can be safely given to host apps.
     */
    private static class MessageResponseAdapter implements MessageResponse {
        private ApplicationMessages.SendStringResponse callback;

        MessageResponseAdapter(ApplicationMessages.SendStringResponse callback) {
            this.callback = callback;
        }

        @Override
        public void send(String reply) {
            callback.call(reply);
        }
    }

    /** Broadcast receiver used to discover active Flutter instances. */
    private class DiscoveryReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            JSONObject discover = new JSONObject();
            try {
                discover.put("id", getContext().getPackageName());
                discover.put("observatoryPort", nativeGetObservatoryPort());
                Log.i(TAG, "DISCOVER: " + discover);
            } catch (JSONException e) {}
        }
    }
}
