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

#include "rssurfacenode4_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_SET_SURFACE_BUFFER_OPAQUE = 0;
#ifdef USE_SURFACE_TEXTURE
constexpr uint8_t DO_SET_SURFACE_TEXTURE = 1;
constexpr uint8_t DO_MARK_UI_FRAME_AVAILABLE = 2;
constexpr uint8_t DO_CREATE_SURFACE_EXT = 3;
constexpr uint8_t MAX_CASE_NUM = 4;
#else
constexpr uint8_t MAX_CASE_NUM = 1;
#endif

template<typename T>
T ConsumePOD(FuzzedDataProvider& fdp)
{
    T object {};
    auto bytes = fdp.ConsumeBytes<uint8_t>(sizeof(T));
    if (bytes.size() == sizeof(T)) {
        errno_t ret = memcpy_s(&object, sizeof(T), bytes.data(), sizeof(T));
        if (ret != EOK) {
            return {};
        }
    }
    return object;
}

inline RSSurfaceNodeConfig GetRSSurfaceNodeConfigFromData(FuzzedDataProvider& fdp)
{
    constexpr int len = 10;
    std::string SurfaceNodeName(len, '\0');
    for (int i = 0; i < len - 1; i++) {
        char tmp = static_cast<char>(fdp.ConsumeIntegral<uint8_t>());
        if (tmp == '\0') {
            tmp = '1';
        }
        SurfaceNodeName[i] = tmp;
    }
    bool isTextureExportNode = fdp.ConsumeBool();
    SurfaceId surfaceId = fdp.ConsumeIntegral<uint64_t>();
    bool isSync = fdp.ConsumeBool();
    SurfaceWindowType surfaceWindowType = static_cast<SurfaceWindowType>(fdp.ConsumeIntegral<uint8_t>());
    RSSurfaceNodeConfig config = { SurfaceNodeName, nullptr, isTextureExportNode,
        surfaceId, isSync, surfaceWindowType, nullptr };
    return config;
}
} // namespace

bool DoSetSurfaceBufferOpaque(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool isOpaque = fdp.ConsumeBool();
    surfaceNode->SetSurfaceBufferOpaque(isOpaque);
    return true;
}

#ifdef USE_SURFACE_TEXTURE
bool DoSetSurfaceTexture(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    RSSurfaceExtConfig config1 = ConsumePOD<RSSurfaceExtConfig>(fdp);
    surfaceNode->SetSurfaceTexture(config1);
    return true;
}

bool DoMarkUiFrameAvailable(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    bool available = fdp.ConsumeBool();
    surfaceNode->MarkUiFrameAvailable(available);
    return true;
}

bool DoCreateSurfaceExt(FuzzedDataProvider& fdp)
{
    auto config = GetRSSurfaceNodeConfigFromData(fdp);
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    if (surfaceNode == nullptr) {
        return false;
    }
    RSSurfaceExtConfig config1 = ConsumePOD<RSSurfaceExtConfig>(fdp);
    surfaceNode->CreateSurfaceExt(config1);
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>();
    switch (choice % OHOS::Rosen::MAX_CASE_NUM) {
        case OHOS::Rosen::DO_SET_SURFACE_BUFFER_OPAQUE:
            OHOS::Rosen::DoSetSurfaceBufferOpaque(fdp);
            break;
#ifdef USE_SURFACE_TEXTURE
        case OHOS::Rosen::DO_SET_SURFACE_TEXTURE:
            OHOS::Rosen::DoSetSurfaceTexture(fdp);
            break;
        case OHOS::Rosen::DO_MARK_UI_FRAME_AVAILABLE:
            OHOS::Rosen::DoMarkUiFrameAvailable(fdp);
            break;
        case OHOS::Rosen::DO_CREATE_SURFACE_EXT:
            OHOS::Rosen::DoCreateSurfaceExt(fdp);
            break;
#endif
        default:
            break;
    }
    return 0;
}
