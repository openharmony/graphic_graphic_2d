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
    Drawing::Rect clipRect;
    Vector4f bounds = {GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    uint64_t screenId = GetData<uint64_t>();
    SurfaceId surfaceId = GetData<SurfaceId>();
    SurfaceNodeCommandHelper::Create(context, id);
    SurfaceNodeCommandHelper::SetContextMatrix(context, id, matrix);
    SurfaceNodeCommandHelper::SetContextAlpha(context, id, GetData<float>());
    SurfaceNodeCommandHelper::SetContextClipRegion(context, id, clipRect);
    SurfaceNodeCommandHelper::SetSecurityLayer(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetSkipLayer(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetSnapshotSkipLayer(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetFingerprint(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetColorSpace(context, id, GetData<GraphicColorGamut>());
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(context, id, GetData<float>(), GetData<float>());
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, id);
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetContextBounds(context, id, bounds);
    SurfaceNodeCommandHelper::SetIsTextureExportNode(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, id, GetData<uint8_t>());
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::MarkUIHidden(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, GetData<uint8_t>());
    SurfaceNodeCommandHelper::SetContainerWindow(context, id, GetData<bool>(), GetData<RRect>());
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id);
    SurfaceNodeCommandHelper::AttachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::DetachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::SetBootAnimation(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetForeground(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetSurfaceId(context, id, surfaceId);
    SurfaceNodeCommandHelper::SetForceUIFirst(context, id, GetData<bool>());
    SurfaceNodeCommandHelper::SetAncoFlags(context, id, GetData<uint32_t>());
    SurfaceNodeCommandHelper::SetHDRPresent(context, id, GetData<bool>());
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
    enum SurfaceWindowType windowType = SurfaceWindowType::DEFAULT_WINDOW;
    SurfaceNodeCommandHelper::CreateWithConfig(context, id, name, type, windowType);
    return true;
}

bool DoSurfacenodecommand002(const uint8_t* data, size_t size)
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
    uint64_t screenId = GetData<uint64_t>();
    bool isEnabled = GetData<bool>();
    uint8_t surfaceNodeType = GetData<uint8_t>();
    bool isHidden = GetData<bool>();

    SurfaceNodeCommandHelper::SetGlobalPositionEnabled(context, id, isEnabled);
    SurfaceNodeCommandHelper::AttachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::DetachToDisplay(context, id, screenId);
    SurfaceNodeCommandHelper::SetAnimationFinished(context, id);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, id, surfaceNodeType);
    SurfaceNodeCommandHelper::MarkUIHidden(context, id, isHidden);
    SurfaceNodeCommandHelper::SetFrameGravityNewVersionEnabled(context, id, isEnabled);
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
    OHOS::Rosen::DoSurfacenodecommand002(data, size);
    return 0;
}

