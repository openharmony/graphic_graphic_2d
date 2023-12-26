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

#ifndef RUNTIME_BLENDER_BUILDER_IMPL_H
#define RUNTIME_BLENDER_BUILDER_IMPL_H

#include "base_impl.h"

#include "effect/blender.h"
#include "effect/runtime_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeBlenderBuilderImpl : public BaseImpl {
public:
    RuntimeBlenderBuilderImpl() noexcept {}
    explicit RuntimeBlenderBuilderImpl(std::shared_ptr<RuntimeEffect> runtimeEffect) {}
    ~RuntimeBlenderBuilderImpl() override {}

    virtual std::shared_ptr<Blender> MakeBlender() = 0;

    virtual void SetUniform(const std::string& name, float val) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif