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

#include <memory>
#include <securec.h>
#include <vector>

#include "rssurfaceohos_fuzzer.h"

#include "iconsumer_surface.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_raster.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#if ACE_ENABLE_GL
#include "render_context/render_context.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos = 0;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSSurfaceOhosFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto rsSurfaceFrameOhosRaster = RSSurfaceFrameOhosRaster(GetData<int32_t>(), GetData<int32_t>());
#if ACE_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        RenderContext renderContext;
        rsSurfaceFrameOhosRaster.SetRenderContext(&renderContext);
    }
#endif
    (void)rsSurfaceFrameOhosRaster.GetBufferAge();
    rsSurfaceFrameOhosRaster.SetReleaseFence(GetData<int32_t>());
    (void)rsSurfaceFrameOhosRaster.GetReleaseFence();
    rsSurfaceFrameOhosRaster.SetDamageRegion(GetData<int32_t>(),
        GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI r1 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI r2 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI r3 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    RectI r4 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
    std::vector<RectI> rects;
    rects.emplace_back(r1);
    rects.emplace_back(r2);
    rects.emplace_back(r3);
    rects.emplace_back(r4);
    rsSurfaceFrameOhosRaster.SetDamageRegion(rects);

    auto consumer_ = IConsumerSurface::Create("DisplayNode");
    auto producer = consumer_->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    auto surface_ = std::make_shared<RSSurfaceOhosRaster>(surface);
    surface_->SetSurfaceBufferUsage(GetData<uint64_t>());
    (void)surface_->IsValid();
    (void)surface_->RequestFrame(GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint64_t>());
    surface_->ClearBuffer();
    surface_->ResetBufferAge();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSSurfaceOhosFuzzTest(data, size);
    return 0;
}