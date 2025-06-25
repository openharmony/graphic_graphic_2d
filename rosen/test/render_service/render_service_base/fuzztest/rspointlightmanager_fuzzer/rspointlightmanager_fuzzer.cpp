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

#include "rspointlightmanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "property/rs_point_light_manager.h"

namespace OHOS {
namespace Rosen {

RSPointLightManager* instance = RSPointLightManager::Instance();

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
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
bool DoRegisterLightSource(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    instance->RegisterLightSource(node);
    return true;
}
bool DoRegisterIlluminated(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    instance->RegisterIlluminated(node);
    return true;
}

bool DoUnRegisterLightSource(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    instance->UnRegisterLightSource(node);
    return true;
}
bool DoUnRegisterIlluminated(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    instance->UnRegisterIlluminated(node);
    return true;
}
bool DoAddDirtyLightSource(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    std::weak_ptr<RSRenderNode> renderNode = node;
    instance->AddDirtyLightSource(renderNode);
    return true;
}

bool DoAddDirtyIlluminated(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id = GetData<uint64_t>();
    auto node = std::make_shared<RSRenderNode>(id);
    std::weak_ptr<RSRenderNode> renderNode = node;
    instance->AddDirtyIlluminated(renderNode);
    return true;
}
bool DoPrepareLight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    instance->GetScreenRotation();
    instance->PrepareLight();
    instance->ClearDirtyList();

    uint64_t id = GetData<uint64_t>();
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> nodeMap;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyList;
    auto node = std::make_shared<RSRenderNode>(id);
    std::weak_ptr<RSRenderNode> renderNode = node;

    nodeMap.emplace(id, renderNode);
    dirtyList.push_back(renderNode);
    bool isLightSourceDirty = GetData<bool>();
    instance->PrepareLight(nodeMap, dirtyList, isLightSourceDirty);
    return true;
}

bool DoSetScreenRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t value = GetData<uint32_t>();
    ScreenRotation screenRotation = (ScreenRotation)value;
    instance->SetScreenRotation(screenRotation);

    RSLightSource lightSourcePtr;
    RectI illuminatedAbsRect;
    instance->CalculateLightPosForIlluminated(lightSourcePtr, illuminatedAbsRect);
    return true;
}

bool DoCheckIlluminated(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    auto lightSourcePtr = std::make_shared<RSRenderNode>(id);
    auto illuminatedGeoPtr = std::make_shared<RSRenderNode>(id);
    instance->CheckIlluminated(lightSourcePtr, illuminatedGeoPtr);

    auto properties = illuminatedGeoPtr->GetRenderProperties();
    RSProperties* parent = &properties;
    std::optional<Drawing::Point> offset;
    properties.sandbox_ = std::make_unique<Sandbox>();
    properties.SetFramePositionY(1.0f);
    properties.lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.lightSourcePtr_->intensity_ = 1.f;
    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::BLOOM_BORDER;
    properties.UpdateGeometry(parent, true, offset);
    instance->CheckIlluminated(lightSourcePtr, illuminatedGeoPtr);

    Vector4f lightPosition = {1.f, 1.f, 1.f, 1.f};
    lightSourcePtr->GetRenderProperties().SetLightPosition(parent, true, offset);
    instance->CheckIlluminated(lightSourcePtr, illuminatedGeoPtr);
    
    uint32_t rotation = GetData<uint32_t>();
    ScreenRotation screenRotation = (ScreenRotation)value;
    instance->SetScreenRotation(rotation);
    instance->CheckIlluminated(lightSourcePtr, illuminatedGeoPtr);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRegisterLightSource(data, size);             // RegisterLightSource
    OHOS::Rosen::DoRegisterIlluminated(data, size);             // RegisterIlluminated
    OHOS::Rosen::DoUnRegisterLightSource(data, size);           // UnRegisterLightSource
    OHOS::Rosen::DoUnRegisterIlluminated(data, size);           // UnRegisterIlluminated
    OHOS::Rosen::DoAddDirtyLightSource(data, size);             // AddDirtyLightSource
    OHOS::Rosen::DoAddDirtyIlluminated(data, size);             // AddDirtyIlluminated
    OHOS::Rosen::DoPrepareLight(data, size);                    // PrepareLight
    OHOS::Rosen::DoSetScreenRotation(data, size);               // SetScreenRotation
    OHOS::Rosen::DoCheckIlluminated(data, size);                // CheckIlluminated
    return 0;
}
