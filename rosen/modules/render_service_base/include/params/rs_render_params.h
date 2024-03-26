/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "property/rs_properties.h"
#include "utils/matrix.h"

namespace OHOS::Rosen {
#define RENDER_BASIC_PARAM_TO_STRING(basicType) (std::string("param[") + #basicType + "]:" + std::to_string(basicType) + " \n")
#define RENDER_RECT_PARAM_TO_STRING(rect) (std::string("param[") + #rect + "]:" + rect.ToString() + " \n")
#define RENDER_PARAM_TO_STRING(param) (std::string("param[") + #param + "]:" + param.ToString() + " \n")

struct DirtyRegionInfoForDFX {
    RectI oldDirty;
    RectI oldDirtyInSurface;
    bool operator==(const DirtyRegionInfoForDFX& rhs) const
    {
        return oldDirty == rhs.oldDirty && oldDirtyInSurface == rhs.oldDirtyInSurface;
    }
};

class RSB_EXPORT RSRenderParams {
public:
    RSRenderParams() = default;
    virtual ~RSRenderParams() = default;

    virtual void SetMatrix(Drawing::Matrix matrix);
    const Drawing::Matrix GetMatrix() const;

    virtual void SetBoundsRect(Drawing::RectF boundsRect);
    const Drawing::Rect GetBounds() const;

    // return to add some dirtynode does not mark pending
    virtual bool SetLocalDrawRect(RectI localDrawRect);
    const RectI GetLocalDrawRect() const;

    bool GetShouldPaint() const;
    void SetShouldPaint(bool shouldPaint);

    bool NeedSync() const;
    void SetNeedSync(bool needSync);

    inline NodeId GetId() const
    {
        return id_;
    }

    void SetChildHasVisibleFilter(bool val);
    bool ChildHasVisibleFilter() const;
    void SetChildHasVisibleEffect(bool val);
    bool ChildHasVisibleEffect() const;

    void SetCacheSize(Vector2f size);
    Vector2f GetCacheSize() const;

    void SetDrawingCacheChanged(bool isChanged);
    bool GetDrawingCacheChanged() const;

    void SetDrawingCacheType(RSDrawingCacheType cacheType);
    RSDrawingCacheType GetDrawingCacheType() const;

    void SetShadowRect(Drawing::Rect rect);
    Drawing::Rect GetShadowRect() const;

    void SetDirtyRegionInfoForDFX(DirtyRegionInfoForDFX dirtyRegionInfo);
    DirtyRegionInfoForDFX GetDirtyRegionInfoForDFX() const;
    
    // disable copy and move
    RSRenderParams(const RSRenderParams&) = delete;
    RSRenderParams(RSRenderParams&&) = delete;
    RSRenderParams& operator=(const RSRenderParams&) = delete;
    RSRenderParams& operator=(RSRenderParams&&) = delete;

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target);

    // dfx
    virtual std::string ToString() const;

protected:
    bool needSync_ = false;

private:
    NodeId id_;
    Drawing::Matrix matrix_;
    Drawing::RectF boundsRect_;
    // this rect should map display coordination
    RectI localDrawRect_;
    bool shouldPaint_;

    Vector2f cacheSize_;
    bool childHasVisibleFilter_ = false;
    bool childHasVisibleEffect_ = false;
    bool isDrawingCacheChanged_ = false;
    Drawing::Rect shadowRect_;
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    DirtyRegionInfoForDFX dirtyRegionInfoForDFX_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
