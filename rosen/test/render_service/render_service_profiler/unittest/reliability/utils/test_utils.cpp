/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_utils.h"

#include <gtest/gtest.h>
#include <memory>

#include "pixel_map.h"

#include "transaction/rs_marshalling_helper.h"
#include "utils/data.h"

namespace OHOS::Rosen {

bool CheckConsistencyWithPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, size_t position, size_t skipFromParcel)
{
    Parcel parcel;

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), 0);

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, pixelMap));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, pixelMap));

    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    parcel.FlushBuffer();

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), 0);

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, pixelMap));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    EXPECT_TRUE(RSMarshallingHelper::SkipPixelMap(parcel));

    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    parcel.FlushBuffer();

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, pixelMap));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    EXPECT_TRUE(RSMarshallingHelper::SkipFromParcel(parcel, skipFromParcel));

    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    parcel.FlushBuffer();
    return true;
}

bool CheckConsistencyWithData(std::shared_ptr<Drawing::Data> data, size_t position)
{
    Parcel parcel;

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), 0);

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, data));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, data));

    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);
    parcel.FlushBuffer();

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), 0);

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, data));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    EXPECT_TRUE(RSMarshallingHelper::SkipData(parcel));

    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);
    parcel.FlushBuffer();

    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, data));

    EXPECT_EQ(parcel.GetReadPosition(), 0);
    EXPECT_EQ(parcel.GetWritePosition(), position);

    parcel.SkipBytes(sizeof(int32_t));
    EXPECT_TRUE(RSMarshallingHelper::SkipFromParcel(parcel, data->GetSize()));
    
    EXPECT_EQ(parcel.GetReadPosition(), position);
    EXPECT_EQ(parcel.GetWritePosition(), position);
    parcel.FlushBuffer();
    return true;
}

} // namespace OHOS::Rosen