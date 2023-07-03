/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rsrenderserviceconnectionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#include "transaction/rs_render_service_connection_stub.h"
#include "message_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
constexpr size_t MAX_DATA = 41;
const std::u16string RENDERSERVICECONNECTION_INTERFACE_TOKEN = u"ohos.rosen.RenderServiceConnection";

class RSRenderServiceConnectionStubFuzzTest : public RSRenderServiceConnectionStub {
public:
    RSRenderServiceConnectionStubFuzzTest() = default;
    virtual ~RSRenderServiceConnectionStubFuzzTest() = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override {}

    bool GetUniRenderEnabled() override
    {
        return 0;
    }

    bool CreateNode(const RSSurfaceRenderNodeConfig& config) override
    {
        return 0;
    }
    
    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override
    {
        return nullptr;
    }

    sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name) override
    {
        return nullptr;
    }

    int32_t SetFocusAppInfo(int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName,
        uint64_t focusNodeId) override
    {
        return 0;
    }

    ScreenId GetDefaultScreenId() override
    {
        return 0;
    }

    std::vector<ScreenId> GetAllScreenIds() override
    {
        return {};
    }

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0) override
    {
        return 0;
    }

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override
    {
        return 0;
    }

    void RemoveVirtualScreen(ScreenId id) override {}

    int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override
    {
        return 0;
    }

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override {}

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override
    {
        return 0;
    }

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override {}

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback, float scaleX, float scaleY) override {}

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override {}

    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) override
    {
        RSVirtualScreenResolution rsVirtualScreenResolution;
        return rsVirtualScreenResolution;
    }

    RSScreenModeInfo GetScreenActiveMode(ScreenId id) override
    {
        RSScreenModeInfo rsScreenModeInfo;
        return rsScreenModeInfo;
    }

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) override
    {
        return {};
    }

    RSScreenCapability GetScreenCapability(ScreenId id) override
    {
        RSScreenCapability rsScreenCapability;
        return rsScreenCapability;
    }

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) override
    {
        return ScreenPowerStatus::POWER_STATUS_ON;
    }

    RSScreenData GetScreenData(ScreenId id) override
    {
        RSScreenData rsScreenData;
        return rsScreenData;
    }

    MemoryGraphic GetMemoryGraphic(int pid) override
    {
        MemoryGraphic memoryGraphic;
        return memoryGraphic;
    }

    std::vector<MemoryGraphic> GetMemoryGraphics() override
    {
        return {};
    }

    int32_t GetScreenBacklight(ScreenId id) override
    {
        return 0;
    }

    void SetScreenBacklight(ScreenId id, uint32_t level) override {}

    void RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) override {}

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) override
    {
        return 0;
    }

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) override
    {
        return 0;
    }

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) override
    {
        return 0;
    }

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override
    {
        return 0;
    }

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override
    {
        return 0;
    }

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) override
    {
        return 0;
    }

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) override
    {
        return 0;
    }

    int32_t GetScreenType(ScreenId id, RSScreenType& screenType) override
    {
        return 0;
    }

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) override
    {
        return 0;
    }

    int32_t RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override
    {
        return 0;
    }

    void SetAppWindowNum(uint32_t num) override {}

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) override {}

    void ReportJankStats() override {}

    bool GetBitmap(NodeId id, SkBitmap& bitmap) override
    {
        return 0;
    }

    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override {}

    std::vector<uint32_t> GetScreenSupportedRefreshRates(ScreenId id) override
    {
        std::vector<uint32_t> screenSupportedRefreshRates;
        return screenSupportedRefreshRates;
    }

    uint32_t GetScreenCurrentRefreshRate(ScreenId id) override
    {
        return 0;
    }

    void SetRefreshRateMode(int32_t refreshRateMode) override {}

    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate) override {}
};

namespace {
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (data_ == nullptr || objectSize > size_ - pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    pos += objectSize;
    return object;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    
    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    uint32_t code = GetData<uint32_t>() % MAX_DATA;
    MessageParcel datas;
    datas.WriteInterfaceToken(RENDERSERVICECONNECTION_INTERFACE_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<RSRenderServiceConnectionStub> rsRenderServiceConnectionStub =
        std::make_shared<RSRenderServiceConnectionStubFuzzTest>();
    rsRenderServiceConnectionStub->OnRemoteRequest(code, datas, reply, option);
    return true;
}
} // ROSEN
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}