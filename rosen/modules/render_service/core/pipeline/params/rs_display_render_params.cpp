/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pipeline/params/rs_display_render_params.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

#include "src/image/SkImage_Base.h"

namespace OHOS::Rosen {
RSDisplayRenderParams::RSDisplayRenderParams(Drawing::Matrix matrix, Drawing::RectF bounds) :
    RSRenderParams(matrix, bounds)
{
}

} // namespace OHOS::Rosen
