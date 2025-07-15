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

#ifndef DRAWING_OBJECT_H
#define DRAWING_OBJECT_H

#include <functional>
#include <memory>
#ifdef ROSEN_OHOS
#include <parcel.h>
#endif
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
class DRAWING_API Object {
public:
    enum class ObjectType : int32_t {
        NO_TYPE,
        SHADER_EFFECT,
        IMAGE_FILTER
    };
    Object() : type_(static_cast<int32_t>(ObjectType::NO_TYPE)), subType_(0) {}
    explicit Object(ObjectType type, int32_t subType = 0)
        : type_(static_cast<int32_t>(type)), subType_(subType) {}
    virtual ~Object() = default;
#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel&) = 0;
    virtual bool Unmarshalling(Parcel&, bool& isValid, int32_t depth = 0) = 0;
#endif
    virtual std::shared_ptr<void> GenerateBaseObject() = 0;
    int32_t GetType() const { return type_; }
    int32_t GetSubType() const { return subType_; }
protected:
    int32_t type_ = 0;
    int32_t subType_ = 0;
};

/**
 * @brief InvalidObj class for handling failed deserialization
 * Used as placeholder when object deserialization fails but data structure integrity needs to be maintained
 */
class DRAWING_API InvalidObj final : public Object {
public:
    InvalidObj() : Object(ObjectType::NO_TYPE) {}

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override {
        // Invalid object should not be marshalled
        return false;
    }

    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override {
        // Invalid object should not be unmarshalled
        return false;
    }
#endif

    std::shared_ptr<void> GenerateBaseObject() override {
        // Invalid object generates nullptr to prevent usage of invalid data
        return nullptr;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif  // DRAWING_OBJECT_H