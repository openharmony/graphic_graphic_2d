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

#include "rssurfacenodecommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "command/rs_surface_node_command.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STRING_LEN = 10;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

bool DoSurfacenodecommand(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSContext context;
    Drawing::Matrix matrix;
    float alpha1 = GetData<float>();
    Drawing::Rect clipRect;
    bool isSecurityLayer = GetData<bool>();
    bool isSkipLayer = GetData<bool>();
    bool hasFingerprint = GetData<bool>();
    GraphicColorGamut colorSpace = GetData<GraphicColorGamut>();
    float width = GetData<float>();
    float height = GetData<float>();
    bool isRefresh = GetData<bool>();
    Vector4f bounds = {GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    bool isTextureExportNode = GetData<bool>();
    uint8_t alpha2 = GetData<uint8_t>();
    bool available = GetData<bool>();
    bool isHidden = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    bool hasContainerWindow = GetData<bool>();
    float density = GetData<float>();
    uint64_t screenId = GetData<uint64_t>();
    bool isBootAnimation = GetData<bool>();
    bool flag = GetData<bool>();
    bool isForeground = GetData<bool>();
    SurfaceId surfaceId = GetData<SurfaceId>();
    bool forceUIFirst = GetData<bool>();
    bool hdrPresent = GetData<bool>();
    SurfaceNodeCommandHelper::Create(context, id);
    SurfaceNodeCommandHelper::SetContextMatrix(context, id, matrix);
    SurfaceNodeCommandHelper::SetContextAlpha(context, id, alpha1);
    SurfaceNodeCommandHelper::SetContextClipRegion(context, id, clipRect);
    SurfaceNodeCommandHelper::SetSecurityLayer(context, id, isSecurityLayer);
    SurfaceNodeCommandHelper::SetSkipLayer(context, id, isSkipLayer);
    SurfaceNodeCommandHelper::SetFingerprint(context, id, hasFingerprint);
    SurfaceNodeCommandHelper::SetColorSpace(context, id, colorSpace);
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(context, id, width, height);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, id, isRefresh);
    SurfaceNodeCommandHelper::SetContextBounds(context, id, bounds);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, id, isTextureExportNode);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, id, alpha2);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, id, available);
    SurfaceNodeCommandHelper::MarkUIHidden(context, id, isHidden);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, type);
    SurfaceNodeCommandHelper::SetContainerWindow(context, id, hasContainerWindow, density);
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id);
    SurfaceNodeCommandHelper::AttachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::DetachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id, isBootAnimation);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, id, flag);
    SurfaceNodeCommandHelper::SetForeground(context, id, isForeground);
    SurfaceNodeCommandHelper::SetSurfaceId(context, id, surfaceId);
    SurfaceNodeCommandHelper::SetForceUIFirst(context, id, forceUIFirst);
    SurfaceNodeCommandHelper::SetAncoForceDoDirect(context, id, forceUIFirst);
    SurfaceNodeCommandHelper::SetHDRPresent(context, id, hdrPresent);
    return true;
}

bool DoCreateWithConfig(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSContext context;
    std::string name(STRING_LEN, GetData<char>());
    uint8_t type = GetData<uint8_t>();
    std::string bundleName(STRING_LEN, GetData<char>());
    SurfaceNodeCommandHelper::CreateWithConfig(context, id, name, type, bundleName);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSurfacenodecommand(data, size);
    OHOS::Rosen::DoCreateWithConfig(data, size);
    return 0;
}

