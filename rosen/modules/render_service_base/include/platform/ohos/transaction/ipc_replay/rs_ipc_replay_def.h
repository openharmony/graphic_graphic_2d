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

#ifndef RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DEF_H
#define RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DEF_H

#include <memory>

#include <parcel.h>

namespace OHOS {
namespace Rosen {
enum class RSIpcReplayType : uint32_t {
    SET_WATERMARK,
    DEFAULT = 0xFFFFFFFF,
};

class RSIpcReplayDataBase : public Parcelable {
public:
    RSIpcReplayDataBase() = default;
    ~RSIpcReplayDataBase() noexcept override = default;

    virtual RSIpcReplayType GetType() const = 0;
    virtual pid_t GetPid() const = 0;
};

using IpcReplayTypeToDataMap = std::unordered_map<RSIpcReplayType, std::vector<std::shared_ptr<RSIpcReplayDataBase>>>;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DEF_H
