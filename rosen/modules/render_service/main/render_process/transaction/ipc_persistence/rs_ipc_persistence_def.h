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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H

#include <memory>

#include <parcel.h>

namespace OHOS {
namespace Rosen {
enum class RSIpcPersistenceType : uint32_t {
    SET_WATERMARK = 0,
    SHOW_WATERMARK,
    ON_HWC_EVENT,
    SET_BEHIND_WINDOW_FILTER_ENABLED,
    SET_SHOW_REFRESH_RATE_ENABLED,
    REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK,
    DEFAULT = 0xFFFFFFFF,
};

// Indicates the default PID of the IPC that does not need the calling PID.
constexpr pid_t IPC_PERSISTENCE_DEFAULT_PID = -1;

class RSRenderPipelineAgent;

class RSIpcPersistenceDataBase : public Parcelable {
public:
    RSIpcPersistenceDataBase() = default;
    ~RSIpcPersistenceDataBase() noexcept override = default;

    virtual RSIpcPersistenceType GetType() const = 0;
    virtual pid_t GetCallingPid() const { return IPC_PERSISTENCE_DEFAULT_PID; }

    virtual void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) = 0;
};

using IpcPersistenceTypeToDataMap =
    std::unordered_map<RSIpcPersistenceType, std::vector<std::shared_ptr<RSIpcPersistenceDataBase>>>;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DEF_H
