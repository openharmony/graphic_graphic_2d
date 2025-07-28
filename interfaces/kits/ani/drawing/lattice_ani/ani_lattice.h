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

#ifndef OHOS_ROSEN_ANI_LATTICE_H
#define OHOS_ROSEN_ANI_LATTICE_H

#include "ani_drawing_utils.h"
#include "draw/core_canvas.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniLattice final {
public:
    explicit AniLattice(std::shared_ptr<Lattice> lattice = nullptr) : m_lattice(lattice) {}
    ~AniLattice();

    static ani_status AniInit(ani_env *env);
    static ani_object CreateImageLattice(ani_env* env,
        ani_object obj, ani_object xDivs, ani_object yDivs, ani_int fXCount,
        ani_int fYCount, ani_object fBounds, ani_object fRectTypes, ani_object fColors);

    std::shared_ptr<Lattice> GetLattice();

private:
    std::shared_ptr<Lattice> m_lattice = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_LATTICE_H
