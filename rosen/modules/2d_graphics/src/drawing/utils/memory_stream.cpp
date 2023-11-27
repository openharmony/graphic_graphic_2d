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

#include "utils/memory_stream.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
MemoryStream::MemoryStream() : memoryStreamImpl_(ImplFactory::CreateMemoryStreamImpl()) {}

MemoryStream::MemoryStream(const void* data, size_t length, bool copyData)
    : memoryStreamImpl_(ImplFactory::CreateMemoryStreamImpl(data, length, copyData)) {}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
