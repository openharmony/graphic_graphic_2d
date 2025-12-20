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

#ifndef SAFUZZ_RANDOM_RS_NG_SHAPE_H
#define SAFUZZ_RANDOM_RS_NG_SHAPE_H

#include "effect/rs_render_shape_base.h"

namespace OHOS {
namespace Rosen {
class RandomRSNGShapePtr {
public:
    static std::shared_ptr<RSNGRenderShapeBase> GetRandomValue();
    static void ResetShapeDepth();

private:
    static std::shared_ptr<RSNGRenderShapeBase> GetRandomSingleShape();
    static std::shared_ptr<RSNGRenderShapeBase> GetRandomShapeChain();
    static std::shared_ptr<RSNGRenderShapeBase> GetNullValue();

#define DECLARE_SHAPE(ShapeName, ShapeType, ...) \
    static std::shared_ptr<RSNGRenderShapeBase> GetRandom##ShapeName()

#include "effect/rs_render_shape_def.in"

#undef DECLARE_SHAPE
};

} // namespace Rosen
} // namespace OHOS
#endif // SAFUZZ_RANDOM_RS_NG_SHAPE_H
