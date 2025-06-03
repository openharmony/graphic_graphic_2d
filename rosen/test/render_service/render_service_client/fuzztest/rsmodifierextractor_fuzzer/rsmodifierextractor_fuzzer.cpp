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

#include "rsmodifierextractor_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "modifier/rs_modifier_extractor.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoGet001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSModifierExtractor modifierExtractor(1);
    modifierExtractor.GetBounds();
    modifierExtractor.GetFrame();
    modifierExtractor.GetPositionZ();
    modifierExtractor.GetPivot();
    modifierExtractor.GetPivotZ();
    modifierExtractor.GetQuaternion();
    modifierExtractor.GetRotation();
    modifierExtractor.GetRotationX();
    modifierExtractor.GetRotationY();
    modifierExtractor.GetCameraDistance();
    modifierExtractor.GetTranslate();
    modifierExtractor.GetTranslateZ();
    modifierExtractor.GetScale();
    modifierExtractor.GetSkew();
    modifierExtractor.GetPersp();
    modifierExtractor.GetAlpha();
    modifierExtractor.GetAlphaOffscreen();
    modifierExtractor.GetCornerRadius();
    modifierExtractor.GetForegroundColor();
    modifierExtractor.GetBackgroundColor();
    modifierExtractor.GetSurfaceBgColor();
    modifierExtractor.GetBackgroundShader();
    modifierExtractor.GetBgImage();
    modifierExtractor.GetBgImageWidth();
    modifierExtractor.GetBgImageHeight();
    modifierExtractor.GetBgImagePositionX();
    modifierExtractor.GetBgImagePositionY();
    modifierExtractor.GetBorderColor();
    modifierExtractor.GetBorderWidth();
    modifierExtractor.GetBorderStyle();
    modifierExtractor.GetOutlineColor();
    modifierExtractor.GetOutlineWidth();
    modifierExtractor.GetOutlineStyle();

    return true;
}

bool DoGet002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSModifierExtractor modifierExtractor(1);
    modifierExtractor.GetHDRUIBrightness();
    modifierExtractor.GetOutlineRadius();
    modifierExtractor.GetForegroundEffectRadius();
    modifierExtractor.GetShadowColor();
    modifierExtractor.GetShadowOffsetX();
    modifierExtractor.GetShadowOffsetY();
    modifierExtractor.GetShadowAlpha();
    modifierExtractor.GetShadowElevation();
    modifierExtractor.GetShadowRadius();
    modifierExtractor.GetShadowPath();
    modifierExtractor.GetShadowMask();
    modifierExtractor.GetShadowIsFilled();
    modifierExtractor.GetShadowColorStrategy();
    modifierExtractor.GetFrameGravity();
    modifierExtractor.GetClipBounds();
    modifierExtractor.GetClipToBounds();
    modifierExtractor.GetClipToFrame();
    modifierExtractor.GetVisible();
    modifierExtractor.GetMask();
    modifierExtractor.GetSpherizeDegree();
    modifierExtractor.GetHDRUIBrightness();
    modifierExtractor.GetLightUpEffectDegree();
    modifierExtractor.GetDynamicDimDegree();
    modifierExtractor.GetBackgroundBlurRadius();
    modifierExtractor.GetBackgroundBlurSaturation();
    modifierExtractor.GetBackgroundBlurBrightness();
    modifierExtractor.GetBackgroundBlurMaskColor();
    modifierExtractor.GetBackgroundBlurColorMode();
    modifierExtractor.GetBackgroundBlurRadiusX();
    modifierExtractor.GetBackgroundBlurRadiusY();
    modifierExtractor.GetForegroundBlurRadius();

    return true;
}

bool DoGet003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSModifierExtractor modifierExtractor(1);
    modifierExtractor.GetForegroundBlurSaturation();
    modifierExtractor.GetForegroundBlurBrightness();
    modifierExtractor.GetForegroundBlurMaskColor();
    modifierExtractor.GetForegroundBlurColorMode();
    modifierExtractor.GetForegroundBlurRadiusX();
    modifierExtractor.GetForegroundBlurRadiusY();
    modifierExtractor.GetAttractionFractionValue();
    modifierExtractor.GetAttractionDstPointValue();
    modifierExtractor.GetLightIntensity();
    modifierExtractor.GetLightColor();
    modifierExtractor.GetLightPosition();
    modifierExtractor.GetIlluminatedBorderWidth();
    modifierExtractor.GetIlluminatedType();
    modifierExtractor.GetBloom();
    modifierExtractor.Dump();

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGet001(data, size);
    OHOS::Rosen::DoGet002(data, size);
    OHOS::Rosen::DoGet003(data, size);
    return 0;
}

