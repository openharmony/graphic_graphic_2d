/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_CJ_FILTER_H
#define OHOS_CJ_FILTER_H

#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "include/core/SkImageFilter.h"
#include "pixel_map.h"
#include "pixel_map_impl.h"

namespace OHOS {
namespace CJEffectKit {
class FFI_EXPORT CJFilter : public OHOS::FFI::FFIData {
    DECL_TYPE(CJFilter, OHOS::FFI::FFIData)
public:
    static int64_t CreateEffect(Media::PixelMapImpl* source, uint32_t& errorCode);
    void Blur(float radius);
    void Invert();
    void Brightness(float bright);
    void Grayscale();
    void SetColorMatrix(std::vector<float> cjcolorMatrix, uint32_t& code);
    static std::shared_ptr<OHOS::Media::PixelMap> GetEffectPixelMap();
};
} // namespace CJEffectKit
} // namespace OHOS

#endif // OHOS_CJ_FILTER_H