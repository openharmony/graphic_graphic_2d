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

#include "customized/random_rs_ng_shader.h"

#include "common/safuzz_log.h"
#include "customized/random_rs_render_property_base.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
bool g_isInit = false;
std::vector<std::function<std::shared_ptr<RSNGRenderShaderBase>()>> g_randomShaderGenerator;
}

std::shared_ptr<RSNGRenderShaderBase> RandomRSNGShaderPtr::GetRandomValue()
{
    if (!g_isInit) {
        g_randomShaderGenerator.push_back(RandomRSNGShaderPtr::GetNullValue);
#define DECLARE_SHADER(ShaderName, ShaderType, ...)     \
            g_randomShaderGenerator.push_back(RandomRSNGShaderPtr::GetRandom##ShaderName)

#include "effect/rs_render_shader_def.in"

#undef DECLARE_SHADER
        g_isInit = true;
    }

    bool generateChain = RandomData::GetRandomBool();
    if (generateChain) {
        return RandomRSNGShaderPtr::GetRandomShaderChain();
    }

    return RandomRSNGShaderPtr::GetRandomSingleShader();
}

std::shared_ptr<RSNGRenderShaderBase> RandomRSNGShaderPtr::GetNullValue()
{
    return nullptr;
}

std::shared_ptr<RSNGRenderShaderBase> RandomRSNGShaderPtr::GetRandomSingleShader()
{
    int index = RandomEngine::GetRandomIndex(g_randomShaderGenerator.size() - 1);
    return g_randomShaderGenerator[index]();
}

std::shared_ptr<RSNGRenderShaderBase> RandomRSNGShaderPtr::GetRandomShaderChain()
{
    std::shared_ptr<RSNGRenderShaderBase> head = nullptr;
    auto current = head;
    int shaderChainSize = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < shaderChainSize; ++i) {
        auto shader = GetRandomSingleShader();
        if (!shader) {
            continue;
        }

        if (!current) {
            head = shader; // init head
        } else {
            current->nextEffect_ = shader;
        }
        current = shader;
    }

    return head;
}

#define SEPARATOR
#define ADD_PROPERTY_TAG(Effect, Prop) \
    value->Setter<Effect##Prop##RenderTag>( \
        RandomRSRenderPropertyBase::GetRandomValue<typename Effect##Prop##RenderTag::ValueType>());
#define DECLARE_SHADER(ShaderName, ShaderType, ...)                                 \
std::shared_ptr<RSNGRenderShaderBase> RandomRSNGShaderPtr::GetRandom##ShaderName()  \
{                                                                                   \
    auto value = std::make_shared<RSNGRender##ShaderName>();                        \
    __VA_ARGS__                                                                     \
    return value;                                                                   \
}

#include "effect/rs_render_shader_def.in"

#undef SEPARATOR
#undef ADD_PROPERTY_TAG
#undef DECLARE_SHADER

} // namespace Rosen
} // namespace OHOS
