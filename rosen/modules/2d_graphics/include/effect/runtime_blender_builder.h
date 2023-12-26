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

#ifndef RUNTIME_BLENDER_BUILDER_H
#define RUNTIME_BLENDER_BUILDER_H

#include "drawing/engine_adapter/impl_interface/runtime_blender_builder_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffect;

class DRAWING_API RuntimeBlenderBuilder {
public:
    RuntimeBlenderBuilder(std::shared_ptr<RuntimeEffect> runtimeEffect) noexcept;
    virtual ~RuntimeBlenderBuilder() = default;
    
    std::shared_ptr<Blender> MakeBlender();

    void SetUniform(const std::string& name, float val);

private:
    std::shared_ptr<RuntimeBlenderBuilderImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
