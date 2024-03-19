/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "render/rs_kawase_blur.h"
#include "platform/common/rs_system_properties.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"

// Advanced Filter
static bool IsAdvancedFilterUsable()
{
    return false;
}

static const bool IS_ADVANCED_FILTER_USABLE_CHECK_ONCE = IsAdvancedFilterUsable();

bool KawaseBlurFilter::ApplyDrawingKawaseBlur(Drawing::Canvas& canvas, Drawing::Brush& brush,
    const std::shared_ptr<Drawing::Image>& image, const Drawing::KawaseParameters& blurParam)
{
    static bool useKawaseOriginal = RSSystemProperties::GetKawaseOriginalEnabled();
    static bool addRandomColor = RSSystemProperties::GetRandomColorEnabled();
    static float factor = RSSystemProperties::GetKawaseRandomColorFactor();

    auto properties = Drawing::KawaseProperties{IS_ADVANCED_FILTER_USABLE_CHECK_ONCE,
        useKawaseOriginal, addRandomColor, factor};
    RS_OPTIONAL_TRACE_BEGIN("ApplyKawaseBlur with radius :" + std::to_string(blurParam.radius));
    auto kawaseFunc = Drawing::KawaseBlurUtils();
    bool res = kawaseFunc.ApplyKawaseBlur(canvas, brush, image, blurParam, properties);
    RS_OPTIONAL_TRACE_END();
    return res;
}
} // namespace Rosen
} // namespace OHOS