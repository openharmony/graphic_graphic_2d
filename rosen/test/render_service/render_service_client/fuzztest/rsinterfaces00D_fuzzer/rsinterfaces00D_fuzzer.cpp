/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rsinterfaces00D_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK = 0;
const uint8_t DO_UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK = 1;
const uint8_t TARGET_SIZE = 2;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
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

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoRegisterSurfaceOcclusionChangeCallback()
{
    SurfaceOcclusionChangeCallback callback;
    NodeId id = GetData<NodeId>();
    std::vector<float> partitionPoints;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
}

void DoUnRegisterSurfaceOcclusionChangeCallback()
{
    NodeId id = GetData<NodeId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.UnRegisterSurfaceOcclusionChangeCallback(id);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceOcclusionChangeCallback();
            break;
        case OHOS::Rosen::DO_UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK:
            OHOS::Rosen::DoUnRegisterSurfaceOcclusionChangeCallback();
            break;
        default:
            return -1;
    }
    return 0;
}
