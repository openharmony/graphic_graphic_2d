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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H

namespace OHOS {
namespace Rosen {
#ifdef ENABLE_EXPORT_MACRO
#define RS_EXPORT __attribute__((visibility("default")))
#else
#define RS_EXPORT
#endif
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H