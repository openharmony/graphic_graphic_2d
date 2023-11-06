/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "webgl/webgl_arg.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "image_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;
using namespace OHOS::Media;
void WebGLImageOption::Dump()
{
    LOGD("ImageOption format 0x%{public}x type 0x%{public}x size [%{public}d, %{public}d]",
        format, type, width, height);
}

bool WebGLArg::GetWebGLArg(napi_value data, WebGLArgValue& arg, const WebGLArgInfo& func)
{
    bool succ = true;
    switch (func.argType) {
        case ARG_BOOL: {
            tie(succ, arg.glBool) = NVal(env_, data).ToBool();
            break;
        }
        case ARG_UINT: {
            tie(succ, arg.glUint) = NVal(env_, data).ToUint32();
            break;
        }
        case ARG_INT: {
            tie(succ, arg.glInt) = NVal(env_, data).ToInt32();
            break;
        }
        case ARG_FLOAT: {
            double v;
            tie(succ, v) = NVal(env_, data).ToDouble();
            arg.glFloat = v;
            break;
        }
        case ARG_ENUM: {
            tie(succ, arg.glEnum) = NVal(env_, data).ToGLenum();
            break;
        }
        case ARG_INTPTR: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.glIntptr = static_cast<GLintptr>(v);
            break;
        }
        case ARG_SIZE: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.glSizei = static_cast<GLsizei>(v);
            break;
        }
        case ARG_SIZEPTR: {
            int64_t v = 0;
            tie(succ, v) = NVal(env_, data).ToInt64();
            arg.glSizeptr = static_cast<GLsizei>(v);
            break;
        }
        default:
            break;
    }
    return succ;
}

std::tuple<GLenum, GLintptr> WebGLArg::ToGLintptr(napi_env env, napi_value dataObj)
{
    int64_t data = 0;
    bool succ;
    tie(succ, data) = NVal(env, dataObj).ToInt64();
    if (!succ) {
        return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0);
    }
    if (data < 0) {
        return make_tuple(WebGLRenderingContextBase::INVALID_VALUE, 0);
    }
    if (data > static_cast<int64_t>(std::numeric_limits<int>::max())) {
        return make_tuple(WebGLRenderingContextBase::INVALID_OPERATION, 0);
    }
    return make_tuple(WebGLRenderingContextBase::NO_ERROR, static_cast<GLintptr>(data));
}

template<class T>
void WebGLCommBuffer::DumpDataBuffer(const char* format) const
{
    LOGD("WebGL WebGLCommBuffer dataLen_ %{public}u ", dataLen_);
    size_t count = dataLen_ / sizeof(T);
    size_t i = 0;
    size_t max = count > MAX_DUMP ? MAX_DUMP : count;
    for (; i < max; i++) {
        LOGD(format, i, *reinterpret_cast<T*>(data_ + i * sizeof(T)));
    }

    if (count <= MAX_DUMP) {
        return;
    }
    if (count > MAX_DUMP) {
        i = count - MAX_DUMP;
    }
    for (; i < count; i++) {
        LOGD(format, i, *reinterpret_cast<T*>(data_ + i * sizeof(T)));
    }
}

void WebGLCommBuffer::DumpBuffer(BufferDataType destDataType) const
{
    switch (destDataType) {
        case BUFFER_DATA_BOOLEAN:
            DumpDataBuffer<bool>("WebGL WebGLCommBuffer bool elm %{public}u %{public}d");
            break;
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED:
            DumpDataBuffer<uint8_t>("WebGL WebGLCommBuffer int8 elm %{public}u %{public}d");
            break;
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16:
            DumpDataBuffer<uint16_t>("WebGL WebGLCommBuffer int16 elm %{public}u %{public}u");
            break;
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_UINT_32:
            DumpDataBuffer<uint32_t>("WebGL WebGLCommBuffer int32 elm %{public}u %{public}u");
            break;
        case BUFFER_DATA_FLOAT_32:
            DumpDataBuffer<float>("WebGL WebGLCommBuffer float elm %{public}u %{public}f");
            break;
        case BUFFER_DATA_FLOAT_64:
            DumpDataBuffer<double>("WebGL WebGLCommBuffer float64 elm %{public}u %{public}lf");
            break;
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64:
            DumpDataBuffer<uint64_t>("WebGL WebGLCommBuffer int64 elm %{public}u %{public}u");
            break;
        case BUFFER_DATA_GLENUM:
            DumpDataBuffer<GLenum>("WebGL WebGLCommBuffer glenum elm %{public}u %{public}x");
            break;
        default:
            break;
    }
}

size_t WebGLCommBuffer::GetBufferDataSize() const
{
    switch (type_) {
        case BUFFER_DATA_BOOLEAN:
            return sizeof(bool);
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED:
            return sizeof(uint8_t);
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16:
            return sizeof(uint16_t);
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_FLOAT_32:
        case BUFFER_DATA_UINT_32:
            return sizeof(uint32_t);
        case BUFFER_DATA_FLOAT_64:
            return sizeof(double);
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64:
            return sizeof(uint64_t);
        case BUFFER_DATA_GLENUM:
            return sizeof(GLenum);
        default:
            break;
    }
    return sizeof(uint32_t);
}

template<class srcT, class dstT>
napi_status WebGLReadBufferArg::GetArrayElement(
    napi_value data, dstT defaultValue, napi_status (*getElementValue)(napi_env env_, napi_value element, srcT* value))
{
    uint32_t length;
    napi_status status = napi_get_array_length(env_, data, &length);
    if (status != napi_ok) {
        return status;
    }
    LOGD("WebGL GetArrayData array len %{public}u ", length);
    if (length == 0) {
        return napi_ok;
    }
    buffer_.resize(length * sizeof(dstT));
    data_ = buffer_.data();
    uint8_t* buffer = buffer_.data();
    for (uint32_t i = 0; i < length; i++) {
        napi_value element;
        status = napi_get_element(env_, data, i, &element);
        if (status != napi_ok) {
            return status;
        }
        srcT elm = 0;
        if (NVal(env_, element).IsUndefined()) {
            *reinterpret_cast<dstT*>(buffer) = defaultValue;
            buffer += sizeof(dstT);
            continue;
        }

        if (NVal(env_, element).IsNull()) {
            *reinterpret_cast<dstT*>(buffer) = elm;
            buffer += sizeof(dstT);
            continue;
        }
        if ((status = getElementValue(env_, element, &elm)) != napi_ok) {
            return status;
        }
        *reinterpret_cast<dstT*>(buffer) = elm;
        buffer += sizeof(dstT);
    }
    dataLen_ = length * sizeof(dstT);
    return napi_ok;
}

napi_status WebGLReadBufferArg::GetArrayData(napi_value data, BufferDataType destDataType)
{
    napi_status status = napi_invalid_arg;
    switch (destDataType) {
        case BufferDataType::BUFFER_DATA_BOOLEAN:
            status = GetArrayElement<bool, bool>(data, false, napi_get_value_bool);
            break;
        case BufferDataType::BUFFER_DATA_INT_8:
        case BufferDataType::BUFFER_DATA_UINT_8:
        case BufferDataType::BUFFER_DATA_UINT_8_CLAMPED:
            status = GetArrayElement<uint32_t, uint8_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_INT_16:
        case BufferDataType::BUFFER_DATA_UINT_16:
            status = GetArrayElement<uint32_t, uint16_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_INT_32:
        case BufferDataType::BUFFER_DATA_UINT_32:
            status = GetArrayElement<uint32_t, uint32_t>(data, 0, napi_get_value_uint32);
            break;
        case BufferDataType::BUFFER_DATA_FLOAT_32: {
            float def = 0;
            (void)memset_s(&def, sizeof(def), 0xff, sizeof(def));
            status = GetArrayElement<double, float>(data, def, napi_get_value_double);
            break;
        }
        case BufferDataType::BUFFER_DATA_FLOAT_64:
            status = GetArrayElement<double, double>(data, 0, napi_get_value_double);
            break;
        case BufferDataType::BUFFER_DATA_BIGINT_64:
        case BufferDataType::BUFFER_DATA_BIGUINT_64:
            status = GetArrayElement<int64_t, uint64_t>(data, 0, napi_get_value_int64);
            break;
        case BufferDataType::BUFFER_DATA_GLENUM:
            status = GetArrayElement<int64_t, GLenum>(data, 0, napi_get_value_int64);
            break;
        default:
            break;
    }
    if (status != napi_ok) {
        return status;
    }
    type_ = destDataType;
    data_ = buffer_.data();
    return napi_ok;
}

// destDataType only for BUFFER_ARRAY
napi_status WebGLReadBufferArg::GenBufferData(napi_value data, BufferDataType destDataType)
{
    if (data == nullptr) {
        return napi_invalid_arg;
    }
    bool dataView, isArrayBuffer, typedArray, array = false;
    napi_status status = napi_invalid_arg;
    napi_is_dataview(env_, data, &dataView);
    napi_is_arraybuffer(env_, data, &isArrayBuffer);
    napi_is_typedarray(env_, data, &typedArray);
    napi_is_array(env_, data, &array);
    napi_value arrayBuffer = nullptr;
    size_t byteOffset = 0;
    if (dataView) {
        LOGD("WebGL WebGLReadBufferArg DataView");
        bufferType_ = BUFFER_DATA_VIEW;
        type_ = BUFFER_DATA_UINT_8;
        status = napi_get_dataview_info(
            env_, data, &dataLen_, reinterpret_cast<void**>(&data_), &arrayBuffer, &byteOffset);
    } else if (isArrayBuffer) {
        LOGD("WebGL WebGLReadBufferArg ArrayBuffer");
        bufferType_ = BUFFER_ARRAY_BUFFER;
        type_ = BUFFER_DATA_UINT_8;
        status = napi_get_arraybuffer_info(env_, data, reinterpret_cast<void**>(&data_), &dataLen_);
    } else if (typedArray) {
        LOGD("WebGL WebGLReadBufferArg TypedArray");
        bufferType_ = BUFFER_TYPED_ARRAY;
        napi_typedarray_type arrayType;
        status = napi_get_typedarray_info(
            env_, data, &arrayType, &dataLen_, reinterpret_cast<void**>(&data_), &arrayBuffer, &byteOffset);
        type_ = (BufferDataType)arrayType;
    } else if (array) {
        LOGD("WebGL WebGLReadBufferArg array ");
        bufferType_ = BUFFER_ARRAY;
        status = GetArrayData(data, destDataType);
    }
    LOGD("WebGL GenBufferData type %{public}u length %{public}zu bufferType %{public}u", type_, dataLen_, bufferType_);
    return status;
}

template<class dstT>
napi_value WebGLWriteBufferArg::GenExternalArray()
{
    LOGD("GenExternalArray dataLen_[%{public}u  %{public}u]", dataLen_, type_);
    napi_value outputBuffer = nullptr;
    size_t count = dataLen_ / sizeof(dstT);
    napi_value outputArray = nullptr;
    napi_status status = napi_ok;
    if (data_ == nullptr) {
        status = napi_create_arraybuffer(env_, dataLen_, reinterpret_cast<void **>(&data_), &outputBuffer);
    } else {
        status = napi_create_external_arraybuffer(
            env_, data_, dataLen_,
            [](napi_env env_, void* finalize_data, void* finalize_hint) {
                LOGD("GenExternalArray free %{public}p", finalize_data);
                delete[] reinterpret_cast<uint8_t*>(finalize_data);
            },
            nullptr, &outputBuffer);
    }
    if (status == napi_ok) {
        status = napi_create_typedarray(env_, (napi_typedarray_type)type_, count, outputBuffer, 0, &outputArray);
    }
    if (status != napi_ok) {
        LOGD("GenExternalArray error %{public}d", status);
    }
    data_ = nullptr;
    return outputArray;
}

napi_value WebGLWriteBufferArg::ToExternalArray(BufferDataType dstDataType)
{
    type_ = dstDataType;
    switch (dstDataType) {
        case BUFFER_DATA_BOOLEAN:
            return GenExternalArray<bool>();
        case BUFFER_DATA_INT_8:
        case BUFFER_DATA_UINT_8:
        case BUFFER_DATA_UINT_8_CLAMPED:
            return GenExternalArray<uint8_t>();
        case BUFFER_DATA_INT_16:
        case BUFFER_DATA_UINT_16:
            return GenExternalArray<uint16_t>();
        case BUFFER_DATA_INT_32:
        case BUFFER_DATA_UINT_32:
            return GenExternalArray<uint32_t>();
        case BUFFER_DATA_FLOAT_32:
            return GenExternalArray<float>();
        case BUFFER_DATA_FLOAT_64:
            return GenExternalArray<double>();
        case BUFFER_DATA_BIGINT_64:
        case BUFFER_DATA_BIGUINT_64:
            return GenExternalArray<uint64_t>();
        default:
            break;
    }
    return NVal::CreateNull(env_).val_;
}

template<class srcT, class dstT>
napi_value WebGLWriteBufferArg::GenNormalArray(
    napi_status (*getNApiValue)(napi_env env_, dstT value, napi_value* element))
{
    napi_value res = nullptr;
    size_t count = dataLen_ / sizeof(srcT);
    napi_create_array_with_length(env_, count, &res);
    for (size_t i = 0; i < count; i++) {
        srcT data = *reinterpret_cast<srcT*>(data_ + i * sizeof(srcT));
        napi_value element;
        getNApiValue(env_, static_cast<dstT>(data), &element);
        napi_set_element(env_, res, i, element);
    }
    return res;
}

napi_value WebGLWriteBufferArg::ToNormalArray(BufferDataType srcDataType, BufferDataType dstDataType)
{
    type_ = dstDataType;
    if (dstDataType == BUFFER_DATA_BOOLEAN) {
        if (srcDataType == BUFFER_DATA_BOOLEAN) {
            return GenNormalArray<bool, bool>(napi_get_boolean);
        } else if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return GenNormalArray<uint32_t, bool>(napi_get_boolean);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (dstDataType == BUFFER_DATA_INT_32 || dstDataType == BUFFER_DATA_UINT_32) {
        if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return GenNormalArray<uint32_t, uint32_t>(napi_create_uint32);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (dstDataType == BUFFER_DATA_BIGINT_64 || dstDataType == BUFFER_DATA_BIGUINT_64) {
        if (srcDataType == BUFFER_DATA_INT_32 || srcDataType == BUFFER_DATA_UINT_32) {
            return GenNormalArray<uint32_t, int64_t>(napi_create_int64);
        }
        return NVal::CreateNull(env_).val_;
    }

    if (srcDataType == BUFFER_DATA_FLOAT_32) {
        if (srcDataType == BUFFER_DATA_FLOAT_32) {
            return GenNormalArray<float, double>(napi_create_double);
        }
    }
    return NVal::CreateNull(env_).val_;
}

template<class T>
tuple<bool, T> WebGLImageSource::GetObjectIntField(napi_value resultObject, const std::string& name)
{
    napi_value result = nullptr;
    napi_status status = napi_get_named_property(env_, resultObject, name.c_str(), &result);
    if (status != napi_ok) {
        return make_tuple(false, 0);
    }
    int64_t res = 0;
    status = napi_get_value_int64(env_, result, &res);
    return make_tuple(status == napi_ok, static_cast<T>(res));
}

const WebGLFormatMap *WebGLImageSource::GetWebGLFormatMap(GLenum type, GLenum format)
{
    static const WebGLFormatMap formatTypeMaps[] = {
        {GL_RGBA, GL_UNSIGNED_BYTE, 4, 4, BUFFER_DATA_UINT_8, DECODE_RGBA_UBYTE},
        {GL_RGB, GL_UNSIGNED_BYTE, 3, 3, BUFFER_DATA_UINT_8, DECODE_RGB_UBYTE},
        {GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 4, 2, BUFFER_DATA_UINT_16, DECODE_RGBA_USHORT_4444},
        {GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 4, 2, BUFFER_DATA_UINT_16, DECODE_RGBA_USHORT_5551},
        {GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 3, 2, BUFFER_DATA_UINT_16, DECODE_RGB_USHORT_565},
        {GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 2, 2, BUFFER_DATA_UINT_8, DECODE_LUMINANCE_ALPHA_UBYTE},
        {GL_LUMINANCE, GL_UNSIGNED_BYTE, 1, 1, BUFFER_DATA_UINT_8, DECODE_LUMINANCE_UBYTE},
        {GL_ALPHA, GL_UNSIGNED_BYTE, 1, 1, BUFFER_DATA_UINT_8, DECODE_ALPHA_UBYTE},
    };
    for (size_t i = 0; i < sizeof(formatTypeMaps) / sizeof(formatTypeMaps[0]); ++i) {
        if (type == formatTypeMaps[i].type && format == formatTypeMaps[i].format) {
            return &formatTypeMaps[i];
        }
    }
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS