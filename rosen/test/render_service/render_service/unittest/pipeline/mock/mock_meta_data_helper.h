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

#ifndef GRAPHIC_RS_METADATA_HELPER_MOCK_H
#define GRAPHIC_RS_METADATA_HELPER_MOCK_H

#include <gmock/gmock.h>
#include "metadata_helper.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
namespace Mock {

class MockMetadataHelper : public MetadataHelper {
public:
    MockMetadataHelper() = default;
    ~MockMetadataHelper() = default;

    static MockMetadataHelper* GetInstance()
    {
        static MockMetadataHelper instance;
        return &instance;
    }

    MOCK_METHOD2(GetCropRectMetadata,
        GSError(const sptr<SurfaceBuffer>&, HDI::Display::Graphic::Common::V1_0::BufferHandleMetaRegion&));
};

} // namespace Mock
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_RS_METADATA_HELPER_MOCK_H