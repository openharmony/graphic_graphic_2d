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

#ifndef RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DATA_H
#define RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DATA_H

#include "common/rs_self_draw_rect_change_callback_constraint.h"
#include "ipc_callbacks/rs_iself_drawing_node_rect_change_callback.h"
#include "rs_ipc_persistence_def.h"

namespace OHOS {
namespace Media {
class PixelMap;
} // namespace Media

namespace Rosen {
class SetWatermarkPersistenceData : public RSIpcPersistenceDataBase {
public:
    SetWatermarkPersistenceData() = default;
    SetWatermarkPersistenceData(
        pid_t pid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool success)
        : pid_(pid), name_(name), watermark_(watermark), success_(success) {}
    ~SetWatermarkPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::SET_WATERMARK; }
    pid_t GetCallingPid() const override { return pid_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static SetWatermarkPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    pid_t pid_;
    std::string name_;
    std::shared_ptr<Media::PixelMap> watermark_;
    bool success_;
};

class ShowWatermarkPersistenceData : public RSIpcPersistenceDataBase {
public:
    ShowWatermarkPersistenceData() = default;
    ShowWatermarkPersistenceData(const std::shared_ptr<Media::PixelMap>& watermarkImg, bool isShow)
        : watermarkImg_(watermarkImg), isShow_(isShow) {}
    ~ShowWatermarkPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::SHOW_WATERMARK; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static ShowWatermarkPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    std::shared_ptr<Media::PixelMap> watermarkImg_;
    bool isShow_;
};

class OnHwcEventPersistenceData : public RSIpcPersistenceDataBase {
public:
    OnHwcEventPersistenceData() = default;
    OnHwcEventPersistenceData(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
        : deviceId_(deviceId), eventId_(eventId), eventData_(eventData) {}
    ~OnHwcEventPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::ON_HWC_EVENT; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OnHwcEventPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    uint32_t deviceId_;
    uint32_t eventId_;
    std::vector<int32_t> eventData_;
};

class SetBehindWindowFilterEnabledPersistenceData : public RSIpcPersistenceDataBase {
public:
    SetBehindWindowFilterEnabledPersistenceData() = default;
    explicit SetBehindWindowFilterEnabledPersistenceData(bool enabled) : enabled_(enabled) {}
    ~SetBehindWindowFilterEnabledPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::SET_BEHIND_WINDOW_FILTER_ENABLED; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static SetBehindWindowFilterEnabledPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    bool enabled_;
};

class SetShowRefreshRateEnabledPersistenceData : public RSIpcPersistenceDataBase {
public:
    SetShowRefreshRateEnabledPersistenceData() = default;
    SetShowRefreshRateEnabledPersistenceData(bool enabled, int32_t type) : enabled_(enabled), type_(type) {}
    ~SetShowRefreshRateEnabledPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override { return RSIpcPersistenceType::SET_SHOW_REFRESH_RATE_ENABLED; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static SetShowRefreshRateEnabledPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    bool enabled_;
    int32_t type_;
};

class SelfDrawingNodeRectChangeCallbackPersistenceData : public RSIpcPersistenceDataBase {
public:
    SelfDrawingNodeRectChangeCallbackPersistenceData() = default;
    SelfDrawingNodeRectChangeCallbackPersistenceData(
        pid_t pid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback)
        : pid_(pid), constraint_(constraint), callback_(callback) {}
    ~SelfDrawingNodeRectChangeCallbackPersistenceData() noexcept override = default;

    RSIpcPersistenceType GetType() const override
    {
        return RSIpcPersistenceType::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK;
    }
    pid_t GetCallingPid() const override { return pid_; }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static SelfDrawingNodeRectChangeCallbackPersistenceData* Unmarshalling(Parcel& parcel);

    void Apply(const sptr<RSRenderPipelineAgent>& renderPipelineAgent) override;

private:
    pid_t pid_;
    RectConstraint constraint_;
    sptr<RSISelfDrawingNodeRectChangeCallback> callback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_PROCESS_TRANSACTION_IPC_PERSISTENCE_RS_IPC_PERSISTENCE_DATA_H
