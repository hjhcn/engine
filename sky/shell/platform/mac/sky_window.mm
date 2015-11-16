// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "sky_window.h"
#include "base/command_line.h"
#include "base/time/time.h"
#include "mojo/public/cpp/bindings/interface_request.h"
#include "sky/services/engine/input_event.mojom.h"
#include "sky/services/pointer/pointer.mojom.h"
#include "sky/shell/platform/mac/platform_view_mac.h"
#include "sky/shell/shell_view.h"
#include "sky/shell/shell.h"
#include "sky/shell/switches.h"
#include "sky/shell/ui_delegate.h"

@interface SkyWindow ()<NSWindowDelegate>

@property(assign) IBOutlet NSOpenGLView* renderSurface;
@property(getter=isSurfaceSetup) BOOL surfaceSetup;

@end

static inline pointer::PointerType EventTypeFromNSEventPhase(NSEventPhase phase) {
  switch (phase) {
    case NSEventPhaseNone:
      return pointer::PointerType::CANCEL;
    case NSEventPhaseBegan:
      return pointer::PointerType::DOWN;
    case NSEventPhaseStationary:
    // There is no EVENT_TYPE_POINTER_STATIONARY. So we just pass a move type
    // with the same coordinates
    case NSEventPhaseChanged:
      return pointer::PointerType::MOVE;
    case NSEventPhaseEnded:
      return pointer::PointerType::UP;
    case NSEventPhaseCancelled:
      return pointer::PointerType::CANCEL;
    case NSEventPhaseMayBegin:
      return pointer::PointerType::CANCEL;
  }
  return pointer::PointerType::CANCEL;
}

@implementation SkyWindow {
  sky::SkyEnginePtr _sky_engine;
  scoped_ptr<sky::shell::ShellView> _shell_view;
}

@synthesize renderSurface = _renderSurface;
@synthesize surfaceSetup = _surfaceSetup;

- (void)awakeFromNib {
  [super awakeFromNib];

  self.delegate = self;

  [self updateWindowSize];
}

- (void)setupShell {
  NSAssert(_shell_view == nullptr, @"The shell view must not already be set");
  auto shell_view = new sky::shell::ShellView(sky::shell::Shell::Shared());
  _shell_view.reset(shell_view);

  auto widget = reinterpret_cast<gfx::AcceleratedWidget>(self.renderSurface);
  self.platformView->SurfaceCreated(widget);
}

- (NSString*)skyInitialBundleURL {
  return [[NSBundle mainBundle] pathForResource:@"app" ofType:@"flx"];
}

// TODO(eseidel): This does not belong in sky_window!
// Probably belongs in NSApplicationDelegate didFinishLaunching.
// We also want a separate setup for normal apps vs SkyShell
// normal apps only use a flx vs. SkyShell which always pulls from network.
- (void)setupAndLoadDart {
  auto interface_request = mojo::GetProxy(&_sky_engine);
  self.platformView->ConnectToEngine(interface_request.Pass());

  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();

  if (command_line.HasSwitch(sky::shell::switches::kSnapshot)) {
    auto snapshot = command_line.GetSwitchValueASCII(sky::shell::switches::kSnapshot);
    _sky_engine->RunFromSnapshot(snapshot);
    return;
  }

  auto args = command_line.GetArgs();
  if (args.size() > 0) {
    _sky_engine->RunFromFile(args[0],
        command_line.GetSwitchValueASCII(sky::shell::switches::kPackageRoot));
    return;
  }

  NSString *endpoint = self.skyInitialBundleURL;
  if (endpoint.length > 0) {
    mojo::String string(endpoint.UTF8String);
    _sky_engine->RunFromBundle(string);
    return;
  }
}

- (void)windowDidResize:(NSNotification*)notification {
  [self updateWindowSize];
}

- (void)updateWindowSize {
  [self setupSurfaceIfNecessary];

  auto metrics = sky::ViewportMetrics::New();
  auto size = self.renderSurface.frame.size;
  metrics->physical_width = size.width;
  metrics->physical_height = size.height;
  metrics->device_pixel_ratio = 1.0;
  _sky_engine->OnViewportMetricsChanged(metrics.Pass());
}

- (void)setupSurfaceIfNecessary {
  if (self.isSurfaceSetup) {
    return;
  }

  self.surfaceSetup = YES;

  [self setupShell];
  [self setupAndLoadDart];
}

- (sky::shell::PlatformViewMac*)platformView {
  auto view = static_cast<sky::shell::PlatformViewMac*>(_shell_view->view());
  DCHECK(view);
  return view;
}

#pragma mark - Responder overrides

- (void)dispatchEvent:(NSEvent*)event phase:(NSEventPhase)phase {
  NSPoint location =
      [_renderSurface convertPoint:event.locationInWindow fromView:nil];
  location.y = _renderSurface.frame.size.height - location.y;

  auto pointer_data = pointer::Pointer::New();

  pointer_data->time_stamp =
      base::TimeDelta::FromSecondsD(event.timestamp).InMilliseconds();
  pointer_data->type = EventTypeFromNSEventPhase(phase);
  pointer_data->kind = pointer::PointerKind::TOUCH;
  pointer_data->pointer = 0;
  pointer_data->x = location.x;
  pointer_data->y = location.y;
  pointer_data->buttons = 0;
  pointer_data->down = false;
  pointer_data->primary = false;
  pointer_data->obscured = false;
  pointer_data->pressure = 1.0;
  pointer_data->pressure_min = 0.0;
  pointer_data->pressure_max = 1.0;
  pointer_data->distance = 0.0;
  pointer_data->distance_min = 0.0;
  pointer_data->distance_max = 0.0;
  pointer_data->radius_major = 0.0;
  pointer_data->radius_minor = 0.0;
  pointer_data->radius_min = 0.0;
  pointer_data->radius_max = 0.0;
  pointer_data->orientation = 0.0;
  pointer_data->tilt = 0.0;

  auto pointer_packet = pointer::PointerPacket::New();
  pointer_packet->pointers.push_back(pointer_data.Pass());
  _sky_engine->OnPointerPacket(pointer_packet.Pass());
}

- (void)mouseDown:(NSEvent*)event {
  [self dispatchEvent:event phase:NSEventPhaseBegan];
}

- (void)mouseDragged:(NSEvent*)event {
  [self dispatchEvent:event phase:NSEventPhaseChanged];
}

- (void)mouseUp:(NSEvent*)event {
  [self dispatchEvent:event phase:NSEventPhaseEnded];
}

- (void)dealloc {
  self.platformView->SurfaceDestroyed();
  [super dealloc];
}

@end
