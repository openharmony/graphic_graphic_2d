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

#ifndef UTILS_DRAWING_MACROS_H
#define UTILS_DRAWING_MACROS_H

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifndef DRAWING_API
#ifdef _WIN32
#define DRAWING_EXPORT __attribute__((dllexport))
#define DRAWING_IMPORT __attribute__((dllimport))
#else
#define DRAWING_EXPORT __attribute__((visibility("default")))
#define DRAWING_IMPORT __attribute__((visibility("default")))
#endif
#ifdef MODULE_DRAWING
#define DRAWING_API DRAWING_EXPORT
#else
#define DRAWING_API DRAWING_IMPORT
#endif
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
