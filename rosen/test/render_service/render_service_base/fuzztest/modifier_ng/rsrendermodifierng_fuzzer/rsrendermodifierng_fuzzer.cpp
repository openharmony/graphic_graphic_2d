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

#include "rsrendermodifierng_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_ATTACH_PROPERTY = 0;
const uint8_t DO_DETACH_PROPERTY = 1;
const uint8_t DO_APPLY_LEGACY_PROPERTY = 2;
const uint8_t DO_FIND_PROPERTY_TYPE = 3;
const uint8_t DO_IS_ATTACHED = 4;
const uint8_t DO_ON_ATTACH_MODIFIER = 5;
const uint8_t DO_ON_DETACH_MODIFIER = 6;
const uint8_t DO_SET_DIRTY = 7;
const uint8_t DO_MARSHALLING = 8;
const uint8_t DO_UNMARSHALLING = 9;
const uint8_t DO_GET_RESET_FUNC_MAP = 10;
const uint8_t TARGET_SIZE = 11;
}

void DoAttachPropertyFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    auto property = std::make_shared<RSRenderProperty<bool>>();
    ModifierNG::RSPropertyType type =
        static_cast<ModifierNG::RSPropertyType>(fdp.ConsumeIntegral<uint32_t>());
    modifier->AttachProperty(type, property);
}

void DoDetachPropertyFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    ModifierNG::RSPropertyType type =
        static_cast<ModifierNG::RSPropertyType>(fdp.ConsumeIntegral<uint32_t>());
    modifier->DetachProperty(type);
}

void DoApplyLegacyPropertyFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    RSProperties properties;
    properties.SetAlpha(fdp.ConsumeFloatingPoint<float>());
    properties.SetAlphaOffscreen(fdp.ConsumeBool());
    modifier->ApplyLegacyProperty(properties);
}

void DoFindPropertyTypeFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    auto property = std::make_shared<RSRenderProperty<bool>>();
    modifier->FindPropertyType(property);
}

void DoIsAttachedFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    modifier->IsAttached();
}

void DoOnAttachModifierFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    NodeId nodeId = fdp.ConsumeIntegral<NodeId>();
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);
    modifier->OnAttachModifier(*renderNode);
}

void DoOnDetachModifierFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    modifier->OnDetachModifier();
}

void DoSetDirtyFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    modifier->SetDirty();
}

void DoMarshallingFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    auto modifier = std::make_shared<ModifierNG::RSAlphaRenderModifier>(id);
    auto property = std::make_shared<RSRenderProperty<float>>(
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeIntegral<PropertyId>());
    ModifierNG::RSPropertyType type =
        static_cast<ModifierNG::RSPropertyType>(fdp.ConsumeIntegral<uint32_t>());
    modifier->AttachProperty(type, property);
    Parcel parcel;
    modifier->Marshalling(parcel);
}

void DoUnmarshallingFuzzTest(FuzzedDataProvider& fdp)
{
    ModifierId id = fdp.ConsumeIntegral<ModifierId>();
    Parcel parcel;
    bool writeResult = parcel.WriteUint8(static_cast<uint8_t>(ModifierNG::RSModifierType::ALPHA));
    writeResult = writeResult && parcel.WriteUint64(id);
    if (writeResult) {
        auto result = ModifierNG::RSRenderModifier::Unmarshalling(parcel);
        (void)result;
    }
}

void DoGetResetFuncMapFuzzTest(FuzzedDataProvider& fdp)
{
    auto& map = ModifierNG::RSRenderModifier::GetResetFuncMap();
    RSProperties properties;
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % static_cast<uint8_t>(map.size());
    uint8_t idx = 0;
    for (auto& [modifierType, resetFunc] : map) {
        if (idx == tarPos && resetFunc != nullptr) {
            resetFunc(properties);
            break;
        }
        idx++;
    }
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
        case OHOS::Rosen::DO_ATTACH_PROPERTY:
            OHOS::Rosen::DoAttachPropertyFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_DETACH_PROPERTY:
            OHOS::Rosen::DoDetachPropertyFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_APPLY_LEGACY_PROPERTY:
            OHOS::Rosen::DoApplyLegacyPropertyFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_FIND_PROPERTY_TYPE:
            OHOS::Rosen::DoFindPropertyTypeFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_IS_ATTACHED:
            OHOS::Rosen::DoIsAttachedFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_ON_ATTACH_MODIFIER:
            OHOS::Rosen::DoOnAttachModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_ON_DETACH_MODIFIER:
            OHOS::Rosen::DoOnDetachModifierFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_SET_DIRTY:
            OHOS::Rosen::DoSetDirtyFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_MARSHALLING:
            OHOS::Rosen::DoMarshallingFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_UNMARSHALLING:
            OHOS::Rosen::DoUnmarshallingFuzzTest(fdp);
            break;
        case OHOS::Rosen::DO_GET_RESET_FUNC_MAP:
            OHOS::Rosen::DoGetResetFuncMapFuzzTest(fdp);
            break;
        default:
            break;
    }
    return 0;
}
