/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef CJ_FILTER_H
#define CJ_FILTER_H

#include "cj_common_ffi.h"
#include "cj_ui_effect_log.h"
#include "ffi_remote_data.h"
#include "filter/include/filter.h"
#include "filter/include/filter_blur_para.h"
#include "filter/include/filter_color_gradient_para.h"
#include "filter/include/filter_dispersion_para.h"
#include "filter/include/filter_radius_gradient_blur_para.h"

namespace OHOS {
namespace Rosen {
class CJFilter : public OHOS::FFI::FFIData {
    DECL_TYPE(CJFilter, OHOS::FFI::FFIData)
public:
    explicit CJFilter();
    virtual ~CJFilter() override = default;
    void SetBlur(float blur, int32_t* errCode);

private:
    std::shared_ptr<Filter> m_FilterObj = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif
