/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_HELPER_H
#define SKIA_HELPER_H

#include "include/core/SkFlattenable.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaHelper {
public:
    static std::shared_ptr<Data> FlattenableSerialize(const SkFlattenable* flattenable);

    template <typename T>
    static sk_sp<T> FlattenableDeserialize(std::shared_ptr<Data> data)
    {
        if (data == nullptr) {
            LOGD("SkiaHelper::FlattenableDeserialize, data is nullptr!");
            return nullptr;
        }

        SkReadBuffer reader(data->GetData(), data->GetSize());
        return reader.readFlattenable<T>();
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
