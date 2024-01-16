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

#ifndef MEMORY_STREAM_H
#define MEMORY_STREAM_H

#include <memory>
#include <cstdint>

#include "impl_interface/memory_stream_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API MemoryStream {
public:
    MemoryStream();
    MemoryStream(const void* data, size_t length, bool copyData = false);
    virtual ~MemoryStream() = default;

    template<typename T>
    T* GetImpl() const
    {
        return memoryStreamImpl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<MemoryStreamImpl> memoryStreamImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif