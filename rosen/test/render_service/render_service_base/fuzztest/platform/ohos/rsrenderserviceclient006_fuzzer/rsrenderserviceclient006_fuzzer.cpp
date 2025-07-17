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

#include "rsrenderserviceclient006_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "transaction/rs_render_service_client.h"
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "core/transaction/rs_interfaces.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST = 0;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST = 1;
const uint8_t DO_SET_SCREEN_POWER_STATUS = 2;
const uint8_t DO_GET_BITMAP = 3;
const uint8_t DO_SET_HWCNODE_BOUNDS = 4;
const uint8_t TARGET_SIZE = 5;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr uint8_t SET_SCREEN_POWER_STATUS = 11;


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

class DerivedSyncTask : public RSSyncTask {
public:
    using RSSyncTask::RSSyncTask;
    bool CheckHeader(Parcel& parcel) const override
    {
        return true;
    }
    bool ReadFromParcel(Parcel& parcel) override
    {
        return true;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    void Process(RSContext& context) override {}
};

bool DoAddVirtualScreenBlackList()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListSize = GetData<uint8_t>();
    for (auto i = 0; i < blackListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        blackListVector.push_back(nodeId);
    }
    client->AddVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoRemoveVirtualScreenBlackList()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListSize = GetData<uint8_t>();
    for (auto i = 0; i < blackListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        blackListVector.push_back(nodeId);
    }
    client->RemoveVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoSetScreenPowerStatus()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint8_t>() % SET_SCREEN_POWER_STATUS);
    client->SetScreenPowerStatus(id, status);
    return true;
}

bool DoGetBitmap()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    Drawing::Bitmap bm;
    NodeId id = GetData<uint64_t>();
    client->GetBitmap(id, bm);
    return true;
}

bool DoSetHwcNodeBounds()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int64_t rsNodeId = GetData<int64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();

    client->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
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
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoAddVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoSetScreenPowerStatus();
            break;
        case OHOS::Rosen::DO_GET_BITMAP:
            OHOS::Rosen::DoGetBitmap();
            break;
        case OHOS::Rosen::DO_SET_HWCNODE_BOUNDS:
            OHOS::Rosen::DoSetHwcNodeBounds();
            break;
        default:
            return -1;
    }
    return 0;
}