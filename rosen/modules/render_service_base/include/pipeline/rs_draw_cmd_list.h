/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "common/rs_macros.h"
#include <parcel.h>

class SkCanvas;
class SkSurface;
struct SkRect;
namespace OHOS {
namespace Rosen {
class OpItem;
class RSPaintFilterCanvas;

class RSB_EXPORT DrawCmdList : public Parcelable {
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

    void GenerateCache(SkSurface* surface);
    void ClearCache();

    bool Marshalling(Parcel& parcel) const override;
    static RSB_EXPORT DrawCmdList* Unmarshalling(Parcel& parcel);

private:
    std::vector<std::unique_ptr<OpItem>> ops_;
    mutable std::mutex mutex_;
    int width_;
    int height_;

    std::unordered_map<int, std::unique_ptr<OpItem>> opReplacedByCache_;
#ifdef ROSEN_OHOS
    bool isCached_ = false;
#endif
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;

class RS_EXPORT DrawCmdListManager {
public:
    static DrawCmdListManager& Instance();

    void RegisterDrawCmdList(NodeId id, std::shared_ptr<DrawCmdList> drawCmdList);
    void ClearDrawCmdList(NodeId id);

    void MarkForceClear(bool flag);

    DrawCmdListManager() = default;
    ~DrawCmdListManager();

private:
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
