// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package io.flutter.plugin.platform;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.os.Build;
import android.view.Surface;
import android.view.View;
import android.view.ViewTreeObserver;

@TargetApi(Build.VERSION_CODES.KITKAT_WATCH)
class VirtualDisplayController {

    public static VirtualDisplayController create(
            Context context,
            PlatformViewFactory viewFactory,
            SurfaceTexture surfaceTexture,
            int width,
            int height,
            int viewId
    ) {
        surfaceTexture.setDefaultBufferSize(width, height);
        Surface surface = new Surface(surfaceTexture);
        DisplayManager displayManager = (DisplayManager) context.getSystemService(Context.DISPLAY_SERVICE);

        int densityDpi = context.getResources().getDisplayMetrics().densityDpi;
        VirtualDisplay virtualDisplay = displayManager.createVirtualDisplay(
                "flutter-vd",
                width,
                height,
                densityDpi,
                surface,
                0
        );

        if (virtualDisplay == null) {
            return null;
        }

        return new VirtualDisplayController(context, virtualDisplay, viewFactory, surface, surfaceTexture, viewId);
    }

    private final Context mContext;
    private final int mDensityDpi;
    private final SurfaceTexture mSurfaceTexture;
    private VirtualDisplay mVirtualDisplay;
    private SingleViewPresentation mPresentation;
    private Surface mSurface;


    private VirtualDisplayController(
            Context context,
            VirtualDisplay virtualDisplay,
            PlatformViewFactory viewFactory,
            Surface surface,
            SurfaceTexture surfaceTexture,
            int viewId
    ) {
        mSurfaceTexture = surfaceTexture;
        mSurface = surface;
        mContext = context;
        mVirtualDisplay = virtualDisplay;
        mDensityDpi = context.getResources().getDisplayMetrics().densityDpi;
        mPresentation = new SingleViewPresentation(context, mVirtualDisplay.getDisplay(), viewFactory, viewId);
        mPresentation.show();
    }

    public void resize(final int width, final int height, final Runnable onNewSizeFrameAvailable) {
        final PlatformView view = mPresentation.detachView();
        // We detach the surface to prevent it being destroyed when releasing the vd.
        //
        // setSurface is only available starting API 20. We could support API 19 by re-creating a new
        // SurfaceTexture here. This will require refactoring the TextureRegistry to allow recycling texture
        // entry IDs.
        mVirtualDisplay.setSurface(null);
        mVirtualDisplay.release();

        mSurfaceTexture.setDefaultBufferSize(width, height);
        DisplayManager displayManager = (DisplayManager) mContext.getSystemService(Context.DISPLAY_SERVICE);
        mVirtualDisplay = displayManager.createVirtualDisplay(
                "flutter-vd",
                width,
                height,
                mDensityDpi,
                mSurface,
                0
        );

        final View embeddedView = getView();
        // There's a bug in Android version older than O where view tree observer onDrawListeners don't get properly
        // merged when attaching to window, as a workaround we register the on draw listener after the view is attached.
        embeddedView.addOnAttachStateChangeListener(new View.OnAttachStateChangeListener() {
            @Override
            public void onViewAttachedToWindow(View v) {
                embeddedView.getViewTreeObserver().addOnDrawListener(new ViewTreeObserver.OnDrawListener() {
                    @Override
                    public void onDraw() {
                        // We need some delay here until the frame propagates through the vd surface to to the texture,
                        // 128ms was picked pretty arbitrarily based on trial and error.
                        // As long as we invoke the runnable after a new frame is available we avoid the scaling jank
                        // described in: https://github.com/flutter/flutter/issues/19572
                        // We should ideally run onNewSizeFrameAvailable ASAP to make the embedded view more responsive
                        // following a resize.
                        embeddedView.postDelayed(onNewSizeFrameAvailable, 128);
                        embeddedView.getViewTreeObserver().removeOnDrawListener(this);
                    }
                });
                embeddedView.removeOnAttachStateChangeListener(this);
            }

            @Override
            public void onViewDetachedFromWindow(View v) {}
        });

        mPresentation = new SingleViewPresentation(mContext, mVirtualDisplay.getDisplay(), view);
        mPresentation.show();
    }

    public void dispose() {
        mPresentation.detachView().dispose();
        mVirtualDisplay.release();
    }

    public View getView() {
        if (mPresentation == null)
            return null;
        return mPresentation.getView();
    }
}
