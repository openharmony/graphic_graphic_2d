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

#include <string>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "window.h"

#include "drawing_sample_replayer.h"


namespace OHOS::Rosen {

DrawingSampleReplayer::~DrawingSampleReplayer()
{
    DestoryNativeWindow(nativeWindow_);
    surfaceNode_->DetachToDisplay(DEFAULT_DISPLAY_ID);
}

int DrawingSampleReplayer::ReadCmds(const std::string path)
{
    int32_t fd = open(path.c_str(), O_RDONLY);
    if (fd <= 0) {
        return -1;
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        return -1;
    }
    auto mapFile = static_cast<uint8_t*>(mmap(nullptr, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapFile == MAP_FAILED) {
        close(fd);
        return -1;
    }
    MessageParcel messageParcel(new EmptyAllocator());
    if (!messageParcel.ParseFrom(reinterpret_cast<uintptr_t>(mapFile), statbuf.st_size)) {
        munmap(mapFile, statbuf.st_size);
        close(fd);
        return -1;
    }
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    RSMarshallingHelper::Unmarshalling(messageParcel, dcl_);
    RSMarshallingHelper::EndNoSharedMem();
    if (dcl_ == nullptr) {
        std::cout << "dcl is nullptr" << std::endl;
        munmap(mapFile, statbuf.st_size);
        return -1;
    }
    munmap(mapFile, statbuf.st_size);
    return 0;
}

void DrawingSampleReplayer::PrepareFrame(Drawing::Canvas* canvas)
{
    if (canvas == nullptr) {
        return;
    }

#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    if (captureMode_ == CaptureMode::SKP) {
        orgSkiaCanvas_ = canvas->GetImpl<Drawing::SkiaCanvas>()->ExportSkCanvas();
        recorder_ = std::make_unique<SkPictureRecorder>();
        pictureCanvas_ = recorder_->beginRecording(width_, height_);
        if (nwayCanvas_ == nullptr) {
            nwayCanvas_ = std::make_unique<SkNWayCanvas>(width_, height_);
        }
        nwayCanvas_->addCanvas(orgSkiaCanvas_);
        nwayCanvas_->addCanvas(pictureCanvas_);

        canvas->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(nwayCanvas_.get());
    }
#endif

    if (ReadCmds("/data/default.drawing") < 0) {
        std::cout << "Failed to load .drawing cmds" << std::endl;
        return;
    }
    dcl_->Playback(*canvas);

#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    if (captureMode_ == CaptureMode::SKP) {
        // finishing capturing and saving skp
        nwayCanvas_->removeAll();
        sk_sp<SkPicture> picture = recorder_->finishRecordingAsPicture();
        if (picture->approximateOpCount() > 0) {
            SkSerialProcs procs;
            procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
                return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
            };
            auto data = picture->serialize(&procs);
            // to save locally
            std::string outputFile = "/data/default.skp";
            SkFILEWStream stream(outputFile.c_str());
            if (stream.isValid()) {
                stream.write(data->data(), data->size());
                stream.flush();
                std::cout << "Written and flushed" << std::endl;
            } else {
                std::cout << "Failed to create stream" << std::endl;
            }
        }
        canvas->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(orgSkiaCanvas_);
    }
#endif
}

void DrawingSampleReplayer::RenderLoop()
{
    PrepareNativeEGLSetup();
    int maxIter = (captureMode_ == CaptureMode::RDC) ? MAX_RENDERED_FRAMES : 1;

    for (int i = 0; i < maxIter; ++i) {
        std::shared_ptr<Drawing::Surface> drawingSurface = renderContext_->AcquireSurface(width_, height_);
        Drawing::Canvas* canvas = drawingSurface->GetCanvas().get();

        std::cout << i << "\t >> new iteration" << std::endl;

        PrepareFrame(canvas);

        renderContext_->RenderFrame(); // which involves flush()
        renderContext_->SwapBuffers(eglSurface_);

        int rdcnum = 0;
        std::string path = "/data/autocaps";
        // Checks if the folder exists
        if (!fs::exists(path)) {
            continue;
        }
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
            const std::filesystem::path& path = entry.path();
            if (path.extension() == ".rdc") {
                rdcnum++;
            }
        }
        if (rdcnum > 0) {
            return;
        }
    }
}

void DrawingSampleReplayer::PrepareNativeEGLSetup()
{
    renderContext_ = std::make_shared<RenderContext>();
    renderContext_->InitializeEglContext();

    auto defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    width_ = defaultDisplay->GetWidth();
    height_ = defaultDisplay->GetHeight();

    std::cout << "Width: " << width_ << "  -  Height: " << height_ << std::endl;

    RSSurfaceNodeConfig surfaceNodeConfig = {"DrawingEngineSample"};
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode_) {
        std::cout << "surfaceNode is nullptr" << std::endl;
        return;
    }

    surfaceNode_->SetFrameGravity(Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode_->SetBounds(0, 0, width_, height_);
    surfaceNode_->AttachToDisplay(DEFAULT_DISPLAY_ID);
    RSTransaction::FlushImplicitTransaction();

    sptr<Surface> ohosSurface = surfaceNode_->GetSurface();
    nativeWindow_ = CreateNativeWindowFromSurface(&ohosSurface);
    if (nativeWindow_ == nullptr) {
        std::cout << "CreateNativeWindowFromSurface Failed" << std::endl;
        return;
    }

    eglSurface_ = renderContext_->CreateEGLSurface((EGLNativeWindowType)nativeWindow_);
    if (eglSurface_ == EGL_NO_SURFACE) {
        std::cout << "Invalid eglSurface, return" << std::endl;
        DestoryNativeWindow(nativeWindow_);
        return;
    }

    renderContext_->MakeCurrent(eglSurface_);
    EGLint initRes = eglGetError();
    if (initRes != EGL_SUCCESS) {
        std::cout << "Fail to init egl" << std::endl;
        DestoryNativeWindow(nativeWindow_);
        return;
    }
    NativeWindowHandleOpt(nativeWindow_, SET_BUFFER_GEOMETRY, width_, height_);
}

void DrawingSampleReplayer::SetCaptureMode(CaptureMode mode)
{
    captureMode_ = mode;
}

} // namespace OHOS::Rosen

int32_t main(int32_t argc, char *argv[])
{
    OHOS::Rosen::DrawingSampleReplayer replayer;
    std::string replayType = "default";
    if (argc > 1) {
        replayType = std::string(argv[1]);
        if (replayType == "skp") {
            replayer.SetCaptureMode(OHOS::Rosen::CaptureMode::SKP);
        } else if (replayType == "rdc") {
            replayer.SetCaptureMode(OHOS::Rosen::CaptureMode::RDC);
        }
    }
    std::cout << "Mode: " << replayType << std::endl;
    replayer.RenderLoop();
    return 0;
}
