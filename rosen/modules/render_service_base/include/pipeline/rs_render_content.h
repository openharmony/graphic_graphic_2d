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
#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H

#include "common/rs_macros.h"
#include "memory/rs_dfx_string.h"
#include "modifier/rs_render_modifier.h"
#include "property/rs_properties.h"
#include "property/rs_property_drawable.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderContent {
public:
    RSRenderContent();
    RSRenderContent(const RSRenderContent&) = delete;
    RSRenderContent(const RSRenderContent&&) = delete;
    RSRenderContent& operator=(const RSRenderContent&) = delete;
    RSRenderContent& operator=(const RSRenderContent&&) = delete;

    RSProperties& GetMutableRenderProperties();
    const RSProperties& GetRenderProperties() const;

    const RSPropertyDrawable::DrawableVec& GetPropertyDrawableVec() const;
    void IterateOnDrawableRange(Slot::RSPropertyDrawableSlot begin, Slot::RSPropertyDrawableSlot end,
        RSPaintFilterCanvas& canvas, RSRenderNode& node);

private:
    RSProperties renderProperties_;
    RSPropertyDrawable::DrawableVec propertyDrawablesVec_;

    friend class RSRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_RENDER_CONTENT_H