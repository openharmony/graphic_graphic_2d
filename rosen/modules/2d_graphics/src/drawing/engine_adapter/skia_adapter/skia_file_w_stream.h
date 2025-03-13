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

#ifndef SKIA_FILE_W_STREAM_H
#define SKIA_FILE_W_STREAM_H

#include "impl_interface/file_w_stream_impl.h"

#include "include/core/SkStream.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class SkiaFileWStream : public FileWStreamImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    explicit SkiaFileWStream(const char path[]) noexcept;
    ~SkiaFileWStream() override
    {
        delete skFileWStream_;
    }

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    SkFILEWStream* GetSkFileWStream();
    bool IsValid() override;

private:
    SkFILEWStream* skFileWStream_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif