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

#include "pixelmapfromsurfacegl_fuzzer.h"

#include <array>
#include <limits>
#include <memory>
#include <mutex>
#include <securec.h>

#include "core/pipeline/render_thread/rs_render_engine.h"
#include "iconsumer_surface.h"
#include "pixel_map_from_surface.h"

#include "common/rs_background_thread.h"

namespace OHOS {
namespace {
using namespace Rosen;

constexpr int32_t MAX_BUFFER_SIZE = 64;
constexpr int32_t STRIDE_ALIGNMENT = 8;
constexpr uint64_t BUFFER_USAGE = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;

class BufferConsumerTestListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override {}
};

class FuzzData {
public:
    FuzzData(const uint8_t* data, size_t size) : data_(data), size_(size) {}

    template<class T>
    T Read()
    {
        T value {};
        if (data_ == nullptr || pos_ > size_ || sizeof(T) > size_ - pos_) {
            return value;
        }
        if (memcpy_s(&value, sizeof(T), data_ + pos_, sizeof(T)) != EOK) {
            return {};
        }
        pos_ += sizeof(T);
        return value;
    }

    size_t Select(size_t limit)
    {
        return limit == 0 ? 0 : Read<uint8_t>() % limit;
    }

private:
    const uint8_t* data_ = nullptr;
    size_t size_ = 0;
    size_t pos_ = 0;
};

struct SurfaceFixture {
    sptr<IConsumerSurface> consumer;
    sptr<Surface> producer;
    sptr<SurfaceBuffer> buffer;
    int32_t width = 0;
    int32_t height = 0;
};

void EnsureGpuContext()
{
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    static std::once_flag contextFlag;
    static std::shared_ptr<RSRenderEngine> renderEngine;
    std::call_once(contextFlag, []() {
        renderEngine = std::make_shared<RSRenderEngine>();
        renderEngine->Init();
        const auto& context = renderEngine->GetRenderContext();
        if (context == nullptr) {
            return;
        }
        RSBackgroundThread::Instance().InitRenderContext(context);
        RSBackgroundThread::Instance().PostSyncTask([]() {});
    });
#endif
}

GraphicPixelFormat SelectPixelFormat(FuzzData& fuzzData)
{
    constexpr std::array<GraphicPixelFormat, 5> PIXEL_FORMATS = {
        GRAPHIC_PIXEL_FMT_RGBA_8888,
        GRAPHIC_PIXEL_FMT_BGRA_8888,
        GRAPHIC_PIXEL_FMT_YCBCR_P010,
        GRAPHIC_PIXEL_FMT_YCRCB_P010,
        GRAPHIC_PIXEL_FMT_RGBA_1010102,
    };
    return PIXEL_FORMATS[fuzzData.Select(PIXEL_FORMATS.size())];
}

bool PrepareSurfaceBuffer(FuzzData& fuzzData, SurfaceFixture& fixture)
{
    fixture.width = static_cast<int32_t>(fuzzData.Select(MAX_BUFFER_SIZE)) + 1;
    fixture.height = static_cast<int32_t>(fuzzData.Select(MAX_BUFFER_SIZE)) + 1;
    fixture.consumer = IConsumerSurface::Create();
    if (fixture.consumer == nullptr) {
        return false;
    }
    sptr<IBufferConsumerListener> listener = new BufferConsumerTestListener();
    (void)fixture.consumer->RegisterConsumerListener(listener);
    sptr<IBufferProducer> bufferProducer = fixture.consumer->GetProducer();
    if (bufferProducer == nullptr) {
        return false;
    }
    fixture.producer = Surface::CreateSurfaceAsProducer(bufferProducer);
    if (fixture.producer == nullptr) {
        return false;
    }
    BufferRequestConfig requestConfig = {
        .width = fixture.width,
        .height = fixture.height,
        .strideAlignment = STRIDE_ALIGNMENT,
        .format = SelectPixelFormat(fuzzData),
        .usage = BUFFER_USAGE,
        .timeout = 0,
    };
    int releaseFence = -1;
    if (fixture.producer->RequestBuffer(fixture.buffer, releaseFence, requestConfig) != GSERROR_OK ||
        fixture.buffer == nullptr) {
        return false;
    }
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = fixture.width,
            .h = fixture.height,
        },
    };
    return fixture.producer->FlushBuffer(fixture.buffer, releaseFence, flushConfig) == GSERROR_OK;
}

Media::Rect MakeValidRect(FuzzData& fuzzData, int32_t width, int32_t height)
{
    int32_t rectWidth = static_cast<int32_t>(fuzzData.Select(width)) + 1;
    int32_t rectHeight = static_cast<int32_t>(fuzzData.Select(height)) + 1;
    int32_t left = static_cast<int32_t>(fuzzData.Select(width - rectWidth + 1));
    int32_t top = static_cast<int32_t>(fuzzData.Select(height - rectHeight + 1));
    return { left, top, rectWidth, rectHeight };
}

void FuzzValidation(FuzzData& fuzzData)
{
    const Media::Rect validRect = { 0, 0, 1, 1 };
    const std::array<Media::Rect, 6> invalidRects = {
        Media::Rect { -1, 0, 1, 1 },
        Media::Rect { 0, -1, 1, 1 },
        Media::Rect { 0, 0, 0, 1 },
        Media::Rect { 0, 0, 1, 0 },
        Media::Rect { std::numeric_limits<int32_t>::max(), 0, 1, 1 },
        Media::Rect { 0, std::numeric_limits<int32_t>::max(), 1, 1 },
    };
    (void)CreatePixelMapFromSurface(nullptr, validRect, fuzzData.Select(2) != 0);
    (void)CreatePixelMapFromSurfaceBuffer(nullptr, validRect);
    sptr<SurfaceBuffer> emptyBuffer = SurfaceBuffer::Create().GetRefPtr();
    const auto& invalidRect = invalidRects[fuzzData.Select(invalidRects.size())];
    (void)CreatePixelMapFromSurfaceBuffer(emptyBuffer, invalidRect);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create();
    sptr<IBufferProducer> bufferProducer = consumer != nullptr ? consumer->GetProducer() : nullptr;
    if (bufferProducer != nullptr) {
        sptr<Surface> producer = Surface::CreateSurfaceAsProducer(bufferProducer);
        (void)CreatePixelMapFromSurface(producer, invalidRect, fuzzData.Select(2) != 0);
        (void)CreatePixelMapFromSurface(producer, validRect, fuzzData.Select(2) != 0);
    }
}

void FuzzBufferedSurface(FuzzData& fuzzData)
{
    EnsureGpuContext();
    SurfaceFixture fixture;
    if (!PrepareSurfaceBuffer(fuzzData, fixture)) {
        return;
    }
    Media::Rect validRect = MakeValidRect(fuzzData, fixture.width, fixture.height);
    Media::Rect outOfBounds = {
        fixture.width - 1,
        fixture.height - 1,
        fixture.width,
        fixture.height,
    };
    (void)CreatePixelMapFromSurfaceBuffer(fixture.buffer, outOfBounds);
    switch (fuzzData.Select(3)) {
        case 0:
            (void)CreatePixelMapFromSurfaceBuffer(fixture.buffer, validRect);
            break;
        case 1:
            (void)CreatePixelMapFromSurface(fixture.producer, validRect, false);
            break;
        default:
            (void)fixture.producer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
            (void)CreatePixelMapFromSurface(fixture.producer, validRect, true);
            break;
    }
}
} // namespace
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    OHOS::FuzzData fuzzData(data, size);
    if (fuzzData.Select(2) == 0) {
        OHOS::FuzzValidation(fuzzData);
    } else {
        OHOS::FuzzBufferedSurface(fuzzData);
    }
    return 0;
}
