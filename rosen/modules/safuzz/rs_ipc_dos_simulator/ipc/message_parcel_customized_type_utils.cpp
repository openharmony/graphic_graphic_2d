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

#include "ipc/message_parcel_customized_type_utils.h"

#include <unordered_map>

#include "common/safuzz_log.h"
#include "customized/random_animation.h"
#include "customized/random_draw_cmd_list.h"
#include "customized/random_pixel_map.h"
#include "customized/random_surface.h"
#include "customized/random_typeface.h"
#include "ipc/rs_render_service_connection_proxy_utils.h"
#include "random/random_data.h"
#ifdef ROSEN_OHOS
#include "transaction/rs_application_agent_impl.h"
#endif
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client_utils.h"
#include "transaction/rs_transaction_data_utils.h"
#include "vsync_iconnection_token.h"

namespace OHOS {
namespace Rosen {
namespace {
#define DECLARE_WRITE_RANDOM(type) { "Write" #type, MessageParcelCustomizedTypeUtils::WriteRandom##type }

const std::unordered_map<std::string, std::function<bool(MessageParcel&, const TestCaseParams&)>>
    TYPE_NAME_TO_WRITE_FUNCTION_MAPPING = {
    DECLARE_WRITE_RANDOM(NString),
#ifdef ROSEN_OHOS
    DECLARE_WRITE_RANDOM(ApplicationAgentSptr),
#endif
    DECLARE_WRITE_RANDOM(VSyncIConnectionToken),

    // callbacks
    DECLARE_WRITE_RANDOM(BufferAvailableCallbackSptr),
    DECLARE_WRITE_RANDOM(BufferClearCallbackSptr),
    DECLARE_WRITE_RANDOM(HgmConfigChangeCallbackSptr),
    DECLARE_WRITE_RANDOM(HgmRefreshRateModeChangeCallbackSptr),
    DECLARE_WRITE_RANDOM(HgmRefreshRateUpdateCallbackSptr),
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    DECLARE_WRITE_RANDOM(PointerLuminanceChangeCallbackSptr),
#endif
    DECLARE_WRITE_RANDOM(FrameRateLinkerExpectedFpsUpdateCallbackSptr),
    DECLARE_WRITE_RANDOM(OcclusionChangeCallbackSptr),
    DECLARE_WRITE_RANDOM(SurfaceBufferCallbackSptr),
    DECLARE_WRITE_RANDOM(SurfaceOcclusionChangeCallbackSptr),
    DECLARE_WRITE_RANDOM(UIExtensionCallbackSptr),
    DECLARE_WRITE_RANDOM(ScreenChangeCallbackSptr),
    DECLARE_WRITE_RANDOM(SurfaceCaptureCallbackSptr),
    DECLARE_WRITE_RANDOM(TransactionDataCallbackSptr),
    DECLARE_WRITE_RANDOM(SelfDrawingNodeRectChangeCallbackSptr),

    DECLARE_WRITE_RANDOM(PixelMapSharedPtr),
    DECLARE_WRITE_RANDOM(SurfaceSptr),
    DECLARE_WRITE_RANDOM(DrawingTypefaceSharedPtr),
    DECLARE_WRITE_RANDOM(DrawingRect),
    DECLARE_WRITE_RANDOM(DrawingDrawCmdListSharedPtr),
    DECLARE_WRITE_RANDOM(RSSyncTaskSharedPtr),
};

#define DECLARE_WRITE_RANDOM_TO_VECTOR(type) \
    { "Write" #type, MessageParcelCustomizedTypeUtils::WriteRandom##type##ToVector }

const std::unordered_map<std::string, std::function<bool(std::vector<std::shared_ptr<MessageParcel>>&,
    const TestCaseParams&)>> TYPE_NAME_TO_WRITE_VECTOR_FUNCTION_MAPPING = {
    DECLARE_WRITE_RANDOM_TO_VECTOR(RSTransactionData),
};
}

std::function<bool(MessageParcel&, const TestCaseParams&)> MessageParcelCustomizedTypeUtils::GetWriteFunctionByTypeName(
    const std::string& typeName)
{
    auto it = TYPE_NAME_TO_WRITE_FUNCTION_MAPPING.find(typeName);
    if (it == TYPE_NAME_TO_WRITE_FUNCTION_MAPPING.end()) {
        return nullptr;
    }
    return it->second;
}

std::function<bool(std::vector<std::shared_ptr<MessageParcel>>&, const TestCaseParams&)>
    MessageParcelCustomizedTypeUtils::GetWriteToVectorFunctionByTypeName(const std::string& typeName)
{
    auto it = TYPE_NAME_TO_WRITE_VECTOR_FUNCTION_MAPPING.find(typeName);
    if (it == TYPE_NAME_TO_WRITE_VECTOR_FUNCTION_MAPPING.end()) {
        return nullptr;
    }
    return it->second;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomNString(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    std::vector<std::string> dataVec = RandomDataBasicType::GetRandomStringVector();
    for (const std::string& data : dataVec) {
        if (!messageParcel.WriteString(data)) {
            SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomNString WriteString failed");
            return false;
        }
    }
    return true;
}

#ifdef ROSEN_OHOS
bool MessageParcelCustomizedTypeUtils::WriteRandomApplicationAgentSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    sptr<IApplicationAgent> obj = RSApplicationAgentImpl::Instance();
    if (obj == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomApplicationAgentSptr nullptr");
        return false;
    }
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomApplicationAgentSptr WriteRemoteObject failed");
        return false;
    }
    return true;
}
#endif

bool MessageParcelCustomizedTypeUtils::WriteRandomVSyncIConnectionToken(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    if (!messageParcel.WriteRemoteObject(token->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomVSyncIConnectionToken WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomBufferAvailableCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    BufferAvailableCallback callback = []() {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomBufferAvailableCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIBufferAvailableCallback> obj = new CustomBufferAvailableCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomBufferAvailableCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomBufferClearCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    BufferClearCallback callback = []() {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomBufferClearCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIBufferClearCallback> obj = new CustomBufferClearCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomBufferClearCallbackSptr WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomHgmConfigChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    HgmConfigChangeCallback callback = [](std::shared_ptr<RSHgmConfigData>) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomHgmConfigChangeCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIHgmConfigChangeCallback> obj = new CustomHgmConfigChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomHgmConfigChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateModeChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    HgmRefreshRateModeChangeCallback callback = [](int32_t) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateModeChangeCallbackSptr "
            "enter dead lock");
        while (true) {}
    };
    sptr<RSIHgmConfigChangeCallback> obj = new CustomHgmRefreshRateModeChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateModeChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateUpdateCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    HgmRefreshRateUpdateCallback callback = [](int32_t) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateUpdateCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIHgmConfigChangeCallback> obj = new CustomHgmRefreshRateUpdateCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomHgmRefreshRateUpdateCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
bool MessageParcelCustomizedTypeUtils::WriteRandomPointerLuminanceChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    PointerLuminanceChangeCallback callback = [](int32_t) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomPointerLuminanceChangeCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIPointerLuminanceChangeCallback> obj = new CustomPointerLuminanceChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomPointerLuminanceChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}
#endif

bool MessageParcelCustomizedTypeUtils::WriteRandomFrameRateLinkerExpectedFpsUpdateCallbackSptr(
    MessageParcel& messageParcel, const TestCaseParams& /* testCaseParams */)
{
    FrameRateLinkerExpectedFpsUpdateCallback callback = [](int32_t, int32_t) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomFrameRateLinkerExpectedFpsUpdateCallbackSptr "
            "enter dead lock");
        while (true) {}
    };
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> obj =
        new CustomFrameRateLinkerExpectedFpsUpdateCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomFrameRateLinkerExpectedFpsUpdateCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomOcclusionChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    OcclusionChangeCallback callback = [](std::shared_ptr<RSOcclusionData>) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomOcclusionChangeCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIOcclusionChangeCallback> obj = new CustomOcclusionChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomOcclusionChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomSurfaceBufferCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    RSRenderServiceClient* rsClient = RSInterfaces::GetInstance().renderServiceClient_.get();
    if (rsClient == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceBufferCallbackSptr "
            "rsClient is nullptr");
        return false;
    }
    sptr<RSISurfaceBufferCallback> obj = new SurfaceBufferCallbackDirector(rsClient);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceBufferCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomSurfaceOcclusionChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    SurfaceOcclusionChangeCallback callback = [](float) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceOcclusionChangeCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSISurfaceOcclusionChangeCallback> obj = new CustomSurfaceOcclusionChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceOcclusionChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomUIExtensionCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    UIExtensionCallback callback = [](std::shared_ptr<RSUIExtensionData>, uint64_t) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomUIExtensionCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIUIExtensionCallback> obj = new CustomUIExtensionCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomUIExtensionCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomScreenChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    ScreenChangeCallback callback = [](ScreenId, ScreenEvent, ScreenChangeReason) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomScreenChangeCallbackSptr enter dead lock");
        while (true) {}
    };
    sptr<RSIScreenChangeCallback> obj = new CustomScreenChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomScreenChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomSurfaceCaptureCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    RSRenderServiceClient* rsClient = RSInterfaces::GetInstance().renderServiceClient_.get();
    if (rsClient == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceCaptureCallbackSptr "
            "rsClient is nullptr");
        return false;
    }
    sptr<RSISurfaceCaptureCallback> obj = new SurfaceCaptureCallbackDirector(rsClient);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceCaptureCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomTransactionDataCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    RSRenderServiceClient* rsClient = RSInterfaces::GetInstance().renderServiceClient_.get();
    if (rsClient == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomTransactionDataCallbackSptr "
            "rsClient is nullptr");
        return false;
    }
    sptr<RSITransactionDataCallback> obj = new TransactionDataCallbackDirector(std::shared_ptr<RSRenderServiceClient>(rsClient));
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomTransactionDataCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomSelfDrawingNodeRectChangeCallbackSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    SelfDrawingNodeRectChangeCallback callback = [](std::shared_ptr<RSSelfDrawingNodeRectData>) {
        SAFUZZ_LOGW("MessageParcelCustomizedTypeUtils::WriteRandomSelfDrawingNodeRectChangeCallbackSptr "
            "enter dead lock");
        while (true) {}
    };
    sptr<RSISelfDrawingNodeRectChangeCallback> obj = new CustomSelfDrawingNodeRectChangeCallback(callback);
    if (!messageParcel.WriteRemoteObject(obj->AsObject())) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSelfDrawingNodeRectChangeCallbackSptr "
            "WriteRemoteObject failed");
        return false;
    }
    return true;
}

bool MessageParcelCustomizedTypeUtils::WriteRandomPixelMapSharedPtr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    std::shared_ptr<Media::PixelMap> pixelmap = RandomPixelMap::GetRandomPixelMap();
    if (pixelmap == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomPixelMapSharedPtr pixelmap is nullptr");
        return false;
    }
    SAFUZZ_LOGI("MessageParcelCustomizedTypeUtils::WriteRandomPixelMapSharedPtr pixelmap w %" PRId32 " h %" PRId32,
        pixelmap->GetWidth(), pixelmap->GetHeight());
    return messageParcel.WriteParcelable(pixelmap.get());
}

bool MessageParcelCustomizedTypeUtils::WriteRandomSurfaceSptr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    sptr<Surface> surface = RandomSurface::GetRandomSurface();
    if (surface == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceSptr surface is nullptr");
        return false;
    }
    auto producer = surface->GetProducer();
    if (producer == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomSurfaceSptr producer is nullptr");
        return false;
    }
    return messageParcel.WriteRemoteObject(producer->AsObject());
}

bool MessageParcelCustomizedTypeUtils::WriteRandomDrawingTypefaceSharedPtr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    std::shared_ptr<Drawing::Typeface> typeface = RandomTypeface::GetRandomTypeface();
    if (typeface == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomDrawingTypefaceSharedPtr typeface is nullptr");
        return false;
    }
    return RSMarshallingHelper::Marshalling(messageParcel, typeface);
}

bool MessageParcelCustomizedTypeUtils::WriteRandomDrawingRect(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    Drawing::Rect rect = RandomData::GetRandomDrawingRect();
    return RSMarshallingHelper::Marshalling(messageParcel, rect);
}

bool MessageParcelCustomizedTypeUtils::WriteRandomDrawingDrawCmdListSharedPtr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = RandomDrawCmdList::GetRandomDrawCmdList();
    if (drawCmdList == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomDrawingDrawCmdListSharedPtr drawCmdList is nullptr");
        return false;
    }
    return RSMarshallingHelper::Marshalling(messageParcel, drawCmdList);
}

bool MessageParcelCustomizedTypeUtils::WriteRandomRSSyncTaskSharedPtr(MessageParcel& messageParcel,
    const TestCaseParams& /* testCaseParams */)
{
    std::shared_ptr<RSSyncTask> syncTask = RandomAnimation::GetRandomRSSyncTask();
    if (syncTask == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomRSSyncTaskSharedPtr syncTask is nullptr");
        return false;
    }
    return syncTask->Marshalling(messageParcel);
}

bool MessageParcelCustomizedTypeUtils::WriteRandomRSTransactionDataToVector(
    std::vector<std::shared_ptr<MessageParcel>>& messageParcels, const TestCaseParams& testCaseParams)
{
    std::unique_ptr<RSTransactionData> transactionData = RSTransactionDataUtils::CreateTransactionDataFromCommandList(
        testCaseParams.commandList, testCaseParams.commandListRepeat);
    if (transactionData == nullptr) {
        SAFUZZ_LOGE("MessageParcelCustomizedTypeUtils::WriteRandomRSTransactionDataToVector "
            "transaction data is nullptr");
        return false;
    }
    SAFUZZ_LOGI("MessageParcelCustomizedTypeUtils::WriteRandomRSTransactionDataToVector command count: %lu",
        transactionData->GetCommandCount());
    return RSRenderServiceConnectionProxyVariant::FillParcelVectorWithTransactionData(transactionData, messageParcels);
}
} // namespace Rosen
} // namespace OHOS
