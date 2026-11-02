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

#include "RsProperty2_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier/rs_property.h"
#include "ui/rs_canvas_node.h"
#include "render/rs_image.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int MATRIX_SIZE = 3;
using FuzzFunc = void (*)(FuzzedDataProvider&);

void DoMarkCustomModifierDirty(FuzzedDataProvider& fdp)
{
    auto node = RSCanvasNode::Create();
    RSProperty<float> prop(1.0f);
    prop.MarkCustomModifierDirty();
    (void)fdp;
}

void DoMarkNodeDirty(FuzzedDataProvider& fdp)
{
    auto node = RSCanvasNode::Create();
    RSProperty<float> prop(1.0f);
    prop.MarkNodeDirty();
    (void)fdp;
}

void DoGeneratePropertyId(FuzzedDataProvider& fdp)
{
    (void)fdp;
    RSProperty<float> prop(1.0f);
    PropertyId id1 = prop.GetId();
    (void)id1;
}

void DoCheckPropertyType(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // CheckPropertyType is a template-based method, exercised implicitly
}

void DoUpdateExtendModifierForGeometry(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // UpdateExtendModifierForGeometry is called from RSNode
}

void DoIsDeduplicationEnabled(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // IsDeduplicationEnabled is a template method
}

void DoSetBool(FuzzedDataProvider& fdp)
{
    bool value = fdp.ConsumeBool();
    RSProperty<bool> prop(true);
    prop.Set(value);
}

void DoSetFloat(FuzzedDataProvider& fdp)
{
    float value = fdp.ConsumeFloatingPoint<float>();
    RSProperty<float> prop(1.0f);
    prop.Set(value);
}

void DoSetInt(FuzzedDataProvider& fdp)
{
    int32_t value = fdp.ConsumeIntegral<int32_t>();
    RSProperty<int32_t> prop(0);
    prop.Set(value);
}

void DoSetVector2f(FuzzedDataProvider& fdp)
{
    Vector2f value = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    RSProperty<Vector2f> prop(Vector2f(0, 0));
    prop.Set(value);
}

void DoSetVector4f(FuzzedDataProvider& fdp)
{
    Vector4f value = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    RSProperty<Vector4f> prop(Vector4f(0, 0, 0, 0));
    prop.Set(value);
}

void DoSetColor(FuzzedDataProvider& fdp)
{
    Color value = {fdp.ConsumeIntegral<uint8_t>(), fdp.ConsumeIntegral<uint8_t>(),
        fdp.ConsumeIntegral<uint8_t>(), fdp.ConsumeIntegral<uint8_t>()};
    RSProperty<Color> prop(Color(0, 0, 0, 0));
    prop.Set(value);
}

void DoSetGravity(FuzzedDataProvider& fdp)
{
    Gravity value = static_cast<Gravity>(fdp.ConsumeIntegral<uint8_t>());
    RSProperty<Gravity> prop(Gravity(0));
    prop.Set(value);
}

void DoSetMatrix3f(FuzzedDataProvider& fdp)
{
    Matrix3f value;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            value[i][j] = fdp.ConsumeFloatingPoint<float>();
        }
    }
    RSProperty<Matrix3f> prop{Matrix3f{}};
    prop.Set(value);
}

void DoSetQuaternion(FuzzedDataProvider& fdp)
{
    Quaternion value = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    RSProperty<Quaternion> prop(Quaternion(0, 0, 0, 0));
    prop.Set(value);
}

void DoSetRSImage(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // RSImage requires complex construction
}

void DoSetRSMask(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // RSMask requires complex construction
}

void DoSetRSPath(FuzzedDataProvider& fdp)
{
    (void)fdp;
    // RSPath requires complex construction
}

// Keep the order consistent with the original switch-case mapping.
const FuzzFunc FUZZ_FUNC_TABLE[] = {
    DoMarkCustomModifierDirty,
    DoMarkNodeDirty,
    DoGeneratePropertyId,
    DoCheckPropertyType,
    DoUpdateExtendModifierForGeometry,
    DoIsDeduplicationEnabled,
    DoSetBool,
    DoSetFloat,
    DoSetInt,
    DoSetVector2f,
    DoSetVector4f,
    DoSetColor,
    DoSetGravity,
    DoSetMatrix3f,
    DoSetQuaternion,
    DoSetRSImage,
    DoSetRSMask,
    DoSetRSPath,
};
constexpr size_t FUZZ_FUNC_TABLE_SIZE = sizeof(FUZZ_FUNC_TABLE) / sizeof(FUZZ_FUNC_TABLE[0]);
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    uint8_t choice = fdp.ConsumeIntegral<uint8_t>();
    OHOS::Rosen::FUZZ_FUNC_TABLE[choice % OHOS::Rosen::FUZZ_FUNC_TABLE_SIZE](fdp);
    return 0;
}
