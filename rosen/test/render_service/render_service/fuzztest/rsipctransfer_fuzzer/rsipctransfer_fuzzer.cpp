/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "rsipctransfer_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>
#include <message_parcel.h>

#include "transfers/rs_on_hwc_event_transfer.h"
#include "transfers/rs_set_behind_window_filter_enabled_transfer.h"
#include "transfers/rs_set_show_refresh_rate_enabled_transfer.h"
#include "transfers/rs_set_watermark_transfer.h"
#include "transfers/rs_self_drawing_node_rect_change_callback_transfer.h"
#include "transfers/rs_show_watermark_transfer.h"

namespace OHOS::Rosen {
namespace {
constexpr uint8_t SELECTOR_SET_SHOW_REFRESH_RATE = 0;
constexpr uint8_t SELECTOR_SET_BEHIND_WINDOW_FILTER = 1;
constexpr uint8_t SELECTOR_ON_HWC_EVENT = 2;
constexpr uint8_t SELECTOR_SET_WATERMARK = 3;
constexpr uint8_t SELECTOR_SHOW_WATERMARK = 4;
constexpr uint8_t SELECTOR_SELF_DRAWING_REGISTER = 5;
constexpr uint8_t SELECTOR_SELF_DRAWING_UNREGISTER = 6;
constexpr uint8_t SELECTOR_UNREGISTER_REPLY_PARSE = 7;
constexpr uint8_t SELECTOR_REGISTER_REPLY_PARSE = 8;
constexpr uint8_t SELECTOR_COUNT = 9;
constexpr pid_t FUZZ_CALLBACK_PID = 42;

void FuzzTransferStubUnmarshalling(const uint8_t* data, size_t size, uint8_t selector, uint32_t maxEntries)
{
    if (data == nullptr || size == 0) {
        return;
    }
    MessageParcel parcel;
    parcel.WriteUnpadBuffer(data, size);
    int32_t errCode = 0;
    switch (selector % SELECTOR_COUNT) { // NOLINT(hicpp-multiway-paths-covered)
        case SELECTOR_SET_SHOW_REFRESH_RATE: {
            [[maybe_unused]] std::shared_ptr<SetShowRefreshRateEnabledTransfer> t =
                SetShowRefreshRateEnabledTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_SET_BEHIND_WINDOW_FILTER: {
            [[maybe_unused]] std::shared_ptr<SetBehindWindowFilterEnabledTransfer> t =
                SetBehindWindowFilterEnabledTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_ON_HWC_EVENT: {
            [[maybe_unused]] std::shared_ptr<OnHwcEventTransfer> t =
                OnHwcEventTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_SET_WATERMARK: {
            [[maybe_unused]] std::shared_ptr<SetWatermarkTransfer> t =
                SetWatermarkTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_SHOW_WATERMARK: {
            [[maybe_unused]] std::shared_ptr<ShowWatermarkTransfer> t =
                ShowWatermarkTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_SELF_DRAWING_REGISTER: {
            [[maybe_unused]] std::shared_ptr<SelfDrawingNodeRectChangeCallbackTransfer> t =
                SelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_SELF_DRAWING_UNREGISTER: {
            [[maybe_unused]] std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer> t =
                UnRegisterSelfDrawingNodeRectChangeCallbackTransfer::StubUnmarshalling(parcel, maxEntries, errCode);
            break;
        }
        case SELECTOR_UNREGISTER_REPLY_PARSE: {
            // reply-parse fuzzing: UnRegister reply carries an int32 result
            std::shared_ptr<UnRegisterSelfDrawingNodeRectChangeCallbackInput> input =
                std::make_shared<UnRegisterSelfDrawingNodeRectChangeCallbackInput>(FUZZ_CALLBACK_PID);
            UnRegisterSelfDrawingNodeRectChangeCallbackTransfer transfer(input);
            transfer.ProxyUnmarshalling(parcel);
            break;
        }
        case SELECTOR_REGISTER_REPLY_PARSE: {
            // reply-parse fuzzing: REGISTER reply carries an int32 result
            std::map<pid_t, std::shared_ptr<SelfDrawingNodeRectChangeCallbackInput>> inputs;
            SelfDrawingNodeRectChangeCallbackTransfer transfer(std::move(inputs));
            transfer.ProxyUnmarshalling(parcel);
            break;
        }
        default:
            break;
    }
    (void)errCode;
}
} // namespace
} // namespace OHOS::Rosen

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint8_t selector = fdp.ConsumeIntegral<uint8_t>();
    uint32_t maxEntries = fdp.ConsumeIntegral<uint32_t>();
    std::vector<uint8_t> remaining = fdp.ConsumeRemainingBytes<uint8_t>();
    OHOS::Rosen::FuzzTransferStubUnmarshalling(remaining.data(), remaining.size(), selector, maxEntries);
    return 0;
}
