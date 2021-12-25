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

#include "buffer_extra_data_impl.h"

#include <message_parcel.h>

#include "buffer_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferExtraDataImpl" };
constexpr int32_t BUFFER_EXTRA_DATA_MAGIC = 0x4567;
} // namespace

GSError BufferExtraDataImpl::ReadFromParcel(MessageParcel &parcel)
{
    int32_t magic;
    if (parcel.ReadInt32(magic) == false || magic != BUFFER_EXTRA_DATA_MAGIC) {
        BLOGW("read failed, magic is error");
        return GSERROR_INTERNEL;
    }

    int32_t size = parcel.ReadInt32();
    BLOGD("read %{public}d", size);
    for (int32_t i = 0; i < size; i++) {
        auto key = parcel.ReadString();
        auto type = static_cast<ExtraDataType>(parcel.ReadInt32());
        switch (type) {
            case ExtraDataType::i32: {
                ExtraSet(key, type, parcel.ReadInt32());
                break;
            }
            case ExtraDataType::i64: {
                ExtraSet(key, type, parcel.ReadInt64());
                break;
            }
            case ExtraDataType::f64: {
                ExtraSet(key, type, parcel.ReadDouble());
                break;
            }
            case ExtraDataType::string: {
                ExtraSet(key, type, parcel.ReadString());
                break;
            }
            default: break;
        }
    }
    return GSERROR_OK;
}

GSError BufferExtraDataImpl::WriteToParcel(MessageParcel &parcel)
{
    parcel.WriteInt32(BUFFER_EXTRA_DATA_MAGIC);
    parcel.WriteInt32(datas.size());
    BLOGD("write " BPUBSize, datas.size());
    for (const auto &[key, data] : datas) {
        parcel.WriteString(key);
        parcel.WriteInt32(static_cast<int32_t>(data.type));
        switch (data.type) {
            case ExtraDataType::i32: {
                int32_t i32 = -1;
                auto pVal = std::any_cast<int32_t>(&data.val);
                if (pVal != nullptr) {
                    i32 = *pVal;
                }
                parcel.WriteInt32(i32);
                break;
            }
            case ExtraDataType::i64: {
                int64_t i64 = -1;
                auto pVal = std::any_cast<int64_t>(&data.val);
                if (pVal != nullptr) {
                    i64 = *pVal;
                }
                parcel.WriteInt64(i64);
                break;
            }
            case ExtraDataType::f64: {
                double f64 = -1;
                auto pVal = std::any_cast<double>(&data.val);
                if (pVal != nullptr) {
                    f64 = *pVal;
                }
                parcel.WriteDouble(f64);
                break;
            }
            case ExtraDataType::string: {
                std::string string = "-1";
                auto pVal = std::any_cast<std::string>(&data.val);
                if (pVal != nullptr) {
                    string = *pVal;
                }
                parcel.WriteString(string);
                break;
            }
            default:
                break;
        }
    }
    return GSERROR_OK;
}

GSError BufferExtraDataImpl::ExtraGet(std::string &key, int32_t &value) const
{
    return ExtraGet<int32_t>(key, ExtraDataType::i32, value);
}

GSError BufferExtraDataImpl::ExtraGet(std::string &key, int64_t &value) const
{
    return ExtraGet<int64_t>(key, ExtraDataType::i64, value);
}

GSError BufferExtraDataImpl::ExtraGet(std::string &key, double &value) const
{
    return ExtraGet<double>(key, ExtraDataType::f64, value);
}

GSError BufferExtraDataImpl::ExtraGet(std::string &key, std::string &value) const
{
    return ExtraGet<std::string>(key, ExtraDataType::string, value);
}

GSError BufferExtraDataImpl::ExtraSet(std::string &key, int32_t value)
{
    return ExtraSet(key, ExtraDataType::i32, value);
}

GSError BufferExtraDataImpl::ExtraSet(std::string &key, int64_t value)
{
    return ExtraSet(key, ExtraDataType::i64, value);
}

GSError BufferExtraDataImpl::ExtraSet(std::string &key, double value)
{
    return ExtraSet(key, ExtraDataType::f64, value);
}

GSError BufferExtraDataImpl::ExtraSet(std::string &key, std::string value)
{
    return ExtraSet(key, ExtraDataType::string, value);
}

template<class T>
GSError BufferExtraDataImpl::ExtraGet(std::string &key, ExtraDataType type, T &value) const
{
    auto it = datas.find(key);
    if (it == datas.end()) {
        return GSERROR_NO_ENTRY;
    }
    if (it->second.type != type) {
        return GSERROR_TYPE_ERROR;
    }
    auto pVal = std::any_cast<T>(&it->second.val);
    if (pVal == nullptr) {
        return GSERROR_TYPE_ERROR;
    }
    value = *pVal;
    return GSERROR_OK;
}

GSError BufferExtraDataImpl::ExtraSet(std::string &key, ExtraDataType type, std::any val)
{
    auto it = datas.find(key);
    if (it == datas.end() && datas.size() > SURFACE_MAX_USER_DATA_COUNT) {
        BLOGW("SurfaceBuffer has too many extra data, cannot save one more!!!");
        return GSERROR_OUT_OF_RANGE;
    }
    datas[key].type = type;
    datas[key].val = val;
    return GSERROR_OK;
}
} // namespace OHOS
