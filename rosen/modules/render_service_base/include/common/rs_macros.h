/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
/** about export template (most at _WIN32 platform)
 *  Sometimes other module maybe use instantiated template classes or instantiate their own template class.
 *  for example: arkui will use RSRenderAnimatableProperty<float> and instantiate the template
 *      with type "NG::ColorBlend", so we need export instantiated template classes but cannot export
 *      template itself. Because if you RSB_EXPORT the template, arkui will think the symbol
 *      RSRenderAnimatableProperty<NG::ColorBlend> is importing from other module,
 *      linker then raise a link problem.
 *
 *  So we should use RSB_EXPORT_TMP and use extern template & template to tell arkui that there are many
 *      instantiated template classes, you don't need instantiate again.
 *
 *  correct way:
 *      base view:
 *          EXPORT RSRenderAnimatableProperty<T>
 *          extern template class RSRenderAnimatableProperty<float>;
 *          (in cpp) template class RSRenderAnimatableProperty<float>;
 *      arkui view:
 *                 RSRenderAnimatableProperty<T>
 *          extern template class RSRenderAnimatableProperty<float>;
 */

#ifdef _WIN32
#define RS_EXPORT __attribute__((dllexport))
#define RS_IMPORT __attribute__((dllimport))
#else
#define RS_EXPORT __attribute__((visibility("default")))
#define RS_IMPORT __attribute__((visibility("default")))
#endif

#ifdef MODULE_RSB
#define RSB_EXPORT      RS_EXPORT
#define RSB_EXPORT_TMP  RS_EXPORT
#else
#define RSB_EXPORT      RS_IMPORT
#define RSB_EXPORT_TMP
#endif

#ifdef MODULE_RSC
#define RSC_EXPORT      RS_EXPORT
#define RSC_EXPORT_TMP  RS_EXPORT
#else
#define RSC_EXPORT      RS_IMPORT
#define RSC_EXPORT_TMP
#endif

#ifdef __gnu_linux__
#define __libcpp_erase_if_container __detail::__erase_nodes_if
#endif
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H
