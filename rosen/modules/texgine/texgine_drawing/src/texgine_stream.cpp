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

#include "texgine_stream.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
TexgineMemoryStream::TexgineMemoryStream(): memoryStream_(std::make_unique<RSMemoryStream>()) {}

TexgineMemoryStream::TexgineMemoryStream(std::unique_ptr<RSMemoryStream> stream): memoryStream_(std::move(stream)) {}

std::unique_ptr<RSMemoryStream> TexgineMemoryStream::GetStream()
{
    return std::move(memoryStream_);
}

std::unique_ptr<TexgineMemoryStream> TexgineMemoryStream::MakeCopy(const void *data, size_t length)
{
    return std::make_unique<TexgineMemoryStream>(std::make_unique<RSMemoryStream>(data, length, true));
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
