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

#include "rssurfaceframeohosgl_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_gl.h"
#include "render_context/render_context.h"
namespace OHOS {
namespace Rosen {

namespace {
const uint8_t DO_SET_DAMAGEREGION = 0;
const uint8_t DO_GET_BUFFERAGE = 1;
const uint8_t DO_GET_RELEASEFENCE = 2;
const uint8_t DO_SET_RELEASEFENCE = 3;
const uint8_t TARGET_SIZE = 4;

auto g_rsSurfaceFrameOhosGl = std::make_shared<RSSurfaceFrameOhosGl>(1, 1);
std::shared_ptr<RenderContext> g_context;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}

void InitRenderContext()
{
    g_context = std::make_shared<RenderContext>();
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        g_context->InitializeEglContext();
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto drawingContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
        g_context->SetUpGpuContext(drawingContext);
    } else {
        g_context->SetUpGpuContext();
    }
#else
    g_context->SetUpGpuContext();
#endif
#endif // RS_ENABLE_GL || RS_ENABLE_VK
    g_rsSurfaceFrameOhosGl->SetRenderContext(g_context.get());
}

void ReleaseRenderContext()
{
    g_rsSurfaceFrameOhosGl->SetRenderContext(nullptr);
#if defined(RS_ENABLE_VK)
    g_context->AbandonContext();
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        g_context->drGPUContext_->ReleaseResourcesAndAbandonContext();
        g_context->drGPUContext_ = nullptr;
    }
#endif
    g_context = nullptr;
}
} // namespace

void DoSetDamageRegion()
{
    OHOS::Rosen::InitRenderContext();

    int32_t left = GetData<int32_t>();
    int32_t top = GetData<int32_t>();
    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();
    g_rsSurfaceFrameOhosGl->SetDamageRegion(left, top, width1, height1);

    std::vector<RectI> rects;
    g_rsSurfaceFrameOhosGl->SetDamageRegion(rects);

    OHOS::Rosen::ReleaseRenderContext();
}

void DoGetBufferAge()
{
    OHOS::Rosen::InitRenderContext();
    g_rsSurfaceFrameOhosGl->GetBufferAge();
    OHOS::Rosen::ReleaseRenderContext();
}

void DoGetReleaseFence()
{
    OHOS::Rosen::InitRenderContext();
    g_rsSurfaceFrameOhosGl->GetReleaseFence();
    OHOS::Rosen::ReleaseRenderContext();
}

void DoSetReleaseFence()
{
    OHOS::Rosen::InitRenderContext();
    int32_t fence = GetData<int32_t>();
    g_rsSurfaceFrameOhosGl->SetReleaseFence(fence);
    OHOS::Rosen::ReleaseRenderContext();
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_DAMAGEREGION:
            OHOS::Rosen::DoSetDamageRegion(); // SetDamageRegion
            break;
        case OHOS::Rosen::DO_GET_BUFFERAGE:
            OHOS::Rosen::DoGetBufferAge();    // GetBufferAge
            break;
        case OHOS::Rosen::DO_GET_RELEASEFENCE:
            OHOS::Rosen::DoGetReleaseFence(); // GetReleaseFence
            break;
        case OHOS::Rosen::DO_SET_RELEASEFENCE:
            OHOS::Rosen::DoSetReleaseFence(); // SetReleaseFence
            break;
        default:
            return -1;
    }
    return 0;
}
