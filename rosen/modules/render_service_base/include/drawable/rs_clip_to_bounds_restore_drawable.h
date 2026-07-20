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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_CLIP_TO_BOUNDS_RESTORE_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_CLIP_TO_BOUNDS_RESTORE_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"

#include "drawable/rs_drawable.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSNGRenderShapeBase;

namespace DrawableV2 {
/**
 * @brief Restore drawable for CLIP_SDF: snapshots the offscreen content, restores the canvas
 * (undo BeginOffscreen), composites the snapshot, then RestoreToCount to balance
 * BG_SAVE_BOUNDS's Save.
 */
class RSClipToBoundsRestoreDrawable : public RSDrawable {
public:
    explicit RSClipToBoundsRestoreDrawable(std::shared_ptr<uint32_t> content) : content_(std::move(content)) {}
    ~RSClipToBoundsRestoreDrawable() override = default;

    /**
     * @brief Pick the SDF shape's transform rect when it matches the node bounding box, else
     * fall back to the bounding box, then expand 1px to avoid edge precision loss. Shared by
     * RSClipToBoundsDrawable::OnUpdate and RSClipToBoundsRestoreDrawable::OnUpdate.
     */
    static Drawing::Rect CalcSdfDrawRect(const RSRenderNode& node, const RSNGRenderShapeBase& sdfShape);

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node, std::shared_ptr<uint32_t> content);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override;

private:
    std::shared_ptr<uint32_t> content_; // shared save count for RestoreToCount (balances BG_SAVE_BOUNDS)

    bool needSync_ = false;
    // OnDraw runs DrawSdfClip when sdfShape_ is non-null (CLIP_SDF); otherwise RestoreToCount only.
    // OnUpdate always returns true so the drawable is never erased in standard mode.
    std::shared_ptr<RSNGRenderShapeBase> stagingSdfShape_ = nullptr;
    std::shared_ptr<RSNGRenderShapeBase> sdfShape_ = nullptr;
    Drawing::Rect stagingSdfDrawRect_;
    Drawing::Rect sdfDrawRect_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_CLIP_TO_BOUNDS_RESTORE_DRAWABLE_H
