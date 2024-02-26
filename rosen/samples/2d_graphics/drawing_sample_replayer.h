/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DRAWING_SAMPLE_REPLAYER_H
#define DRAWING_SAMPLE_REPLAYER_H

#include "display_manager.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"

#include "skia_adapter/skia_canvas.h"

#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSerialProcs.h"
#include "include/utils/SkNWayCanvas.h"

namespace OHOS::Rosen {

enum CaptureMode {
    RDC = 0,
    SKP,
    LAST,
};

class EmptyAllocator : public DefaultAllocator {
public:
    EmptyAllocator() = default;
    ~EmptyAllocator() = default;
    void* Realloc(void *data, size_t newSize) override { return nullptr; }
    void* Alloc(size_t size) override { return nullptr; }
    void Dealloc(void *data) override { }
};

class DrawingSampleReplayer {
public:
    DrawingSampleReplayer() = default;
    ~DrawingSampleReplayer();

    void RenderLoop();
    void SetCaptureMode(CaptureMode mode);
private:
    int ReadCmds(const std::string path);
    void PrepareFrame(Drawing::Canvas* canvas);
    void PrepareNativeEGLSetup();

    enum CaptureMode captureMode_ = CaptureMode::RDC;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RenderContext> renderContext_;
    OHNativeWindow* nativeWindow_ = nullptr;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;

    std::shared_ptr<Drawing::DrawCmdList> dcl_;

    // for .skp
    SkCanvas* pictureCanvas_ = nullptr;
    SkCanvas* orgSkiaCanvas_ = nullptr;
    std::unique_ptr<SkPictureRecorder> recorder_;
    std::unique_ptr<SkNWayCanvas> nwayCanvas_;

    const uint32_t MAX_RENDERED_FRAMES = 50;
    const uint32_t DEFAULT_DISPLAY_ID = 0;
};

} // namespace OHOS::Rosen

#endif // DRAWING_SAMPLE_REPLAYER_H