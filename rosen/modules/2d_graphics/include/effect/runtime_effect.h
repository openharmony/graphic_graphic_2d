/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RUNTIME_EFFECT_H
#define RUNTIME_EFFECT_H

#include "drawing/engine_adapter/impl_interface/runtime_effect_impl.h"

#include "effect/blender.h"
#include "effect/shader_effect.h"
#include "utils/data.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffectOptions {
public:
    bool forceNoInline = false;
    bool useAF = false;
};

class DRAWING_API RuntimeEffect {
public:
    /**
     * @brief Shader SL requires an entry point that looks like:
     * vec4 main(vec2 inCoords) { ... }
     * -or-
     * vec4 main(vec2 inCoords, vec4 inColor) { ... }
     * Most shaders don't use the input color, so that parameter is optional.
     *
     * @param sl 
     * @return A shared pointer to RuntimeEffect
     */
    static std::shared_ptr<RuntimeEffect> CreateForShader(const std::string& sl,
        const RuntimeEffectOptions&);
    static std::shared_ptr<RuntimeEffect> CreateForShader(const std::string& sl);
    static std::shared_ptr<RuntimeEffect> CreateForES3Shader(const std::string& sl);

    /**
     * @brief Blend SkSL requires an entry point that looks like:
     * vec4 main(vec4 srcColor, vec4 dstColor) { ... }
     *
     * @param sl 
     * @return A shared pointer to RuntimeEffect
     */
    static std::shared_ptr<RuntimeEffect> CreateForBlender(const std::string& sl);

    explicit RuntimeEffect(const std::string& sl) noexcept;
    RuntimeEffect(const std::string& sl, const RuntimeEffectOptions&) noexcept;
    RuntimeEffect(const std::string& sl, const RuntimeEffectOptions&, bool isES3) noexcept;
    RuntimeEffect(const std::string& sl, bool isBlender) noexcept;
    std::shared_ptr<ShaderEffect> MakeShader(std::shared_ptr<Data> uniforms,
                                             std::shared_ptr<ShaderEffect> children[],
                                             size_t childCount, const Matrix* localMatrix,
                                             bool isOpaque);
    virtual ~RuntimeEffect() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

protected:
    RuntimeEffect() noexcept;

private:
    std::shared_ptr<RuntimeEffectImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
