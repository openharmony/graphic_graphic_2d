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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_PROPERTY_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_PROPERTY_H

#include <type_traits>
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <string>
#include <functional>

#include "buffer_utils.h"
#include "message_parcel.h"
#include "graphic_common.h"
#include "rs_layer_cmd_type.h"
#include "rs_layer_common_def.h"
#include "common/rs_macros.h"
#include "pixel_map.h"
#include "platform/common/rs_log.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#include "sync_fence.h"
#endif

#include "surface_buffer.h"
#include "sync_fence.h"
#include "hdi_display_type.h"
#include "iconsumer_surface.h"

namespace OHOS {
namespace Rosen {
class RSRenderLayerPropertyBase : public std::enable_shared_from_this<RSRenderLayerPropertyBase> {
public:
    RSRenderLayerPropertyBase() = default;
    RSRenderLayerPropertyBase(const RSLayerPropertyId& id) : propertyId_(id) {}
    RSRenderLayerPropertyBase(const RSRenderLayerPropertyBase&) = delete;
    RSRenderLayerPropertyBase(const RSRenderLayerPropertyBase&&) = delete;
    RSRenderLayerPropertyBase& operator=(const RSRenderLayerPropertyBase&) = delete;
    RSRenderLayerPropertyBase& operator=(const RSRenderLayerPropertyBase&&) = delete;
    virtual ~RSRenderLayerPropertyBase() = default;

    void SetPropertyId(RSLayerPropertyId id)
    {
        propertyId_ = id;
    }

    RSLayerPropertyId GetPropertyId() const 
    {
        return propertyId_;
    }

    virtual void SetCmdType(RSLayerCmdType type) = 0;

protected:
    virtual const std::shared_ptr<RSRenderLayerPropertyBase> Clone() const
    {
        return nullptr;
    }

    RSLayerPropertyId propertyId_;

private:
    virtual std::shared_ptr<RSRenderLayerPropertyBase> Add(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSRenderLayerPropertyBase> Minus(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& value)
    {
        return shared_from_this();
    }

    virtual std::shared_ptr<RSRenderLayerPropertyBase> Multiply(const float scale)
    {
        return shared_from_this();
    }

    virtual bool IsEqual(const std::shared_ptr<const RSRenderLayerPropertyBase>& value) const
    {
        return true;
    }

    friend std::shared_ptr<RSRenderLayerPropertyBase> operator+=(
        const std::shared_ptr<RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
    friend std::shared_ptr<RSRenderLayerPropertyBase> operator-=(
        const std::shared_ptr<RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
    friend std::shared_ptr<RSRenderLayerPropertyBase> operator*=(
        const std::shared_ptr<RSRenderLayerPropertyBase>& a, const float scale);
    friend std::shared_ptr<RSRenderLayerPropertyBase> operator+(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
    friend std::shared_ptr<RSRenderLayerPropertyBase> operator-(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
    friend std::shared_ptr<RSRenderLayerPropertyBase> operator*(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& a, const float scale);
    friend bool operator==(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
    friend bool operator!=(
        const std::shared_ptr<const RSRenderLayerPropertyBase>& a,
        const std::shared_ptr<const RSRenderLayerPropertyBase>& b);
};

template<typename T>
class RSRenderLayerCmdProperty : public RSRenderLayerPropertyBase {
public:
    RSRenderLayerCmdProperty() : RSRenderLayerPropertyBase(0) {}
    RSRenderLayerCmdProperty(const T& value) : RSRenderLayerPropertyBase(0), stagingValue_(value) {}
    RSRenderLayerCmdProperty(const T& value, const RSLayerPropertyId& id) : RSRenderLayerPropertyBase(id),
        stagingValue_(value) {}
    ~RSRenderLayerCmdProperty() override = default;

    using ValueType = T;

    virtual void Set(const T& value)
    {
        stagingValue_ = value;
    }

    virtual T Get() const
    {
        return stagingValue_;
    }

    RSLayerCmdType GetCmdType() const
    {
        return cmdType_;
    }

    void SetCmdType(RSLayerCmdType type) override
    {
        cmdType_ = type;
    }

    template<typename U>
    bool OnMarshalling(OHOS::MessageParcel& parcel, const U& value)
    {
        if (cmdType_ == RSLayerCmdType::INVALID) {
            return false;
        }
        if (!parcel.WriteUint16(static_cast<uint16_t>(cmdType_))) {
            return false;
        }
        if (!parcel.WriteUint64(propertyId_)) {
            return false;
        }
        return MarshallingValue(parcel, value);
    }

    // base type
    template<typename U>
    typename std::enable_if<std::is_fundamental<U>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        return parcel.WriteUnpadBuffer(&value, sizeof(U));
    }

    // string type
    template<typename U>
    typename std::enable_if<std::is_same<U, std::string>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        return parcel.WriteString(value);
    }

    // vector type
    template<typename U>
    typename std::enable_if<std::is_same<U, std::vector<typename U::value_type>>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        constexpr uint32_t MAX_VECTOR_SIZE = 10000;
        if (value.size() > MAX_VECTOR_SIZE) {
            return false;
        }
        if (!parcel.WriteUint32(value.size())) {
            return false;
        }
        for (const auto& item : value) {
            if (!MarshallingValue(parcel, item)) {
                return false;
            }
        }
        return true;
    }

    // enum type
    template<typename U>
    typename std::enable_if<std::disjunction_v<std::is_same<U, GraphicLayerType>,
        std::is_same<U, GraphicTransformType>,
        std::is_same<U, GraphicBlendType>,
        std::is_same<U, GraphicColorDataSpace>,
        std::is_same<U, LayerMask>,
        std::is_same<U, GraphicLayerAlpha>,
        std::is_same<U, GraphicCompositionType>>, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        return parcel.WriteUnpadBuffer(&value, sizeof(U));
    }

    // struct type
    template<typename U>
    typename std::enable_if<std::disjunction_v<std::is_same<U, GraphicIRect>,
        std::is_same<U, GraphicLayerColor>,
        std::is_same<U, GraphicMatrix>,
        std::is_same<U, GraphicHDRMetaDataSet>,
        std::is_same<U, GraphicPresentTimestamp>,
        std::is_same<U, GraphicHDRMetaData>,
        std::is_same<U, GraphicSolidColorLayerProperty>>, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        return parcel.WriteUnpadBuffer(&value, sizeof(U));
    }

    // surface buffer
    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SurfaceBuffer>>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        if (value == nullptr) {
            parcel.WriteBool(false);
            return true;
        }
        parcel.WriteBool(true);
        GSError ret = value->WriteAllPropertiesToMessageParcel(parcel);
        if (ret != GSERROR_OK) {
            return false;
        }
        return true;
    }

    // sync fence
    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SyncFence>>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        if (value == nullptr) {
            return parcel.WriteBool(false);
        }
        if (!parcel.WriteBool(true)) {
            return false;
        }
        return value->WriteToMessageParcel(parcel);
    }

    template<typename U>
    bool OnUnmarshalling(OHOS::MessageParcel& parcel, std::shared_ptr<RSRenderLayerCmdProperty<U>>& val)
    {
        uint16_t typeValue;
        if (!parcel.ReadUint16(typeValue)) {
            return false;
        }
        RSLayerPropertyId id;
        if (!parcel.ReadUint64(id)) {
            return false;
        }
        U value;
        if (!UnmarshallingValue(parcel, value)) {
            return false;
        }
        val = std::make_shared<RSRenderLayerCmdProperty<U>>(value);
        val->SetPropertyId(id);
        val->SetCmdType(static_cast<RSLayerCmdType>(typeValue));
        return true;
    }

    // base type
    template<typename U>
    typename std::enable_if<std::is_fundamental<U>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(U))) {
            if (buff == nullptr) {
                return false;
            }
            value = *(reinterpret_cast<const U*>(buff));
            return true;
        }
        return false;
    }

    // string type
    template<typename U>
    typename std::enable_if<std::is_same<U, std::string>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        return parcel.ReadString(value);
    }

    // vector type
    template<typename U>
    typename std::enable_if<std::is_same<U, std::vector<typename U::value_type>>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        constexpr uint32_t MAX_VECTOR_SIZE = 10000;
        uint32_t size;
        if (!parcel.ReadUint32(size)) {
            return false;
        }
        if (size > MAX_VECTOR_SIZE) {
            return false;
        }
        value.clear();
        value.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            typename U::value_type item;
            if (!UnmarshallingValue(parcel, item)) {
                return false;
            }
            value.emplace_back(item);
        }
        return true;
    }

    // enum type
    template<typename U>
    typename std::enable_if<std::disjunction_v<std::is_same<U, GraphicLayerType>,
        std::is_same<U, GraphicTransformType>,
        std::is_same<U, GraphicBlendType>,
        std::is_same<U, GraphicColorDataSpace>,
        std::is_same<U, LayerMask>,
        std::is_same<U, GraphicLayerAlpha>,
        std::is_same<U, GraphicCompositionType>>, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(U))) {
            if (buff == nullptr) {
                return false;
            }
            value = *(reinterpret_cast<const U*>(buff));
            return true;
        }
        return false;
    }

    // struct type
    template<typename U>
    typename std::enable_if<std::disjunction_v<std::is_same<U, GraphicIRect>,
        std::is_same<U, GraphicLayerColor>,
        std::is_same<U, GraphicMatrix>,
        std::is_same<U, GraphicHDRMetaDataSet>,
        std::is_same<U, GraphicPresentTimestamp>,
        std::is_same<U, GraphicHDRMetaData>,
        std::is_same<U, GraphicSolidColorLayerProperty>>, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        if (const uint8_t* buff = parcel.ReadUnpadBuffer(sizeof(U))) {
            if (buff == nullptr) {
                return false;
            }
            value = *(reinterpret_cast<const U*>(buff));
            return true;
        }
        return false;
    }

    // surface buffer
    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SurfaceBuffer>>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        bool hasValue;
        if (!parcel.ReadBool(hasValue)) {
            return false;
        }
        if (!hasValue) {
            value = nullptr;
            return true;
        }
        sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
        auto readSafeFdFunc = [](OHOS::MessageParcel& parcel, std::function<int(OHOS::MessageParcel&)> readFdDefaultFunc) -> int {
            return parcel.ReadFileDescriptor();
        };
        GSError ret = buffer->ReadAllPropertiesFromMessageParcel(parcel, readSafeFdFunc);
        if (ret != GSERROR_OK) {
            return false;
        }
        value = buffer;
        return true;
    }

    // sync fence
    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SyncFence>>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        bool hasValue;
        if (!parcel.ReadBool(hasValue)) {
            return false;
        }
        if (!hasValue) {
            value = nullptr;
            return true;
        }
        OHOS::MessageParcel* msgParcel = static_cast<OHOS::MessageParcel*>(&parcel);
        if (msgParcel == nullptr) {
            return false;
        }
        auto readSafeFdFunc = [](OHOS::MessageParcel& parcel,
            std::function<int(OHOS::MessageParcel&)> readFdDefaultFunc) -> int {
            return parcel.ReadFileDescriptor();
        };
        value = SyncFence::ReadFromMessageParcel(*msgParcel, readSafeFdFunc);
        return value != nullptr;
    }

    // surface tunnel handle
    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SurfaceTunnelHandle>>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        if (value == nullptr) {
            return parcel.WriteBool(false);
        }
        if (!parcel.WriteBool(true)) {
            return false;
        }
        GSError ret = WriteExtDataHandle(parcel, value->GetHandle());
        if (ret != GSERROR_OK) {
            return false;
        }
        return true;
    }

    template<typename U>
    typename std::enable_if<std::is_same<U, sptr<SurfaceTunnelHandle>>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        bool hasValue;
        if (!parcel.ReadBool(hasValue)) {
            return false;
        }
        if (!hasValue) {
            value = nullptr;
            return true;
        }
        sptr<SurfaceTunnelHandle> handle = new SurfaceTunnelHandle();
        if (ReadExtDataHandle(parcel, handle) != GSERROR_OK) {
            return false;
        }
        value = handle;
        return true;
    }

    // std::shared_ptr<Media::PixelMap>
    template<typename U>
    typename std::enable_if<std::is_same<U, std::shared_ptr<Media::PixelMap>>::value, bool>::type
    MarshallingValue(OHOS::MessageParcel& parcel, const U& value)
    {
        if (value == nullptr) {
            return parcel.WriteBool(false);
        }
        if (!parcel.WriteBool(true)) {
            return false;
        }
        return parcel.WriteParcelable(value.get());
    }

    // std::shared_ptr<Media::PixelMap>
    template<typename U>
    typename std::enable_if<std::is_same<U, std::shared_ptr<Media::PixelMap>>::value, bool>::type
    UnmarshallingValue(OHOS::MessageParcel& parcel, U& value)
    {
        bool hasValue = false;
        if (!parcel.ReadBool(hasValue)) {
            return false;
        }
        if (!hasValue) {
            value = nullptr;
            return true;
        }
        value.reset(parcel.ReadParcelable<Media::PixelMap>());
        return true;
    }

protected:
    bool IsValid(const T& value)
    {
        return true;
    }

    RSLayerCmdType cmdType_ = RSLayerCmdType::INVALID;
    RSLayerPropertyId propertyId_ = 0;
    T stagingValue_ {};
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_RENDER_LAYER_CMD_PROPERTY_H
