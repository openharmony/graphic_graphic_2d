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

#include "rs_layer_marshalling_helper.h"
#include "platform/common/rs_log.h"
#include "rs_render_layer_cmd.h"
#include "rs_render_layer_cmd_property.h"

namespace OHOS {
namespace Rosen {

#define MARSHALLING_AND_UNMARSHALLING(TYPE, TYPENAME)                                            \
    bool RSLayerMarshallingHelper::Marshalling(OHOS::MessageParcel& parcel, const TYPE& val)     \
    {                                                                                            \
        return parcel.Write##TYPENAME(val);                                                      \
    }                                                                                            \
    bool RSLayerMarshallingHelper::Unmarshalling(OHOS::MessageParcel& parcel, TYPE& val)         \
    {                                                                                            \
        return parcel.Read##TYPENAME(val);                                                       \
    }

MARSHALLING_AND_UNMARSHALLING(bool, Bool)
MARSHALLING_AND_UNMARSHALLING(int8_t, Int8)
MARSHALLING_AND_UNMARSHALLING(uint8_t, Uint8)
MARSHALLING_AND_UNMARSHALLING(int16_t, Int16)
MARSHALLING_AND_UNMARSHALLING(uint16_t, Uint16)
MARSHALLING_AND_UNMARSHALLING(int32_t, Int32)
MARSHALLING_AND_UNMARSHALLING(uint32_t, Uint32)
MARSHALLING_AND_UNMARSHALLING(int64_t, Int64)
MARSHALLING_AND_UNMARSHALLING(uint64_t, Uint64)
MARSHALLING_AND_UNMARSHALLING(float, Float)
MARSHALLING_AND_UNMARSHALLING(double, Double)
#undef MARSHALLING_AND_UNMARSHALLING

bool RSLayerMarshallingHelper::Marshalling(OHOS::MessageParcel& parcel, const std::shared_ptr<RSRenderLayerCmd>& val)
{
    return val != nullptr && val->Marshalling(parcel);
}

bool RSLayerMarshallingHelper::Unmarshalling(OHOS::MessageParcel& parcel, std::shared_ptr<RSRenderLayerCmd>& val)
{
    val = RSRenderLayerCmd::Unmarshalling(parcel);
    if (val == nullptr) {
        RS_LOGE("RSLayerMarshallingHelper::UnMarshalling Unmarshalling failed.");
        return false;
    }
    return true;
}

bool RSLayerMarshallingHelper::Marshalling(OHOS::MessageParcel& parcel, const RSLayerCmdType& val)
{
    return parcel.WriteUint16(static_cast<uint16_t>(val));
}

bool RSLayerMarshallingHelper::Unmarshalling(OHOS::MessageParcel& parcel, RSLayerCmdType& val)
{
    uint16_t data = 0;
    if (!parcel.ReadUint16(data)) {
        RS_LOGE("RSLayerMarshallingHelper::UnMarshalling ReadUint16 failed.");
        return false;
    }
    val = static_cast<RSLayerCmdType>(data);
    return true;
}
} // namespace Rosen
} // namespace OHOS