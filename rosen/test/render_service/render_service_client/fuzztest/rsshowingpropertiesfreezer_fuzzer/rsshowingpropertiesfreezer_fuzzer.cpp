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

#include "rsshowingpropertiesfreezer_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "modifier/rs_showing_properties_freezer.h"

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

bool DoGetBounds(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBounds();

    return true;
}

bool DoGetFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetFrame();

    return true;
}

bool DoGetPositionZ(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetPositionZ();

    return true;
}

bool DoGetPivot(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetPivot();

    return true;
}

bool DoGetPivotZ(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetPivotZ();

    return true;
}

bool DoGetQuaternion(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetQuaternion();

    return true;
}

bool DoGetRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetRotation();

    return true;
}

bool DoGetRotationX(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetRotationX();

    return true;
}

bool DoGetRotationY(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetRotationY();

    return true;
}

bool DoGetCameraDistance(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetCameraDistance();

    return true;
}

bool DoGetTranslate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetTranslate();

    return true;
}

bool DoGetTranslateZ(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetTranslateZ();

    return true;
}

bool DoGetScale(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetScale();

    return true;
}

bool DoGetSkew(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetSkew();

    return true;
}

bool DoGetPersp(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetPersp();

    return true;
}

bool DoGetAlpha(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetAlpha();

    return true;
}

bool DoGetCornerRadius(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetCornerRadius();

    return true;
}

bool DoGetForegroundColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetForegroundColor();

    return true;
}

bool DoGetBackgroundColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBackgroundColor();

    return true;
}

bool DoGetBgImageWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBgImageWidth();

    return true;
}

bool DoGetBgImageHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBgImageHeight();

    return true;
}

bool DoGetBgImagePositionX(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBgImagePositionX();

    return true;
}

bool DoGetBgImagePositionY(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBgImagePositionY();

    return true;
}

bool DoGetBorderColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBorderColor();

    return true;
}

bool DoGetBorderWidth(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetBorderWidth();

    return true;
}

bool DoGetShadowColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowColor();

    return true;
}

bool DoGetShadowOffsetX(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowOffsetX();

    return true;
}

bool DoGetShadowOffsetY(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowOffsetY();

    return true;
}

bool DoGetShadowAlpha(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowAlpha();

    return true;
}

bool DoGetShadowElevation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowElevation();

    return true;
}

bool DoGetShadowRadius(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetShadowRadius();

    return true;
}

bool DoGetSpherizeDegree(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetSpherizeDegree();

    return true;
}

bool DoGetHDRUIBrightness(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetHDRUIBrightness();

    return true;
}

bool DoGetAttractionFractionValue(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetAttractionFractionValue();

    return true;
}

bool DoGetAttractionDstPointValue(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetAttractionDstPointValue();

    return true;
}

bool DoGetLightUpEffectDegree(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetLightUpEffectDegree();

    return true;
}

bool DoGetDynamicDimDegree(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    RSShowingPropertiesFreezer showingPropertiesFreezer(id);
    showingPropertiesFreezer.GetDynamicDimDegree();

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetBounds(data, size);
    OHOS::Rosen::DoGetFrame(data, size);
    OHOS::Rosen::DoGetPositionZ(data, size);
    OHOS::Rosen::DoGetPivot(data, size);
    OHOS::Rosen::DoGetPivotZ(data, size);
    OHOS::Rosen::DoGetQuaternion(data, size);
    OHOS::Rosen::DoGetRotation(data, size);
    OHOS::Rosen::DoGetRotationX(data, size);
    OHOS::Rosen::DoGetRotationY(data, size);
    OHOS::Rosen::DoGetCameraDistance(data, size);
    OHOS::Rosen::DoGetTranslate(data, size);
    OHOS::Rosen::DoGetTranslateZ(data, size);
    OHOS::Rosen::DoGetScale(data, size);
    OHOS::Rosen::DoGetSkew(data, size);
    OHOS::Rosen::DoGetPersp(data, size);
    OHOS::Rosen::DoGetAlpha(data, size);
    OHOS::Rosen::DoGetCornerRadius(data, size);
    OHOS::Rosen::DoGetForegroundColor(data, size);
    OHOS::Rosen::DoGetBackgroundColor(data, size);
    OHOS::Rosen::DoGetBgImageWidth(data, size);
    OHOS::Rosen::DoGetBgImageHeight(data, size);
    OHOS::Rosen::DoGetBgImagePositionX(data, size);
    OHOS::Rosen::DoGetBgImagePositionY(data, size);
    OHOS::Rosen::DoGetBorderColor(data, size);
    OHOS::Rosen::DoGetBorderWidth(data, size);
    OHOS::Rosen::DoGetShadowColor(data, size);
    OHOS::Rosen::DoGetShadowOffsetX(data, size);
    OHOS::Rosen::DoGetShadowOffsetY(data, size);
    OHOS::Rosen::DoGetShadowAlpha(data, size);
    OHOS::Rosen::DoGetShadowElevation(data, size);
    OHOS::Rosen::DoGetShadowRadius(data, size);
    OHOS::Rosen::DoGetSpherizeDegree(data, size);
    OHOS::Rosen::DoGetAttractionFractionValue(data, size);
    OHOS::Rosen::DoGetAttractionDstPointValue(data, size);
    OHOS::Rosen::DoGetLightUpEffectDegree(data, size);
    OHOS::Rosen::DoGetDynamicDimDegree(data, size);
    OHOS::Rosen::DoGetHDRUIBrightness(data, size);
    return 0;
}

