// SKY_ENGINE_CORE_PAINTING_CANVASIMAGE_H_
// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_LOADER_CANVASIMAGELOADER_H_
#define SKY_ENGINE_CORE_LOADER_CANVASIMAGELOADER_H_

#include "base/memory/weak_ptr.h"
#include "mojo/data_pipe_utils/data_pipe_drainer.h"
#include "sky/engine/core/loader/ImageDecoderCallback.h"
#include "sky/engine/platform/SharedBuffer.h"
#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/tonic/uint8_list.h"
#include "sky/engine/wtf/OwnPtr.h"
#include "sky/engine/wtf/text/AtomicString.h"

namespace blink {

class CanvasImageDecoder : public mojo::common::DataPipeDrainer::Client,
                           public RefCounted<CanvasImageDecoder>,
                           public DartWrappable {
  DEFINE_WRAPPERTYPEINFO();
 public:
  static PassRefPtr<CanvasImageDecoder> create(PassOwnPtr<ImageDecoderCallback> callback);
  virtual ~CanvasImageDecoder();

  // mojo::common::DataPipeDrainer::Client
  void OnDataAvailable(const void*, size_t) override;
  void OnDataComplete() override;

  void initWithConsumer(mojo::ScopedDataPipeConsumerHandle handle);
  void initWithList(const Uint8List& list);

 private:
  CanvasImageDecoder(PassOwnPtr<ImageDecoderCallback> callback);

  void RejectCallback();

  OwnPtr<mojo::common::DataPipeDrainer> drainer_;
  RefPtr<SharedBuffer> buffer_;
  OwnPtr<ImageDecoderCallback> callback_;

  base::WeakPtrFactory<CanvasImageDecoder> weak_factory_;
};

}  // namespace blink

#endif  // SKY_ENGINE_CORE_LOADER_CANVASIMAGELOADER_H_
