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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H

#include <memory>

#include <include/core/SkRect.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineRect {
public:
    TexgineRect();
    static TexgineRect MakeLTRB(float left, float top, float right, float bottom);
    static TexgineRect MakeXYWH(float x, float y, float w, float h);
    static TexgineRect MakeWH(float w, float h);
    std::shared_ptr<SkRect> GetRect() const;
    void SetRect(SkRect rect);

    float *fLeft_;
    float *fTop_;
    float *fRight_;
    float *fBottom_;

private:
    std::shared_ptr<SkRect> rect_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_RECT_H
