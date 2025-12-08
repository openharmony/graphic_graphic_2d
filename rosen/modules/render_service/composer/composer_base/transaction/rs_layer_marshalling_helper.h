/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_MARSHALLING_HELPER_H
#define RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_MARSHALLING_HELPER_H

#include <map>
#include <memory>
#include <optional>
#include <thread>
#include "hdi_display_type.h"
#include "iconsumer_surface.h"
#include "rs_render_layer_cmd.h"
#include "surface_type.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSLayerMarshallingHelper {
public:
    static bool Marshalling(OHOS::MessageParcel& parcel)
    {
        return true;
    }
    static bool Unmarshalling(OHOS::MessageParcel& parcel)
    {
        return true;
    }

// base marshalling & unmarshalling
#define DECLARE_MARSHALLING_BASIC(TYPE)                                                \
    static bool Marshalling(OHOS::MessageParcel& parcel, const TYPE& val);             \
    static bool Unmarshalling(OHOS::MessageParcel& parcel, TYPE& val);

    // base types
    DECLARE_MARSHALLING_BASIC(bool)
    DECLARE_MARSHALLING_BASIC(int8_t)
    DECLARE_MARSHALLING_BASIC(uint8_t)
    DECLARE_MARSHALLING_BASIC(int16_t)
    DECLARE_MARSHALLING_BASIC(uint16_t)
    DECLARE_MARSHALLING_BASIC(int32_t)
    DECLARE_MARSHALLING_BASIC(uint32_t)
    DECLARE_MARSHALLING_BASIC(int64_t)
    DECLARE_MARSHALLING_BASIC(uint64_t)
    DECLARE_MARSHALLING_BASIC(float)
    DECLARE_MARSHALLING_BASIC(double)

    // rs layer command
    DECLARE_MARSHALLING_BASIC(std::shared_ptr<RSRenderLayerCmd>)
    DECLARE_MARSHALLING_BASIC(RSLayerCmdType)
#undef DECLARE_MARSHALLING_BASIC

// cmdtype marshalling & unmarshalling
#define DECLARE_RSLAYER_CMD(CMD_NAME, TYPE, CMD_TYPE, DELTA_OP, THRESHOLD_TYPE)              \
    static bool Marshalling##CMD_NAME(OHOS::MessageParcel& parcel,                           \
        std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> val);                                \
    static bool Unmarshalling##CMD_NAME(OHOS::MessageParcel& parcel,                         \
        std::shared_ptr<RSRenderLayerCmdProperty<TYPE>> val);                                \

#include "rs_layer_cmd_def.in"
#undef DECLARE_RSLAYER_CMD

    template<typename T>
    typename std::enable_if<std::is_same<T, RSLayerId>::value, bool>::type
    static Marshalling(OHOS::MessageParcel& parcel, const T& value)
    {
        return parcel.WriteUint64(value);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, RSRenderLayerCmd>::value, bool>::type
    static Marshalling(OHOS::MessageParcel& parcel, const T& value)
    {
        return value != nullptr && value->Marshalling(parcel);
    }

    template<typename T, typename... Args>
    static bool Marshalling(OHOS::MessageParcel& parcel, const T& first, const Args&... args)
    {
        return Marshalling(parcel, first) && Marshalling(parcel, args...);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, RSLayerId>::value, bool>::type
    static Unmarshalling(OHOS::MessageParcel& parcel, const T& value)
    {
        return parcel.ReadUint64(value);
    }

    template<typename T>
    typename std::enable_if<std::is_same<T, RSRenderLayerCmd>::value, bool>::type
    static Unmarshalling(OHOS::MessageParcel& parcel, const T& value)
    {
        return value != nullptr && value->Unmarshalling(parcel);
    }

    template<typename T, typename... Args>
    static bool Unmarshalling(OHOS::MessageParcel& parcel, T& first, Args&... args)
    {
        return Unmarshalling(parcel, first) && Unmarshalling(parcel, args...);
    }
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_TRANSACTION_RS_LAYER_MARSHALLING_HELPER_H