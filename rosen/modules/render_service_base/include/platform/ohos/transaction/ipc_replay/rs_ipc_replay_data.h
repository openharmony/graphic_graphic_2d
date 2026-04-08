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

#ifndef RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DATA_H
#define RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DATA_H

#include <memory>

#include <parcel.h>

#include "platform/ohos/transaction/ipc_replay/rs_ipc_replay_def.h"

namespace OHOS {
namespace Media {
class PixelMap;
} // namespace Media

namespace Rosen {
class SetWatermarkReplayData : public RSIpcReplayDataBase {
public:
    SetWatermarkReplayData() = default;
    SetWatermarkReplayData(pid_t pid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool success)
        : pid_(pid), name_(name), watermark_(watermark), success_(success) {}
    ~SetWatermarkReplayData() noexcept override = default;

    RSIpcReplayType GetType() const override { return RSIpcReplayType::SET_WATERMARK; }
    pid_t GetPid() const override { return pid_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static SetWatermarkReplayData* Unmarshalling(Parcel& parcel);

    pid_t pid_;
    std::string name_;
    std::shared_ptr<Media::PixelMap> watermark_;
    bool success_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_IPC_REPLAY_RS_IPC_REPLAY_DATA_H
