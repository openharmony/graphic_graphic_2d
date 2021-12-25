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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_EXTRA_DATA_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_EXTRA_DATA_IMPL_H

#include <buffer_extra_data.h>

#include <any>

namespace OHOS {
class BufferExtraDataImpl : public BufferExtraData {
public:
    virtual GSError ReadFromParcel(MessageParcel &parcel) override;
    virtual GSError WriteToParcel(MessageParcel &parcel) override;
    virtual GSError ExtraGet(std::string &key, int32_t &value) const override;
    virtual GSError ExtraGet(std::string &key, int64_t &value) const override;
    virtual GSError ExtraGet(std::string &key, double &value) const override;
    virtual GSError ExtraGet(std::string &key, std::string &value) const override;
    virtual GSError ExtraSet(std::string &key, int32_t value) override;
    virtual GSError ExtraSet(std::string &key, int64_t value) override;
    virtual GSError ExtraSet(std::string &key, double value) override;
    virtual GSError ExtraSet(std::string &key, std::string value) override;

private:
    enum class ExtraDataType : int32_t {
        i32,
        i64,
        f64,
        string,
    };
    template<class T>
    GSError ExtraGet(std::string &key, ExtraDataType type, T &value) const;
    GSError ExtraSet(std::string &key, ExtraDataType type, std::any val);

    struct ExtraData {
        std::any val;
        ExtraDataType type;
    };
    std::map<std::string, struct ExtraData> datas;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_EXTRA_DATA_IMPL_H
