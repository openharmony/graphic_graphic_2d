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

#ifndef IMAGE_FILTER_OBJ_H
#define IMAGE_FILTER_OBJ_H

#include "utils/object.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class ImageFilterObj : public Object {
public:
    ImageFilterObj(int32_t subType) : Object(ObjectType::IMAGE_FILTER, subType) {}
    ~ImageFilterObj() override = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif  // IMAGE_FILTER_OBJ_H