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

#include "rs_processor_factory.h"

#include "rs_physical_screen_processor.h"
#include "rs_uni_render_processor.h"
#include "rs_uni_render_virtual_processor.h"
#include "rs_virtual_screen_processor.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RSProcessor> RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType type)
{
    switch (type) {
        case RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE:
            return std::make_shared<RSVirtualScreenProcessor>();
        case RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE:
            return std::make_shared<RSPhysicalScreenProcessor>();
        case RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE:
            return std::make_shared<RSUniRenderProcessor>();
        case RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE:
        case RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE:
            return std::make_shared<RSUniRenderVirtualProcessor>();
        default:
            return nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS
