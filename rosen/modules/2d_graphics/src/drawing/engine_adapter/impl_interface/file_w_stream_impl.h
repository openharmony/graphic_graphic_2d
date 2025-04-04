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

#ifndef FILE_W_STREAM_IMPL_H
#define FILE_W_STREAM_IMPL_H

#include "base_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class FileWStreamImpl : public BaseImpl {
public:
    FileWStreamImpl() noexcept {}
    explicit FileWStreamImpl(const char path[]) noexcept {}
    ~FileWStreamImpl() override = default;
    virtual bool IsValid() = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif