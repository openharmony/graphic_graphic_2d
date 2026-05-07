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

#include "rsgeometrymodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "modifier_ng/geometry/rs_transform_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_BOUNDS_CLIP = 0;
const uint8_t DO_BOUNDS = 1;
const uint8_t DO_FRAME_CLIP = 2;
const uint8_t DO_FRAME = 3;
const uint8_t DO_TRANSFORM = 4;
const uint8_t TARGET_SIZE = 5;
constexpr int32_t GRAVITY_MAX =
    static_cast<int32_t>(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT) + 1;
}

void DoRSBoundsClipModifierFuzzTest(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    modifier->GetType();
    modifier->MarkNodeDirty();
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetClipRectWithRadius(randomVec4F, randomVec4F);
    auto rrect = std::make_shared<RRect>();
    modifier->SetClipRRect(rrect);
    auto clipToBounds = std::make_shared<RSPath>();
    modifier->SetClipBounds(clipToBounds);
    modifier->SetClipToBounds(fdp.ConsumeBool());
    modifier->SetCornerRadius(randomVec4F);
    modifier->GetCornerRadius();
}

void DoRSBoundsModifierFuzzTest(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->GetType();
    modifier->MarkNodeDirty();
    auto geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    Vector2f randomVec2F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
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
}

void DoRSFrameClipModifierFuzzTest(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSFrameClipModifier>();
    modifier->GetType();
    modifier->MarkNodeDirty();
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetClipToFrame(fdp.ConsumeBool());
    modifier->SetCustomClipToFrame(randomVec4F);
    modifier->SetFrameGravity(static_cast<Gravity>(fdp.ConsumeIntegral<uint32_t>() % GRAVITY_MAX));
}

void DoRSFrameModifierFuzzTest(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSFrameModifier>();
    modifier->GetType();
    modifier->MarkNodeDirty();
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    Vector2f randomVec2F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
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
}

void DoRSTransformModifierFuzzTest(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSTransformModifier>();
    modifier->GetType();
    modifier->MarkNodeDirty();
    auto geometry = std::make_shared<RSObjAbsGeometry>();
    modifier->ApplyGeometry(geometry);
    float randomFloat = fdp.ConsumeFloatingPoint<float>();
    bool randomBool = fdp.ConsumeBool();
    std::optional<Vector2f> parentPosition = std::make_optional<Vector2f>();
    Vector2f randomVec2F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
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
    Vector3f randomVec3F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>()};
    Vector4f randomVec4F{fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    modifier->SetSkew(randomVec3F);
    modifier->SetPersp(randomVec4F);
    modifier->GetScale();
    modifier->GetScaleZ();
    modifier->GetSkew();
    modifier->GetPersp();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_BOUNDS_CLIP:
            OHOS::Rosen::DoRSBoundsClipModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_BOUNDS:
            OHOS::Rosen::DoRSBoundsModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_FRAME_CLIP:
            OHOS::Rosen::DoRSFrameClipModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_FRAME:
            OHOS::Rosen::DoRSFrameModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_TRANSFORM:
            OHOS::Rosen::DoRSTransformModifierFuzzTest(fdp);
            break;
        default:
            break;
    }
    return 0;
}
