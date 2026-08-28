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
#include "rsservice2renderconnectionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <message_option.h>
#include <message_parcel.h>

#include "core/rs_render_pipeline_agent.h"
#include "rs_iservice_to_render_connection.h"
#include "rs_service_to_render_connection.h"
#include "rs_ipc_persistence_manager.h"
#include "transfers/rs_on_hwc_event_transfer.h"
#include "transfers/rs_set_behind_window_filter_enabled_transfer.h"
#include "transfers/rs_set_show_refresh_rate_enabled_transfer.h"
#include "transfers/rs_set_watermark_transfer.h"
#include "transfers/rs_self_drawing_node_rect_change_callback_transfer.h"
#include "transfers/rs_show_watermark_transfer.h"

namespace OHOS::Rosen {
namespace {
// ODR-use each transfer's registrar so factories self-register at static init; otherwise the
// stub's CreateTransferByTypeId would find no factory and skip the deserialization attack surface.
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

bool FuzzStubSendRenderProcessData(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    static bool registered = (ForceFactoryRegistration(), true);
    (void)registered;
    // The stub has no per-request session state, so a single instance is reused across inputs
    // instead of paying a binder stub construction per exec.
    static sptr<RSServiceToRenderConnection> stub =
        new RSServiceToRenderConnection(sptr<RSRenderPipelineAgent>(nullptr));
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_SYNC);
    parcel.WriteInterfaceToken(RSIServiceToRenderConnection::GetDescriptor());
    parcel.WriteUnpadBuffer(data, size);
    uint32_t code = static_cast<uint32_t>(RSIServiceToRenderConnectionInterfaceCode::SEND_RENDER_PROCESS_DATA);
    stub->OnRemoteRequest(code, parcel, reply, option);
    return true;
}
} // namespace OHOS::Rosen

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::FuzzStubSendRenderProcessData(data, size);
    return 0;
}
