/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H

#ifdef ROSEN_OHOS

#include <parcel.h>

#include "common/rs_common_def.h"

template<typename T>
class sk_sp;
class SkData;
class SkFlattenable;
class SkPath;

namespace OHOS {
namespace Rosen {
class RSPath;
class RSShader;
template<typename T>
class RSRenderCurveAnimation;
template<typename T>
class RSRenderKeyframeAnimation;
class RSRenderTransition;
class RSRenderPathAnimation;

class RSMarshallingHelper {
public:
    // default marshalling and unmarshalling method for POD types
    // TODO: implement marshalling & unmarshalling methods for other types (e.g. RSImage, drawCMDList)
    template<typename T>
    static bool Marshalling(Parcel& parcel, const T& val)
    {
        return parcel.WriteUnpadBuffer(&val, sizeof(T));
    }
    template<typename T>
    static bool Unmarshalling(Parcel& parcel, T& val)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(T))) {
            val = *(reinterpret_cast<const T*>(buff));
            return true;
        }
        return false;
    }

    // specialized marshalling & unmarshalling function for certain types
#define DECLARE_FUNCTION_OVERLOAD(TYPE)                       \
    static bool Marshalling(Parcel& parcel, const TYPE& val); \
    static bool Unmarshalling(Parcel& parcel, TYPE& val);

    DECLARE_FUNCTION_OVERLOAD(bool)
    DECLARE_FUNCTION_OVERLOAD(int8_t)
    DECLARE_FUNCTION_OVERLOAD(uint8_t)
    DECLARE_FUNCTION_OVERLOAD(int16_t)
    DECLARE_FUNCTION_OVERLOAD(uint16_t)
    DECLARE_FUNCTION_OVERLOAD(int32_t)
    DECLARE_FUNCTION_OVERLOAD(uint32_t)
    DECLARE_FUNCTION_OVERLOAD(int64_t)
    DECLARE_FUNCTION_OVERLOAD(uint64_t)
    DECLARE_FUNCTION_OVERLOAD(float)
    DECLARE_FUNCTION_OVERLOAD(double)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkData>)
    DECLARE_FUNCTION_OVERLOAD(sk_sp<SkFlattenable>)
    DECLARE_FUNCTION_OVERLOAD(SkPath)
    DECLARE_FUNCTION_OVERLOAD(RSShader)
    DECLARE_FUNCTION_OVERLOAD(RSPath)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderPathAnimation>)
    DECLARE_FUNCTION_OVERLOAD(std::shared_ptr<RSRenderTransition>)
#undef DECLARE_FUNCTION_OVERLOAD

#define DECLARE_TEMPLATE_OVERLOAD(TEMPLATE)                                           \
    template<typename T>                                                              \
    static bool Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val); \
    template<typename T>                                                              \
    static bool Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val);

    DECLARE_TEMPLATE_OVERLOAD(RSRenderCurveAnimation)
    DECLARE_TEMPLATE_OVERLOAD(RSRenderKeyframeAnimation)
#undef DECLARE_TEMPLATE_OVERLOAD
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_OHOS
#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_MARSHALLING_HELPER_H
