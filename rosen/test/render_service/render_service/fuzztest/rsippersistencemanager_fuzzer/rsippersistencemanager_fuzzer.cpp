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
#include "rsippersistencemanager_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <message_parcel.h>

#include "rs_ipc_persistence_manager.h"
#include "transfers/rs_on_hwc_event_transfer.h"
#include "transfers/rs_set_behind_window_filter_enabled_transfer.h"
#include "transfers/rs_set_show_refresh_rate_enabled_transfer.h"
#include "transfers/rs_set_watermark_transfer.h"
#include "transfers/rs_self_drawing_node_rect_change_callback_transfer.h"
#include "transfers/rs_show_watermark_transfer.h"

namespace OHOS::Rosen {
namespace {
// ODR-use each transfer's registrar so factories self-register at static init; otherwise
// IsValidTypeId would return false for every typeId and the replay path would skip deserialization.
// (IsValidTypeId was removed; CreateTransferByTypeId now rejects unregistered typeIds directly.)
void ForceFactoryRegistration()
{
    [[maybe_unused]] auto* r1 = &TransferRegistrationChecker<SetShowRefreshRateEnabledTransfer>::registrar;
    [[maybe_unused]] auto* r2 = &TransferRegistrationChecker<SetBehindWindowFilterEnabledTransfer>::registrar;
    [[maybe_unused]] auto* r3 = &TransferRegistrationChecker<OnHwcEventTransfer>::registrar;
    [[maybe_unused]] auto* r4 = &TransferRegistrationChecker<SetWatermarkTransfer>::registrar;
    [[maybe_unused]] auto* r5 = &TransferRegistrationChecker<ShowWatermarkTransfer>::registrar;
    [[maybe_unused]] auto* r6 = &TransferRegistrationChecker<SelfDrawingNodeRectChangeCallbackTransfer>::registrar;
    [[maybe_unused]] auto* r7 =
        &TransferRegistrationChecker<UnRegisterSelfDrawingNodeRectChangeCallbackTransfer>::registrar;
}
} // namespace

// F-03: fuzz the persistence map replay path (manager::Unmarshalling).
// Fuzzes typeCount + typeId + per-transfer payloads; maxEntries defaults to uint32_max (replay cap).
// F-05: semantic replay seeds live in corpus/ (typeCount=1+typeId+payload, typeCount=101 over cap,
// duplicate typeId, unregistered typeId).
bool FuzzManagerReplayUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    static bool registered = (ForceFactoryRegistration(), true);
    (void)registered;
    MessageParcel parcel;
    parcel.WriteUnpadBuffer(data, size);
    std::optional<IpcPersistenceMap> result = RSIpcPersistenceManager::Unmarshalling(parcel);
    (void)result;
    return true;
}
} // namespace OHOS::Rosen

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::FuzzManagerReplayUnmarshalling(data, size);
    return 0;
}
