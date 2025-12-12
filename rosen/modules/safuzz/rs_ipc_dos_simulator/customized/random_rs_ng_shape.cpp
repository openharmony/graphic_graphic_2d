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

#include "customized/random_rs_ng_shape.h"

#include "common/safuzz_log.h"
#include "customized/random_rs_render_property_base.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_SHAPE_DEPTH = 5; // 5: MAX_SHAPE_DEPTH
uint32_t g_shapeDepth = 0;
bool g_isInit = false;
std::vector<std::function<std::shared_ptr<RSNGRenderShapeBase>()>> g_randomShapeGenerator;
}

void RandomRSNGShapePtr::ResetShapeDepth()
{
    g_shapeDepth = 0;
}

std::shared_ptr<RSNGRenderShapeBase> RandomRSNGShapePtr::GetRandomValue()
{
    if (!g_isInit) {
        g_randomShapeGenerator.push_back(RandomRSNGShapePtr::GetNullValue);
#define DECLARE_SHAPE(ShapeName, ShapeType, ...)     \
    g_randomShapeGenerator.push_back(RandomRSNGShapePtr::GetRandom##ShapeName)

#include "effect/rs_render_shape_def.in"

#undef DECLARE_SHAPE
        g_isInit = true;
    }

    bool generateChain = RandomData::GetRandomBool();
    if (generateChain) {
        return RandomRSNGShapePtr::GetRandomShapeChain();
    }

    return RandomRSNGShapePtr::GetRandomSingleShape();
}

std::shared_ptr<RSNGRenderShapeBase> RandomRSNGShapePtr::GetNullValue()
{
    return nullptr;
}

std::shared_ptr<RSNGRenderShapeBase> RandomRSNGShapePtr::GetRandomSingleShape()
{
    if (g_shapeDepth > MAX_SHAPE_DEPTH) {
        return nullptr;
    }
    g_shapeDepth++;
    auto index = RandomEngine::GetRandomIndex(g_randomShapeGenerator.size() - 1);
    return g_randomShapeGenerator[index]();
}

std::shared_ptr<RSNGRenderShapeBase> RandomRSNGShapePtr::GetRandomShapeChain()
{
    std::shared_ptr<RSNGRenderShapeBase> head = nullptr;
    auto current = head;
    int shapeChainSize = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < shapeChainSize; ++i) {
        auto shape = GetRandomSingleShape();
        if (!shape) {
            continue;
        }

        if (!current) {
            head = shape; // init head
        } else {
            current->nextEffect_ = shape;
        }
        current = shape;
    }

    return head;
}

#define ADD_PROPERTY_TAG(Effect, Prop) \
    value->Setter<Effect##Prop##RenderTag>( \
        RandomRSRenderPropertyBase::GetRandomValue<typename Effect##Prop##RenderTag::ValueType>())
#define DECLARE_SHAPE(ShapeName, ShapeType, ...)                                    \
std::shared_ptr<RSNGRenderShapeBase> RandomRSNGShapePtr::GetRandom##ShapeName()     \
{                                                                                   \
    auto value = std::make_shared<RSNGRender##ShapeName>();                         \
    __VA_ARGS__;                                                                    \
    return value;                                                                   \
}

#include "effect/rs_render_shape_def.in"

#undef ADD_PROPERTY_TAG
#undef DECLARE_SHAPE

} // namespace Rosen
} // namespace OHOS
