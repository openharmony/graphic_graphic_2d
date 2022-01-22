/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <surface.h>
#include <unistd.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "png.h"
#include "render_context/render_context.h"
#include "string_ex.h"
#include "surface_buffer.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "unique_fd.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
// temporary debug
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"


using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace detail {
constexpr int MILLI_SECS_PER_SECOND = 1000;
constexpr int MICRO_SECS_PER_MILLISECOND = 1000;
constexpr int MICRO_SECS_PER_SECOND = MICRO_SECS_PER_MILLISECOND * MILLI_SECS_PER_SECOND;
constexpr uint8_t BIT_DEPTH_VALUE = 8;
constexpr int SETW_VALUE = 2;
constexpr char SETFILL_VALUE = '0';
constexpr int SLEEP_TIME = 1;

RenderContext* renderContext = nullptr;

template <typename Duration>
using SysTime = std::chrono::time_point<std::chrono::system_clock, Duration>;
using SysMicroSeconds = SysTime<std::chrono::microseconds>;

using WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};

bool WriteToPng(const std::string &fileName, const WriteToPngParam &param)
{
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        std::cout << "png_create_write_struct error, nullptr!" << std::endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        std::cout << "png_create_info_struct error, nullptr!" << std::endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        std::cout << "open file error, nullptr!" << std::endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    int ret = fclose(fp);
    if (ret != 0) {
        std::cout << "close fp failed" << std::endl;
    }
    return true;
}

uint64_t MicroSecondsSinceEpoch()
{
    SysMicroSeconds tmp = std::chrono::system_clock::now();
    return tmp.time_since_epoch().count();
}

std::string FormattedTimeStamp()
{
    std::stringstream ss;
    auto microsecondsSinceEpoch = MicroSecondsSinceEpoch();
    auto seconds = static_cast<time_t>(microsecondsSinceEpoch / MICRO_SECS_PER_SECOND);

    ::tm* localTime = ::localtime(&seconds);

    ss << std::setw(4) << (localTime->tm_year + 1900) << "-" // 4: parameters of the stew function, 1900: time base
       << std::setfill(SETFILL_VALUE) << std::setw(SETW_VALUE) << (localTime->tm_mon + 1) << "-"
       << std::setfill(SETFILL_VALUE) << std::setw(SETW_VALUE) << localTime->tm_mday << "_"
       << std::setfill(SETFILL_VALUE) << std::setw(SETW_VALUE) << localTime->tm_hour << "-"
       << std::setfill(SETFILL_VALUE) << std::setw(SETW_VALUE) << localTime->tm_min << "-"
       << std::setfill(SETFILL_VALUE) << std::setw(SETW_VALUE) << localTime->tm_sec << ".";
    auto micros = microsecondsSinceEpoch % MICRO_SECS_PER_SECOND;
    ss << std::setfill(SETFILL_VALUE) << std::setw(6) << micros; // 6: parameters of the stew function
    return ss.str();
}

#ifdef ACE_ENABLE_GL
RenderContext* GetRenderContext()
{
    if (renderContext != nullptr) {
        return renderContext;
    }

    renderContext = RenderContextFactory::GetInstance().CreateEngine();
    renderContext->InitializeEglContext();
    return renderContext;
}
#endif // ACE_ENABLE_GL
} // namespace detail

constexpr int SKSCALAR_X = 0;
constexpr int SKSCALAR_Y = 0;
constexpr int SKSCALAR_W = 1400;
constexpr int SKSCALAR_H = 1200;

static void DrawSurface(
    SkRect surfaceGeometry, uint32_t color, SkRect shapeGeometry, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return;
    }
#ifdef ACE_ENABLE_GL
    rsSurface->SetRenderContext(detail::GetRenderContext());
#endif // ACE_ENABLE_GL
    auto framePtr = rsSurface->RequestFrame(width, height);
    if (framePtr == nullptr) {
        std::cout << __func__ << ": framePtr is nullptr!" << std::endl;
        return;
    }
    auto canvas = framePtr->GetCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20);  // 20: SkScalar Width
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    canvas->drawRect(shapeGeometry, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framPtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framPtr1);
}

static std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

using DisplayId = ScreenId;

class ImageReader {
public:
    ImageReader() {}
    ~ImageReader() noexcept
    {
    }

    bool Init()
    {
        csurface_ = Surface::CreateSurfaceAsConsumer();
        if (csurface_ == nullptr) {
            // log
            return false;
        }

        auto producer = csurface_->GetProducer();
        psurface_ = Surface::CreateSurfaceAsProducer(producer);
        if (psurface_ == nullptr) {
            // log
            return false;
        }

        listener_ = new BufferListener(*this);
        SurfaceError ret = csurface_->RegisterConsumerListener(listener_);
        if (ret != SURFACE_ERROR_OK) {
            // log
            return false;
        }
        return true;
    }

    sptr<Surface> GetSurface() const
    {
        return psurface_;
    }
private:
    class BufferListener : public IBufferConsumerListener {
    public:
        explicit BufferListener(ImageReader &imgReader) : imgReader_(imgReader)
        {
        }
        ~BufferListener() noexcept override = default;
        void OnBufferAvailable() override
        {
            imgReader_.OnVsync();
        }

    private:
        ImageReader &imgReader_;
    };
    friend class BufferListener;

    void DumpBuffer(const sptr<SurfaceBuffer> &buf)
    {
        BufferHandle *bufferHandle =  buf->GetBufferHandle();
        if (bufferHandle == nullptr) {
            printf("bufferHandle nullptr!\n");
            return;
        }

        uint32_t width = bufferHandle->width;
        uint32_t height = bufferHandle->height;
        uint32_t stride = bufferHandle->stride;
        uint8_t *addr = (uint8_t *)buf->GetVirAddr();

        detail::WriteToPngParam param;
        param.width = width;
        param.height = height;
        param.data = addr;
        param.stride = stride;
        param.bitDepth = detail::BIT_DEPTH_VALUE;

        if (access("/data", F_OK) < 0) {
            std::cout << "ImageReader::DumpBuffer(): Can't access data directory!" << std::endl;
            return;
        }

        std::string timeStamp = detail::FormattedTimeStamp();
        std::string dumpFileName = "/data/dumpImg-" + std::to_string(getpid()) + "-" + timeStamp + ".png";

        bool ret = WriteToPng(dumpFileName, param);
        if (ret) {
            std::cout << "ImageReader::Dumpbuffer(): " << dumpFileName << std::endl;
        }
    }

    void OnVsync()
    {
        std::cout << "ImageReader::OnVsync" << std::endl;

        sptr<SurfaceBuffer> cbuffer = nullptr;
        int32_t fence = -1;
        int64_t timestamp = 0;
        Rect damage;
        auto sret = csurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
        UniqueFd fenceFd(fence);
        if (cbuffer == nullptr || sret != OHOS::SURFACE_ERROR_OK) {
            std::cout << "ImageReader::OnVsync: surface buffer is null!" << std::endl;
            return;
        }

        DumpBuffer(cbuffer);

        if (cbuffer != prevBuffer_) {
            if (prevBuffer_ != nullptr) {
                SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
                if (ret != SURFACE_ERROR_OK) {
                    std::cout << "ImageReader::OnVsync: release buffer error! err: " << ret << std::endl;
                    return;
                }
            }

            prevBuffer_ = cbuffer;
        }
    }

    sptr<IBufferConsumerListener> listener_;
    sptr<Surface> csurface_; // cosumer surface
    sptr<Surface> psurface_; // producer surface
    sptr<SurfaceBuffer> prevBuffer_;
};

int main()
{
    ScreenId id = RSInterfaces::GetInstance().GetDefaultScreenId();
    cout << "RS default screen id is " << id << ".\n";

    auto surfaceLauncher = CreateSurface();
    DrawSurface(SkRect::MakeXYWH(SKSCALAR_X, SKSCALAR_Y, SKSCALAR_W, SKSCALAR_H), 0xFFF0FFF0,
        SkRect::MakeXYWH(SKSCALAR_X, SKSCALAR_Y, SKSCALAR_W, SKSCALAR_H), surfaceLauncher);
    RSDisplayNodeConfig config;
    config.screenId = id;
    RSDisplayNode::SharedPtr sourceDisplayNode = RSDisplayNode::Create(config);
    sourceDisplayNode->AddChild(surfaceLauncher, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();

    RSScreenModeInfo modeInfo = RSInterfaces::GetInstance().GetScreenActiveMode(id);
    cout << "height=" << modeInfo.GetScreenHeight() << ", width=" << modeInfo.GetScreenWidth() << endl;
    ImageReader imgReader;
    if (!imgReader.Init()) {
        cout << "ImgReader init failed!" << endl;
    }
    DisplayId virtualDisplayId = RSInterfaces::GetInstance().CreateVirtualScreen("virtualDisplay",
        modeInfo.GetScreenWidth(), modeInfo.GetScreenHeight(), imgReader.GetSurface());
    cout << "VirtualScreenId: " << virtualDisplayId << endl;
    cout << "------------------------------------------------------------------" << endl;
    RSDisplayNodeConfig mirrorConfig {virtualDisplayId, true, sourceDisplayNode->GetId()};
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(mirrorConfig);
    sleep(1);
    
    int frameCnt = 5; // test 5 frames.
    for (int i = 0; i < frameCnt; ++i) {
        DrawSurface(SkRect::MakeXYWH(SKSCALAR_X, SKSCALAR_Y, SKSCALAR_W, SKSCALAR_H), 0xFFF0FFF0,
            SkRect::MakeXYWH(SKSCALAR_X, SKSCALAR_Y, SKSCALAR_W, SKSCALAR_H), surfaceLauncher);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        sleep(detail::SLEEP_TIME);
    }

    sourceDisplayNode->ClearChildren();
    sourceDisplayNode->RemoveFromTree();

    return 0;
}
