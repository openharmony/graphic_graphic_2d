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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MEMORY_STREAM_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MEMORY_STREAM_H

#include <memory>

#include <include/core/SkStream.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineMemoryStream {
public:
    TexgineMemoryStream();
    TexgineMemoryStream(std::unique_ptr<SkMemoryStream> stream);
    std::unique_ptr<SkMemoryStream> GetStream();
    static std::unique_ptr<TexgineMemoryStream> MakeCopy(const void *data, size_t length);

private:
    std::unique_ptr<SkMemoryStream> memoryStream_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_MEMORY_STREAM_H
