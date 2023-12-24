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

#include "effect/runtime_blender_builder.h"
#include "effect/runtime_effect.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RuntimeBlenderBuilder::RuntimeBlenderBuilder(std::shared_ptr<RuntimeEffect> runtimeEffect) noexcept
    : impl_(ImplFactory::CreateRuntimeBlenderBuilderImpl(runtimeEffect)) {}

std::shared_ptr<Blender> RuntimeBlenderBuilder::MakeBlender()
{
    return impl_->MakeBlender();
}

void RuntimeBlenderBuilder::SetUniform(const std::string& name, float val)
{
    impl_->SetUniform(name, val);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS