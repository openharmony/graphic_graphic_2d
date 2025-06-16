/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_geometry_modifier_fuzzer.h"

#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

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

bool RSBoundsClipModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBoundsClipModifier> modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();

    modifier->GetType();
    modifier->MarkNodeDirty();
    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetClipRectWithRadius(randomVec4F, randomVec4F);
    std::shared_ptr<RRect> rrect = std::make_shared<RRect>();
    modifier->SetClipRRect(rrect);
    std::shared_ptr<RSPath> clipToBounds = std::make_shared<RSPath>();
    modifier->SetClipBounds(clipToBounds);
    bool RandomBool = GetData<bool>();
    modifier->SetClipToBounds(RandomBool);
    modifier->SetCornerRadius(randomVec4F);
    modifier->GetCornerRadius();

    return true;
}   

bool RSBoundsModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSBoundsModifier> modifier = std::make_shared<ModifierNG::RSBoundsModifier>();

    modifier->GetType();
    modifier->MarkNodeDirty();
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);
    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    Vector2f randomVec2F{GetData<float>(), GetData<float>()};
    float randomFloat = GetData<float>();
    modifier->SetBounds(randomVec4F);
    modifier->SetBoundsSize(randomVec2F);
    modifier->SetBoundsWidth(randomFloat);
    modifier->SetBoundsHeight(randomFloat);
    modifier->SetBoundsPosition(randomVec2F);
    modifier->SetBoundsPositionX(randomFloat);
    modifier->SetBoundsPositionY(randomFloat);

    modifier->GetBounds();
    modifier->GetBoundsSize();
    modifier->GetBoundsWidth();
    modifier->GetBoundsHeight();
    modifier->GetBoundsPosition();
    modifier->GetBoundsPositionX();
    modifier->GetBoundsPositionY();
    return true;
}

bool RSFrameClipModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSFrameClipModifier> modifier = std::make_shared<ModifierNG::RSFrameClipModifier>();

    modifier->GetType();
    modifier->MarkNodeDirty();
    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    bool randomBool = GetData<bool>();
    Gravity gravity = GetData<Gravity>();
    modifier->SetClipToFrame(randomBool);
    modifier->SetCustomClipToFrame(randomVec4F);
    modifier->SetFrameGravity(gravity);

    return true;
}

bool RSFrameModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSFrameModifier> modifier = std::make_shared<ModifierNG::RSFrameModifier>();

    modifier->GetType();
    modifier->MarkNodeDirty();

    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    Vector2f randomVec2F{GetData<float>(), GetData<float>()};
    float randomFloat = GetData<float>();
    modifier->SetFrame(randomVec4F);
    modifier->SetFrameSize(randomVec2F);
    modifier->SetFrameWidth(randomFloat);
    modifier->SetFrameHeight(randomFloat);
    modifier->SetFramePosition(randomVec2F);
    modifier->SetFramePositionX(randomFloat);
    modifier->SetFramePositionY(randomFloat);

    modifier->GetFrame();
    modifier->GetFrameSize();
    modifier->GetFrameWidth();
    modifier->GetFrameHeight();
    modifier->GetFramePosition();
    modifier->GetFramePositionX();
    modifier->GetFramePositionY();

    return true;
}

bool RSTransformModifierFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::shared_ptr<ModifierNG::RSTransformModifier> modifier = std::make_shared<ModifierNG::RSTransformModifier>();

    modifier->GetType();
    modifier->MarkNodeDirty();
    std::shared_ptr<RSObjAbsGeometry> geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);

    float randomFloat = GetData<float>();
    bool randomBool = GetData<bool>();
    std::optional<Vector2f> parentPosition = std::make_optional<Vector2f>();
    Vector2f randomVec2F{GetData<float>(), GetData<float>()};

    modifier->SetSandBox(parentPosition);
    modifier->SetPositionZ(randomFloat);
    modifier->SetPositionZApplicableCamera3D(randomBool);
    modifier->SetPivot(randomVec2F);
    modifier->SetPivotZ(randomFloat);

    modifier->GetSandBox();
    modifier->GetPositionZ();
    modifier->GetPositionZApplicableCamera3D();
    modifier->GetPivot();
    modifier->GetPivotZ();

    Quaternion quaternion;
    modifier->SetQuaternion(quaternion);
    modifier->SetRotation(randomFloat);
    modifier->SetRotationX(randomFloat);
    modifier->SetRotationY(randomFloat);

    modifier->GetQuaternion();
    modifier->GetRotation();
    modifier->GetRotationX();
    modifier->GetRotationY();

    modifier->SetCameraDistance(randomFloat);
    modifier->SetTranslate(randomVec2F);
    modifier->SetTranslateZ(randomFloat);
    modifier->GetCameraDistance();
    modifier->GetTranslate();
    modifier->GetTranslateZ();

    modifier->SetScale(randomVec2F);
    modifier->SetScaleZ(randomFloat);
    Vector3f randomVec3F{GetData<float>(), GetData<float>(), GetData<float>()};
    Vector4f randomVec4F{GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    modifier->SetSkew(randomVec3F);
    modifier->SetPersp(randomVec4F);

    modifier->GetScale();
    modifier->GetScaleZ();
    modifier->GetSkew();
    modifier->GetPersp();

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSBoundsClipModifierFuzzTest(data, size);
    OHOS::Rosen::RSBoundsModifierFuzzTest(data, size);
    OHOS::Rosen::RSFrameClipModifierFuzzTest(data, size);
    OHOS::Rosen::RSFrameModifierFuzzTest(data, size);
    OHOS::Rosen::RSTransformModifierFuzzTest(data, size);
    return 0;
}

