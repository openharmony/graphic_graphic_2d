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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DATA_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DATA_H

#include <memory>

#include "drawing.h"

#include "texgine_canvas.h"
#include "texgine_picture.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineData {
public:
    /*
     * @brief Return the instance of SkData
     */
    std::shared_ptr<RSData> GetData() const;

    /*
     * @brief Set SkData to TexgineData
     */
    void SetData(const std::shared_ptr<RSData> &data);
    static std::shared_ptr<TexgineData> MakeFromFileName(const std::string &path);

private:
    std::shared_ptr<RSData> data_ = nullptr;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_DATA_H
