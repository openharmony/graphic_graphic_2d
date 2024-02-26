
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

#ifndef ADAPTER_TYPE_H
#define ADAPTER_TYPE_H

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class AdapterType {
    SKIA_ADAPTER,
    DDGR_ADAPTER,
    OTHER_ADAPTER,
};

enum class DrawingType {
    COMMON,
    RECORDING,
    OVER_DRAW,
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
