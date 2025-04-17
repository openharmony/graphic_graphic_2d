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

#ifndef SAFUZZ_RANDOM_RS_SHADER_H
#define SAFUZZ_RANDOM_RS_SHADER_H

#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {
class RandomRSShader {
public:
    static std::shared_ptr<RSShader> GetRandomRSShader();

private:
    static std::shared_ptr<RSShader> GetRandomRSDotMatrixShader();
    static std::shared_ptr<RSShader> GetRandomRSFlowLightSweepShader();
    static std::shared_ptr<RSShader> GetRandomRSDrawingShader();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_RS_SHADER_H
