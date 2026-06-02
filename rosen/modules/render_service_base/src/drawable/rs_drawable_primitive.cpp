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
 
#ifdef USE_PRIMITIVE
#include "common/rs_common_def.h"
#include "drawable/rs_drawable.h"
#include "drawable/rs_misc_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "platform/common/rs_log.h"
#include "primitive/primitive_adapter.h"
#include "rs_trace.h"
 
namespace OHOS::Rosen {
void RSDrawable::OnDrawPrimitive(Drawing::Canvas* canvas, const Drawing::Rect* rect)
{
    if (!UsePrimList()) {
        primList_ = nullptr;
#ifdef PRIMITIVE_PROFILER
        ROSEN_LOGI("RSDrawable::OnDrawPrimitive, UsePrimList return false");
#endif
        OnDraw(canvas, rect);
        return;
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
    if (!paintFilterCanvas) {
        OnDraw(canvas, rect);
        return;
    }

    auto primListAdapter = paintFilterCanvas->primListAdapter_;
    if (primListAdapter == nullptr) {
        OnDraw(canvas, rect);
        return;
    }

    if (!primListAdapter->IsPrimListEnable()) {
        OnDraw(canvas, rect);
        return;
    }

    if (primListAdapter->IsDirty()) {
        goto PRIM_COLLECTING;
    }

    if (isDirty_) {
        isDirty_ = false;
        goto PRIM_COLLECTING;
    }

    if (primList_ == nullptr) {
        goto PRIM_COLLECTING;
    }

    if (primListAdapter->DrawPrimList(*canvas, primList_)) {
        return;
    }

PRIM_COLLECTING:
    primListAdapter->BeginPrimCollecting(*canvas, *rect);
    OnDraw(canvas, rect);
    primList_ = primListAdapter->EndPrimCollecting(*canvas);
    return;
}

void RSDrawable::OnPrimitiveSync()
{
    OnSync();
    isDirty_ = isDirty_ || stagingIsDirty_;
    stagingIsDirty_ = false;
    RS_OPTIONAL_TRACE_NAME_FMT("RSDrawable::OnPrimitiveSync, isDirty_[%d]", isDirty_);
}

void DrawableV2::RSChildrenDrawable::OnDrawPrimitive(Drawing::Canvas* canvas, const Drawing::Rect* rect)
{
    OnDraw(canvas, rect);
    return;
}
} // namespace OHOS::Rosen
#endif