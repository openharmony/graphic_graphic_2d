/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_LIST_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_LIST_H

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

class SkCanvas;
struct SkRect;
namespace OHOS {
namespace Rosen {
class OpItem;
class RSPaintFilterCanvas;

#ifdef ROSEN_OHOS
class DrawCmdList : public Parcelable {
#else
class DrawCmdList {
#endif
public:
    DrawCmdList(int w, int h);
    DrawCmdList& operator=(DrawCmdList&& that);
    virtual ~DrawCmdList();

    void AddOp(std::unique_ptr<OpItem>&& op);
    void ClearOp();

    void Playback(SkCanvas& canvas, const SkRect* rect = nullptr) const;
    void Playback(RSPaintFilterCanvas& canvas, const SkRect* rect = nullptr) const;

    int GetSize() const;
    int GetWidth() const;
    int GetHeight() const;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    static DrawCmdList* Unmarshalling(Parcel& parcel);
#endif

private:
    std::vector<std::unique_ptr<OpItem>> ops_;
    mutable std::mutex mutex_;
    int width_;
    int height_;
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;

class DrawCmdListManager {
public:
    static DrawCmdListManager& Instance();

    void RegisterDrawCmdList(NodeId id, std::shared_ptr<DrawCmdList> drawCmdList);
    void ClearDrawCmdList(NodeId id);

    void MarkForceClear(bool flag);

private:
    DrawCmdListManager() = default;
    ~DrawCmdListManager() = default;

    DrawCmdListManager(const DrawCmdListManager&) = delete;
    DrawCmdListManager(const DrawCmdListManager&&) = delete;
    DrawCmdListManager& operator=(const DrawCmdListManager&) = delete;
    DrawCmdListManager& operator=(const DrawCmdListManager&&) = delete;

    std::atomic_bool forceClear_ = true;

    std::mutex listsMutex_;
    std::unordered_map<NodeId, std::vector<std::weak_ptr<DrawCmdList>>> lists_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_LIST_H
