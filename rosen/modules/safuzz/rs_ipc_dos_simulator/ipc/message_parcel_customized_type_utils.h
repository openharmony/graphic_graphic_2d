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

#ifndef SAFUZZ_MESSAGE_PARCEL_CUSTOMIZED_TYPE_UTILS_H
#define SAFUZZ_MESSAGE_PARCEL_CUSTOMIZED_TYPE_UTILS_H

#include <functional>
#include <memory>
#include <vector>

#include "configs/test_case_config.h"
#include "tools/ipc_utils.h"

namespace OHOS {
namespace Rosen {
class MessageParcelCustomizedTypeUtils {
public:
    static std::function<bool(MessageParcel&, const TestCaseParams&)> GetWriteFunctionByTypeName(
        const std::string& typeName);
    static std::function<bool(std::vector<std::shared_ptr<MessageParcel>>&, const TestCaseParams&)> \
        GetWriteToVectorFunctionByTypeName(const std::string& typeName);

private:
    static bool WriteRandomNString(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
#ifdef ROSEN_OHOS
    static bool WriteRandomApplicationAgentSptr(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
#endif
    static bool WriteRandomVSyncIConnectionToken(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteRandomBufferAvailableCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomBufferClearCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomHgmConfigChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomHgmRefreshRateModeChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomHgmRefreshRateUpdateCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    static bool WriteRandomPointerLuminanceChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
#endif
    static bool WriteRandomFrameRateLinkerExpectedFpsUpdateCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomOcclusionChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomSurfaceBufferCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomSurfaceOcclusionChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomUIExtensionCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomScreenChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomSurfaceCaptureCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomTransactionDataCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomSelfDrawingNodeRectChangeCallbackSptr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);

    static bool WriteRandomPixelMapSharedPtr(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteRandomSurfaceSptr(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteRandomDrawingTypefaceSharedPtr(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteRandomDrawingRect(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);
    static bool WriteRandomDrawingDrawCmdListSharedPtr(MessageParcel& messageParcel,
        const TestCaseParams& testCaseParams);
    static bool WriteRandomRSSyncTaskSharedPtr(MessageParcel& messageParcel, const TestCaseParams& testCaseParams);

    static bool WriteRandomRSTransactionDataToVector(std::vector<std::shared_ptr<MessageParcel>>& messageParcels,
        const TestCaseParams& testCaseParams);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_MESSAGE_PARCEL_CUSTOMIZED_TYPE_UTILS_H
