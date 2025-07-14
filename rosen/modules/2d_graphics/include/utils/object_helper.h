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

#ifndef OBJECT_HELPER_H
#define OBJECT_HELPER_H

#include <functional>
#include <map>
#include <shared_mutex>
#ifdef ROSEN_OHOS
#include <parcel.h>
#endif
#include "utils/object.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;

class DRAWING_API ObjectHelper {
public:
#ifdef ROSEN_OHOS
    using UnmarshallingFunc = std::function<std::shared_ptr<Object>(Parcel&, bool&, int32_t)>;
#endif

    static constexpr int32_t MAX_NESTING_DEPTH = 800;

    static ObjectHelper& Instance();

#ifdef ROSEN_OHOS
    bool Register(int32_t type, int32_t subType, UnmarshallingFunc func) {
        std::unique_lock<std::shared_mutex> lock(mapMutex_);
        uint64_t key = (static_cast<uint64_t>(type) << 32) | subType;
        funcMap_[key] = func;
        return true;
    }

    /**
     * @brief Get registered unmarshalling function for given type and subType
     * @param type Object type
     * @param subType Object subType
     * @return UnmarshallingFunc or nullptr if not found
     */
    UnmarshallingFunc GetFunc(int32_t type, int32_t subType) const {
        std::shared_lock<std::shared_mutex> lock(mapMutex_);
        uint64_t key = (static_cast<uint64_t>(type) << 32) | subType;
        auto it = funcMap_.find(key);
        return (it != funcMap_.end()) ? it->second : nullptr;
    }

    /**
     * @brief   Set external Data marshalling callback for advanced processing
     * @param callback Function to handle Data marshalling with additional safety checks
     */
    void SetDataMarshallingCallback(
        std::function<bool(Parcel&, std::shared_ptr<Data>)> callback) {
        dataMarshallingCallback_ = callback;
    }

    /**
     * @brief   Set external Data unmarshalling callback for advanced processing
     * @param callback Function to handle Data unmarshalling with additional safety checks
     */
    void SetDataUnmarshallingCallback(
        std::function<std::shared_ptr<Data>(Parcel&)> callback) {
        dataUnmarshallingCallback_ = callback;
    }

    std::function<bool(Parcel&, std::shared_ptr<Data>)> GetDataMarshallingCallback() const {
        return dataMarshallingCallback_;
    }

    std::function<std::shared_ptr<Data>(Parcel&)> GetDataUnmarshallingCallback() const {
        return dataUnmarshallingCallback_;
    }
#endif

private:
    ObjectHelper() = default;
    ~ObjectHelper() = default;
    ObjectHelper(const ObjectHelper& other) = delete;
    ObjectHelper(ObjectHelper&& other) = delete;
    ObjectHelper& operator=(const ObjectHelper& other) = delete;
    ObjectHelper& operator=(ObjectHelper&& other) = delete;
#ifdef ROSEN_OHOS
    mutable std::shared_mutex mapMutex_;
    std::map<uint64_t, UnmarshallingFunc> funcMap_;
    std::function<bool(Parcel&, std::shared_ptr<Data>)> dataMarshallingCallback_;
    std::function<std::shared_ptr<Data>(Parcel&)> dataUnmarshallingCallback_;
#endif
};

// Registration macro for Object-based classes
#define OBJECT_UNMARSHALLING_REGISTER(className, type, subType) \
    static bool isRegistered##className = \
        OHOS::Rosen::Drawing::ObjectHelper::Instance().Register( \
            type, subType, [](Parcel& parcel, bool& isValid, int32_t depth) \
                -> std::shared_ptr<OHOS::Rosen::Drawing::Object> { \
                auto obj = className::CreateForUnmarshalling(); \
                return obj->Unmarshalling(parcel, isValid, depth) ? obj : nullptr; \
            })

#ifdef ROSEN_OHOS
// Registration macro for Data callbacks - for use in specific cpp files
#define DATA_CALLBACKS_REGISTER(marshallingFunc, unmarshallingFunc) \
    static bool dataCallbacksRegistered = []() { \
        OHOS::Rosen::Drawing::ObjectHelper::Instance().SetDataMarshallingCallback(marshallingFunc); \
        OHOS::Rosen::Drawing::ObjectHelper::Instance().SetDataUnmarshallingCallback(unmarshallingFunc); \
        return true; \
    }()
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // OBJECT_HELPER_H