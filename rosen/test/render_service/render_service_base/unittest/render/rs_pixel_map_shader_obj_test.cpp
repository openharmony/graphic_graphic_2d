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

#include "gtest/gtest.h"

#include "render/rs_pixel_map_shader_obj.h"
#include "render/rs_pixel_map_util.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "pixel_map.h"
#include "draw/surface.h"
#ifdef ROSEN_OHOS
#include <message_parcel.h>
#include "utils/object_helper.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSPixelMapShaderObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height);
};

void RSPixelMapShaderObjTest::SetUpTestCase() {}

void RSPixelMapShaderObjTest::TearDownTestCase() {}

void RSPixelMapShaderObjTest::SetUp() {}
void RSPixelMapShaderObjTest::TearDown() {}

std::shared_ptr<Media::PixelMap> RSPixelMapShaderObjTest::CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap == nullptr) {
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo info =
        Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    if (surface == nullptr) {
        return nullptr;
    }
    auto canvas = surface->GetCanvas();
    if (canvas == nullptr) {
        return nullptr;
    }
    canvas->Clear(Drawing::Color::COLOR_BLUE);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_GREEN);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width, height));
    canvas->DetachBrush();
    return pixelmap;
}

/*
 * @tc.name: Constructor001
 * @tc.desc: Test RSPixelMapShaderObj CreateForUnmarshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Constructor001, TestSize.Level1)
{
    auto shaderObj = RSPixelMapShaderObj::CreateForUnmarshalling();
    ASSERT_NE(shaderObj, nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));
}

/*
 * @tc.name: Constructor002
 * @tc.desc: Test RSPixelMapShaderObj parameterized constructor with valid PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Constructor002, TestSize.Level1)
{
    auto pixelMap = CreatePixelMap(100, 100);
    ASSERT_NE(pixelMap, nullptr);

    Drawing::TileMode tileX = Drawing::TileMode::CLAMP;
    Drawing::TileMode tileY = Drawing::TileMode::REPEAT;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(pixelMap, tileX, tileY,
        sampling, matrix));
    ASSERT_NE(shaderObj, nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));
}

/*
 * @tc.name: Constructor003
 * @tc.desc: Test RSPixelMapShaderObj with null PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Constructor003, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Drawing::TileMode tileX = Drawing::TileMode::MIRROR;
    Drawing::TileMode tileY = Drawing::TileMode::DECAL;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::make_shared<RSPixelMapShaderObj>(nullPixelMap, tileX, tileY, sampling, matrix);
    ASSERT_NE(shaderObj, nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));
}

/*
 * @tc.name: CreatePixelMapShader001
 * @tc.desc: Test RSPixelMapShaderObj::CreatePixelMapShader with valid PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, CreatePixelMapShader001, TestSize.Level1)
{
    auto pixelMap = CreatePixelMap(200, 150);
    ASSERT_NE(pixelMap, nullptr);

    Drawing::TileMode tileX = Drawing::TileMode::REPEAT;
    Drawing::TileMode tileY = Drawing::TileMode::REPEAT;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderEffect = RSPixelMapShaderObj::CreatePixelMapShader(pixelMap, tileX, tileY, sampling, matrix);
    ASSERT_NE(shaderEffect, nullptr);
    EXPECT_TRUE(shaderEffect->IsLazy());
    EXPECT_EQ(shaderEffect->GetType(), Drawing::ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Extract the Obj from Lazy and check its type
    auto obj = shaderEffect->GetShaderEffectObj();
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));
}

/*
 * @tc.name: CreatePixelMapShader002
 * @tc.desc: Test RSPixelMapShaderObj::CreatePixelMapShader with null PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, CreatePixelMapShader002, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Drawing::TileMode tileX = Drawing::TileMode::CLAMP;
    Drawing::TileMode tileY = Drawing::TileMode::CLAMP;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderEffect = RSPixelMapShaderObj::CreatePixelMapShader(nullPixelMap, tileX, tileY, sampling, matrix);
    EXPECT_TRUE(shaderEffect == nullptr);
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test RSPixelMapShaderObj::GenerateBaseObject with valid PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, GenerateBaseObject001, TestSize.Level1)
{
    auto pixelMap = CreatePixelMap(128, 128);
    ASSERT_NE(pixelMap, nullptr);

    Drawing::TileMode tileX = Drawing::TileMode::MIRROR;
    Drawing::TileMode tileY = Drawing::TileMode::CLAMP;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(pixelMap, tileX, tileY,
        sampling, matrix));
    auto baseObject = shaderObj->GenerateBaseObject();
    ASSERT_NE(baseObject, nullptr);

    auto generatedShader = std::static_pointer_cast<Drawing::ShaderEffect>(baseObject);
    ASSERT_NE(generatedShader, nullptr);
    EXPECT_EQ(generatedShader->GetType(), Drawing::ShaderEffect::ShaderEffectType::IMAGE);
    EXPECT_FALSE(generatedShader->IsLazy());

    // Memory validation: Compare with direct CreateImageShader using serialization data
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    ASSERT_NE(image, nullptr);
    auto directShader = Drawing::ShaderEffect::CreateImageShader(*image, tileX, tileY, sampling, matrix);
    ASSERT_NE(directShader, nullptr);

    // Compare serialization data to verify functional equivalence
    auto generatedData = generatedShader->Serialize();
    auto directData = directShader->Serialize();
    ASSERT_NE(generatedData, nullptr);
    ASSERT_NE(directData, nullptr);
    EXPECT_EQ(generatedData->GetSize(), directData->GetSize());
    EXPECT_EQ(memcmp(generatedData->GetData(), directData->GetData(), generatedData->GetSize()), 0);
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test RSPixelMapShaderObj::GenerateBaseObject with null PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, GenerateBaseObject002, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Drawing::TileMode tileX = Drawing::TileMode::REPEAT;
    Drawing::TileMode tileY = Drawing::TileMode::MIRROR;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(nullPixelMap, tileX, tileY,
        sampling, matrix));
    auto baseObject = shaderObj->GenerateBaseObject();
    EXPECT_TRUE(baseObject == nullptr);
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test RSPixelMapShaderObj::GenerateBaseObject consistency
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, GenerateBaseObject003, TestSize.Level1)
{
    auto pixelMap = CreatePixelMap(64, 64);
    ASSERT_NE(pixelMap, nullptr);

    Drawing::TileMode tileX = Drawing::TileMode::REPEAT;
    Drawing::TileMode tileY = Drawing::TileMode::CLAMP;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(pixelMap, tileX, tileY,
        sampling, matrix));

    auto generatedShader1 = std::static_pointer_cast<Drawing::ShaderEffect>(shaderObj->GenerateBaseObject());
    auto generatedShader2 = std::static_pointer_cast<Drawing::ShaderEffect>(shaderObj->GenerateBaseObject());
    ASSERT_NE(generatedShader1, nullptr);
    ASSERT_NE(generatedShader2, nullptr);

    // Multiple calls to GenerateBaseObject should produce equivalent results
    auto data1 = generatedShader1->Serialize();
    auto data2 = generatedShader2->Serialize();
    ASSERT_NE(data1, nullptr);
    ASSERT_NE(data2, nullptr);
    EXPECT_EQ(data1->GetSize(), data2->GetSize());
    EXPECT_EQ(memcmp(data1->GetData(), data2->GetData(), data1->GetSize()), 0);
}

#ifdef ROSEN_OHOS

/*
 * @tc.name: Marshalling002
 * @tc.desc: Test RSPixelMapShaderObj::Marshalling with null PixelMap
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Marshalling002, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> nullPixelMap = nullptr;
    Drawing::TileMode tileX = Drawing::TileMode::CLAMP;
    Drawing::TileMode tileY = Drawing::TileMode::DECAL;
    Drawing::SamplingOptions sampling;
    Drawing::Matrix matrix;

    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(nullPixelMap, tileX, tileY,
        sampling, matrix));

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: Unmarshalling001
 * @tc.desc: Test RSPixelMapShaderObj::Unmarshalling with valid parcel and ObjectHelper registration
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Unmarshalling001, TestSize.Level1)
{
    // First, create and marshal a RSPixelMapShaderObj
    auto originalPixelMap = CreatePixelMap(100, 75);
    ASSERT_NE(originalPixelMap, nullptr);

    Drawing::TileMode originalTileX = Drawing::TileMode::MIRROR;
    Drawing::TileMode originalTileY = Drawing::TileMode::REPEAT;
    Drawing::SamplingOptions originalSampling(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::LINEAR);
    Drawing::Matrix originalMatrix;
    originalMatrix.Translate(10.0f, 20.0f);

    auto originalShaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(
        originalPixelMap, originalTileX, originalTileY, originalSampling, originalMatrix));

    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Now test unmarshalling using ObjectHelper pattern
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();

    // Verify that RSPixelMapShaderObj is properly registered
    auto func = Drawing::ObjectHelper::Instance().GetFunc(type, subType);
    ASSERT_NE(func, nullptr) << "RSPixelMapShaderObj should be registered in ObjectHelper";

    bool isValid = true;
    auto obj = func(parcel, isValid, 0);
    ASSERT_NE(obj, nullptr);
    EXPECT_TRUE(isValid);

    // Verify obj is RSPixelMapShaderObj
    auto newShaderObj = std::static_pointer_cast<RSPixelMapShaderObj>(obj);
    ASSERT_NE(newShaderObj, nullptr);
    EXPECT_EQ(obj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(obj->GetSubType(), static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));

    // Create ShaderEffectLazy from the obj
    auto lazyShader = Drawing::ShaderEffectLazy::CreateFromShaderEffectObj(
        std::static_pointer_cast<Drawing::ShaderEffectObj>(obj));
    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());
    EXPECT_EQ(lazyShader->GetType(), Drawing::ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify type consistency
    EXPECT_EQ(originalShaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(originalShaderObj->GetSubType(), newShaderObj->GetSubType());
}

/*
 * @tc.name: Unmarshalling002
 * @tc.desc: Test RSPixelMapShaderObj::Unmarshalling with invalid parcel
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, Unmarshalling002, TestSize.Level1)
{
    auto shaderObj = RSPixelMapShaderObj::CreateForUnmarshalling();

    MessageParcel emptyParcel;
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(emptyParcel, isValid);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for RSPixelMapShaderObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(RSPixelMapShaderObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    auto originalPixelMap = CreatePixelMap(150, 200);
    ASSERT_NE(originalPixelMap, nullptr);

    Drawing::TileMode originalTileX = Drawing::TileMode::DECAL;
    Drawing::TileMode originalTileY = Drawing::TileMode::CLAMP;
    Drawing::SamplingOptions originalSampling(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
    Drawing::Matrix originalMatrix;
    originalMatrix.Rotate(45.0f, 0.0f, 0.0f);

    auto originalShaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(
        originalPixelMap, originalTileX, originalTileY, originalSampling, originalMatrix));

    // Marshal
    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal directly using RSPixelMapShaderObj
    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType

    auto restoredShaderObj = RSPixelMapShaderObj::CreateForUnmarshalling();
    bool isValid = true;
    bool unmarshalResult = restoredShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
    ASSERT_NE(restoredShaderObj, nullptr);

    // Verify consistency
    EXPECT_EQ(originalShaderObj->GetType(), restoredShaderObj->GetType());
    EXPECT_EQ(originalShaderObj->GetSubType(), restoredShaderObj->GetSubType());

    // Memory validation: verify object equivalence
    auto originalBaseObject = originalShaderObj->GenerateBaseObject();
    auto restoredBaseObject = restoredShaderObj->GenerateBaseObject();
    ASSERT_NE(originalBaseObject, nullptr);
    ASSERT_NE(restoredBaseObject, nullptr);

    auto originalShader = std::static_pointer_cast<Drawing::ShaderEffect>(originalBaseObject);
    auto restoredShader = std::static_pointer_cast<Drawing::ShaderEffect>(restoredBaseObject);
    EXPECT_EQ(originalShader->GetType(), restoredShader->GetType());
    EXPECT_FALSE(originalShader->IsLazy());
    EXPECT_FALSE(restoredShader->IsLazy());

    // Memory-level comparison to ensure serialization/deserialization preserves shader functionality
    auto originalData = originalShader->Serialize();
    auto restoredData = restoredShader->Serialize();
    ASSERT_NE(originalData, nullptr);
    ASSERT_NE(restoredData, nullptr);
    EXPECT_EQ(originalData->GetSize(), restoredData->GetSize());
    EXPECT_EQ(memcmp(originalData->GetData(), restoredData->GetData(), originalData->GetSize()), 0);
}

#endif
} // namespace Rosen
} // namespace OHOS
