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
#include "memory/rs_dfx_string.h"
#include <parcel.h>

#ifndef USE_ROSEN_DRAWING

class SkCanvas;
class SkSurface;
struct SkRect;
namespace OHOS {
namespace Rosen {
class OpItem;
class RSPaintFilterCanvas;
enum RSOpType : uint16_t;

class RSB_EXPORT DrawCmdList : public Parcelable {
public:
    DrawCmdList(int w, int h);
    DrawCmdList& operator=(DrawCmdList&& that);
    virtual ~DrawCmdList();

    void AddOp(std::unique_ptr<OpItem>&& op);
    void ClearOp();

    void UpdateNodeIdToPicture(NodeId nodeId);
    void FindIndexOfImage() const;

    void Playback(SkCanvas& canvas, const SkRect* rect = nullptr);
    void Playback(RSPaintFilterCanvas& canvas, const SkRect* rect = nullptr);

    std::string PlayBackForRecord(SkCanvas& canvas, int startOpId, int endOpId, int descStartOpId,
        const SkRect* rect = nullptr);
    std::string PlayBackForRecord(RSPaintFilterCanvas& canvas, int startOpId, int endOpId,
        int descStartOpId, const SkRect* rect = nullptr);

    void SetWidth(int width);
    void SetHeight(int height);
    std::string GetOpsWithDesc() const;
    size_t GetSize() const;
    int GetWidth() const;
    int GetHeight() const;
    void DumpPicture(DfxString& info) const;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSB_EXPORT DrawCmdList* Unmarshalling(Parcel& parcel);

    // cache related, only available on OHOS
    void GenerateCache(const RSPaintFilterCanvas* canvas = nullptr, const SkRect* rect = nullptr);
    void ClearCache();

private:
    std::vector<std::unique_ptr<OpItem>> ops_;
    mutable std::vector<uint32_t> imageIndexs_;
    mutable std::mutex mutex_;
    int width_;
    int height_;

#ifdef ROSEN_OHOS
    // cache related, only available on OHOS
    std::vector<std::pair<int, std::unique_ptr<OpItem>>> opReplacedByCache_;
    bool isCached_ = false;
    bool cachedHighContrast_ = false;
    friend class RSMarshallingHelper;
    using OpUnmarshallingFunc = OpItem* (*)(Parcel& parcel);
    static OpUnmarshallingFunc GetOpUnmarshallingFunc(RSOpType type);
#endif
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
} // namespace Rosen
} // namespace OHOS

#endif // USE_ROSEN_DRAWING
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_LIST_H
