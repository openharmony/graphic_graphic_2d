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

#include "pipeline/rs_render_node.h"
#include "modifier_ng/appearance/rs_alpha_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_ATTACH_PROPERTY = 0;
const uint8_t DO_DETACH_PROPERTY = 1;
const uint8_t DO_APPLY_LEGACY_PROPERTY = 2;
const uint8_t DO_FIND_PROPERTY_TYPE = 3;
const uint8_t TARGET_SIZE = 4;
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
        default:
            break;
    }
    return 0;
}
