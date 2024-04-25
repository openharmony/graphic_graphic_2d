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

#ifndef BLENDER_H
#define BLENDER_H

#include "include/core/SkBlender.h"

#include "drawing/engine_adapter/impl_interface/blender_impl.h"
#include "utils/drawing_macros.h"
#include "draw/blend_mode.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Blender {
public:
    Blender() noexcept;
    virtual ~Blender() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

    void SetSkBlender(sk_sp<SkBlender> blender);

    static std::shared_ptr<Blender> CreateWithBlendMode(BlendMode mode);

private:
    std::shared_ptr<BlenderImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
