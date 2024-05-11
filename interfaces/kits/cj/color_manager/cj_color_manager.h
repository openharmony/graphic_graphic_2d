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

#ifndef OHOS_CJ_COLOR_MANAGER_H
#define OHOS_CJ_COLOR_MANAGER_H

#include "cj_color_mgr_utils.h"
#include "color_space.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace ColorManager {
class CjColorManager : public OHOS::FFI::FFIData {
public:
    explicit CjColorManager(std::shared_ptr<ColorSpace> ptr);
    virtual ~CjColorManager()
    {
        colorSpaceToken_ = nullptr;
    }
    static std::tuple<int32_t, std::string, std::shared_ptr<ColorSpace>> create(ApiColorSpaceType colorSpaceName);
    static std::shared_ptr<ColorSpace> create(ColorSpacePrimaries primaries, float gamma, int32_t* errCode);
    inline const std::shared_ptr<ColorSpace>& GetColorSpaceToken() const
    {
        return colorSpaceToken_;
    }
    
    uint32_t GetColorSpaceName(int32_t* errCode);
    std::array<float, DIMES_2> GetWhitePoint(int32_t* errCode);
    float GetGamma(int32_t* errCode);
    OHOS::FFI::RuntimeType *GetRuntimeType() override { return GetClassType(); }
private:
    friend class OHOS::FFI::RuntimeType;
    friend class OHOS::FFI::TypeBase;
    static OHOS::FFI::RuntimeType *GetClassType()
    {
        static OHOS::FFI::RuntimeType runtimeType =
            OHOS::FFI::RuntimeType::Create<OHOS::FFI::FFIData>("CjColorManager");
        return &runtimeType;
    }
    std::shared_ptr<ColorSpace> colorSpaceToken_;
};
} // namespace ColorManager
} // namespace OHOS

#endif // OHOS_CJ_COLOR_MANAGER_H