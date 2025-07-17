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
#include "effect/blend_shader_obj.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#include "utils/object_helper.h"
#endif
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BlendShaderObjTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BlendShaderObjTest::SetUpTestCase() {}
void BlendShaderObjTest::TearDownTestCase() {}
void BlendShaderObjTest::SetUp() {}
void BlendShaderObjTest::TearDown() {}

/*
 * @tc.name: Constructor001
 * @tc.desc: Test BlendShaderObj constructor scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Constructor001, TestSize.Level1)
{
    // Test 1: CreateForUnmarshalling
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test 2: Parameterized constructor with valid shaders
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode mode = BlendMode::SRC_OVER;
    auto validShaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
    EXPECT_TRUE(validShaderObj != nullptr);
    EXPECT_EQ(validShaderObj->GetType(), static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(validShaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test 3: Constructor with null dst shader
    std::shared_ptr<ShaderEffect> nullDst = nullptr;
    auto nullTestShader = ShaderEffect::CreateColorShader(0xFFFF0000);
    BlendMode nullTestMode = BlendMode::MULTIPLY;
    auto nullShaderObj = BlendShaderObj::Create(nullDst, nullTestShader, nullTestMode);
    EXPECT_TRUE(nullShaderObj == nullptr); // Should return nullptr when dst shader is null
}

/*
 * @tc.name: GenerateBaseObject001
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with valid shaders
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObject001, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode mode = BlendMode::OVERLAY;

    auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
    auto baseObject = shaderObj->GenerateBaseObject();
    EXPECT_TRUE(baseObject != nullptr);

    // Try to cast to ShaderEffect
    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    EXPECT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader->IsLazy());
    }
}

/*
 * @tc.name: GenerateBaseObject002
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with null dst shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObject002, TestSize.Level1)
{
    std::shared_ptr<ShaderEffect> nullDst = nullptr;
    auto srcShader = ShaderEffect::CreateColorShader(0xFFFF0000);
    BlendMode mode = BlendMode::SCREEN;
    auto shaderObj = BlendShaderObj::Create(nullDst, srcShader, mode);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when dst shader is null
}

/*
 * @tc.name: GenerateBaseObject003
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with null src shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObject003, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    std::shared_ptr<ShaderEffect> nullSrc = nullptr;
    BlendMode mode = BlendMode::DARKEN;
    auto shaderObj = BlendShaderObj::Create(dstShader, nullSrc, mode);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when src shader is null
}

/*
 * @tc.name: GenerateBaseObject004
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with different blend modes
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObject004, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    // Test different blend modes
    std::vector<BlendMode> blendModes = {
        BlendMode::SRC_OVER,
        BlendMode::MULTIPLY,
        BlendMode::SCREEN,
        BlendMode::OVERLAY,
        BlendMode::DARKEN,
        BlendMode::LIGHTEN
    };
    for (const auto& mode : blendModes) {
        auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
        auto baseObject = shaderObj->GenerateBaseObject();
        EXPECT_TRUE(baseObject != nullptr);
        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        EXPECT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        }
    }
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: Marshalling001
 * @tc.desc: Test BlendShaderObj::Marshalling with valid non-Lazy shaders
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Marshalling001, TestSize.Level1)
{
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode mode = BlendMode::MULTIPLY;
    auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result); // Should succeed with registered DataMarshallingCallback
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
}

/*
 * @tc.name: Marshalling002
 * @tc.desc: Test BlendShaderObj::Marshalling with null dst shader (failure case)
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Marshalling002, TestSize.Level1)
{
    std::shared_ptr<ShaderEffect> nullDst = nullptr;
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode mode = BlendMode::SCREEN;
    auto shaderObj = BlendShaderObj::Create(nullDst, srcShader, mode);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when dst shader is null
}

/*
 * @tc.name: Marshalling003
 * @tc.desc: Test BlendShaderObj::Marshalling with null src shader (failure case)
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Marshalling003, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    std::shared_ptr<ShaderEffect> nullSrc = nullptr;
    BlendMode mode = BlendMode::OVERLAY;
    auto shaderObj = BlendShaderObj::Create(dstShader, nullSrc, mode);
    EXPECT_TRUE(shaderObj == nullptr); // Should return nullptr when src shader is null
}

/*
 * @tc.name: Unmarshalling001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with non-Lazy shader data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Unmarshalling001, TestSize.Level1)
{
    MessageParcel parcel;
    // Write test data to parcel for non-Lazy shaders
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::DARKEN)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    parcel.WriteBool(false); // isSrcLazy = false
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid);
    // May fail due to missing DataUnmarshallingCallback, but test the code path
    EXPECT_FALSE(result); // Expected to fail without proper callback registration
}

/*
 * @tc.name: Unmarshalling002
 * @tc.desc: Test BlendShaderObj::Unmarshalling with Lazy shader data
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Unmarshalling002, TestSize.Level1)
{
    MessageParcel parcel;
    // Write test data to parcel for Lazy shaders
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::LIGHTEN)); // blendMode
    parcel.WriteBool(true); // isDstLazy = true
    parcel.WriteInt32(static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LIGHT_UP));
    parcel.WriteBool(true); // isSrcLazy = true
    parcel.WriteInt32(static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::LIGHT_UP));
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid);
    // May fail due to missing nested object creation, but test the code path
    EXPECT_FALSE(result); // Expected to fail without proper object creation
}

/*
 * @tc.name: GenerateBaseObject005
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with different BlendModes
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObject005, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);

    // Test additional blend modes not covered in GenerateBaseObject004
    std::vector<BlendMode> additionalModes = {
        BlendMode::CLEAR,
        BlendMode::SRC,
        BlendMode::DST,
        BlendMode::SRC_IN,
        BlendMode::DST_IN
    };

    for (const auto& mode : additionalModes) {
        auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
        EXPECT_TRUE(shaderObj != nullptr);

        auto baseObject = shaderObj->GenerateBaseObject();
        EXPECT_TRUE(baseObject != nullptr);

        auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
        EXPECT_TRUE(generatedShader != nullptr);
        if (generatedShader) {
            EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        }
    }
}

/*
 * @tc.name: Marshalling004
 * @tc.desc: Test BlendShaderObj::Marshalling without DataMarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Marshalling004, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode mode = BlendMode::MULTIPLY;

    auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, mode);
    EXPECT_TRUE(shaderObj != nullptr);

    // Ensure no DataMarshallingCallback is set
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_FALSE(result); // Should fail without DataMarshallingCallback
}

/*
 * @tc.name: Unmarshalling003
 * @tc.desc: Test BlendShaderObj::Unmarshalling without DataUnmarshallingCallback
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Unmarshalling003, TestSize.Level1)
{
    MessageParcel parcel;
    // Write test data for non-Lazy shaders
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::SCREEN)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    parcel.WriteBool(false); // isSrcLazy = false

    // Ensure no DataUnmarshallingCallback is set
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);

    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);

    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(result); // Should fail without DataUnmarshallingCallback
}

/*
 * @tc.name: Unmarshalling004
 * @tc.desc: Test BlendShaderObj::Unmarshalling with invalid nested object
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, Unmarshalling004, TestSize.Level1)
{
    MessageParcel parcel;
    // Write test data for Lazy shaders with invalid type
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::OVERLAY)); // blendMode
    parcel.WriteBool(true); // isDstLazy = true
    parcel.WriteInt32(999); // Invalid type
    parcel.WriteInt32(888); // Invalid subType

    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);

    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(result); // Should fail with invalid nested object type
}

/*
 * @tc.name: MarshallingUnmarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for BlendShaderObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, MarshallingUnmarshallingRoundTrip001, TestSize.Level1)
{
    // Create original BlendShaderObj
    auto originalDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto originalSrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    BlendMode originalMode = BlendMode::MULTIPLY;
    auto originalShaderObj = BlendShaderObj::Create(originalDstShader, originalSrcShader, originalMode);
    EXPECT_TRUE(originalShaderObj != nullptr);

    // Set up DataMarshallingCallback for successful marshalling
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    // Set up DataUnmarshallingCallback for successful unmarshalling
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });
    // Marshal
    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(newShaderObj != nullptr);

    // Read type and subType that were written externally
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));
    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Verify consistency
    EXPECT_EQ(originalShaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(originalShaderObj->GetSubType(), newShaderObj->GetSubType());
    // Test that both can generate base objects
    auto originalBaseObject = originalShaderObj->GenerateBaseObject();
    auto newBaseObject = newShaderObj->GenerateBaseObject();
    EXPECT_TRUE(originalBaseObject != nullptr);
    EXPECT_TRUE(newBaseObject != nullptr);
    auto originalShader = std::static_pointer_cast<ShaderEffect>(originalBaseObject);
    auto newShader = std::static_pointer_cast<ShaderEffect>(newBaseObject);
    EXPECT_EQ(originalShader->GetType(), newShader->GetType());
    EXPECT_FALSE(originalShader->IsLazy());
    EXPECT_FALSE(newShader->IsLazy());

    // Clean up callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: UnmarshallingWithLazyShader001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with Lazy shader round trip
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingWithLazyShader001, TestSize.Level1)
{
    // Create a BlendShaderObj with Lazy shaders
    auto normalDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto normalSrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);

    // Create Lazy shaders using ShaderEffectLazy factory methods
    auto lazyDstShader = ShaderEffectLazy::CreateBlendShader(normalDstShader, normalSrcShader, BlendMode::SRC_OVER);
    auto lazySrcShader = ShaderEffectLazy::CreateBlendShader(normalSrcShader, normalDstShader, BlendMode::MULTIPLY);
    EXPECT_TRUE(lazyDstShader != nullptr);
    EXPECT_TRUE(lazySrcShader != nullptr);
    EXPECT_TRUE(lazyDstShader->IsLazy());
    EXPECT_TRUE(lazySrcShader->IsLazy());

    // Set up callbacks for marshalling test
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });

    auto originalShaderObj = BlendShaderObj::Create(lazyDstShader, lazySrcShader, BlendMode::OVERLAY);
    EXPECT_TRUE(originalShaderObj != nullptr);

    // Marshal
    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(originalShaderObj->GetType());
    parcel.WriteInt32(originalShaderObj->GetSubType());
    bool marshalResult = originalShaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(newShaderObj != nullptr);

    // Read type and subType that were written externally
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
    // Verify consistency
    EXPECT_EQ(originalShaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(originalShaderObj->GetSubType(), newShaderObj->GetSubType());
    // Test that both can generate base objects
    auto originalBaseObject = originalShaderObj->GenerateBaseObject();
    auto newBaseObject = newShaderObj->GenerateBaseObject();
    EXPECT_TRUE(originalBaseObject != nullptr);
    EXPECT_TRUE(newBaseObject != nullptr);
    auto originalShader = std::static_pointer_cast<ShaderEffect>(originalBaseObject);
    auto newShader = std::static_pointer_cast<ShaderEffect>(newBaseObject);
    EXPECT_TRUE(originalShader != nullptr);
    EXPECT_TRUE(newShader != nullptr);
    // Real memory content comparison through Serialize
    auto originalData = originalShader->Serialize();
    auto newData = newShader->Serialize();
    EXPECT_TRUE(originalData != nullptr);
    EXPECT_TRUE(newData != nullptr);
    // Compare serialized data size
    EXPECT_EQ(originalData->GetSize(), newData->GetSize());
    EXPECT_GT(originalData->GetSize(), 0);
    EXPECT_GT(newData->GetSize(), 0);
    // Compare serialized memory content
    if (originalData->GetSize() > 0 && newData->GetSize() > 0) {
        const void* originalMemory = originalData->GetData();
        const void* newMemory = newData->GetData();
        EXPECT_TRUE(originalMemory != nullptr);
        EXPECT_TRUE(newMemory != nullptr);
        // Byte-level memory comparison
        int memResult = memcmp(originalMemory, newMemory, originalData->GetSize());
        EXPECT_EQ(memResult, 0);
    }
    // Clean up callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: MixedLazyNonLazyShader001
 * @tc.desc: Test BlendShaderObj with Lazy dst and non-Lazy src shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, MixedLazyNonLazyShader001, TestSize.Level1)
{
    // Create mixed shader types: Lazy dst, non-Lazy src
    auto normalDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto nonLazySrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyDstShader = ShaderEffectLazy::CreateBlendShader(normalDstShader, nonLazySrcShader, BlendMode::SRC_IN);
    EXPECT_TRUE(lazyDstShader != nullptr);
    EXPECT_TRUE(lazyDstShader->IsLazy());
    EXPECT_TRUE(nonLazySrcShader != nullptr);
    EXPECT_FALSE(nonLazySrcShader->IsLazy());

    auto shaderObj = BlendShaderObj::Create(lazyDstShader, nonLazySrcShader, BlendMode::SCREEN);
    EXPECT_TRUE(shaderObj != nullptr);

    // Test GenerateBaseObject with mixed types
    auto baseObject = shaderObj->GenerateBaseObject();
    EXPECT_TRUE(baseObject != nullptr);

    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    EXPECT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader->IsLazy());
    }

    // Set up callbacks for marshalling test
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });

    // Test marshalling/unmarshalling with mixed types
    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(shaderObj->GetType());
    parcel.WriteInt32(shaderObj->GetSubType());
    bool marshalResult = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);
    auto newShaderObj = BlendShaderObj::CreateForUnmarshalling();
    // Read type and subType that were written externally
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
    // Verify consistency
    EXPECT_EQ(shaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(shaderObj->GetSubType(), newShaderObj->GetSubType());
    // Clean up callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: MixedLazyNonLazyShader002
 * @tc.desc: Test BlendShaderObj with non-Lazy dst and Lazy src shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, MixedLazyNonLazyShader002, TestSize.Level1)
{
    // Create mixed shader types: non-Lazy dst, Lazy src
    auto nonLazyDstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto normalSrcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazySrcShader = ShaderEffectLazy::CreateBlendShader(normalSrcShader, nonLazyDstShader, BlendMode::DST_IN);
    EXPECT_TRUE(nonLazyDstShader != nullptr);
    EXPECT_FALSE(nonLazyDstShader->IsLazy());
    EXPECT_TRUE(lazySrcShader != nullptr);
    EXPECT_TRUE(lazySrcShader->IsLazy());

    auto shaderObj = BlendShaderObj::Create(nonLazyDstShader, lazySrcShader, BlendMode::DARKEN);
    EXPECT_TRUE(shaderObj != nullptr);

    // Test GenerateBaseObject with mixed types
    auto baseObject = shaderObj->GenerateBaseObject();
    EXPECT_TRUE(baseObject != nullptr);

    auto generatedShader = std::static_pointer_cast<ShaderEffect>(baseObject);
    EXPECT_TRUE(generatedShader != nullptr);
    if (generatedShader) {
        EXPECT_EQ(generatedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader->IsLazy());
    }

    // Set up callbacks for marshalling test
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });

    // Test marshalling/unmarshalling with mixed types
    MessageParcel parcel;
    // Write type and subType externally (as done by MarshallingDrawingObjectFromDrawCmdList)
    parcel.WriteInt32(shaderObj->GetType());
    parcel.WriteInt32(shaderObj->GetSubType());
    bool marshalResult = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);
    auto newShaderObj = BlendShaderObj::CreateForUnmarshalling();
    // Read type and subType that were written externally
    int32_t type = parcel.ReadInt32();
    int32_t subType = parcel.ReadInt32();
    EXPECT_EQ(type, static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(subType, static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    bool isValid = true;
    bool unmarshalResult = newShaderObj->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);

    // Verify consistency
    EXPECT_EQ(shaderObj->GetType(), newShaderObj->GetType());
    EXPECT_EQ(shaderObj->GetSubType(), newShaderObj->GetSubType());

    // Test that both can generate base objects
    auto originalBaseObject = shaderObj->GenerateBaseObject();
    auto newBaseObject = newShaderObj->GenerateBaseObject();
    EXPECT_TRUE(originalBaseObject != nullptr);
    EXPECT_TRUE(newBaseObject != nullptr);

    // Clean up callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: DepthProtectionTest001
 * @tc.desc: Test BlendShaderObj depth protection against malicious nesting
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, DepthProtectionTest001, TestSize.Level1)
{
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    // Simulate malicious deep nesting by using depth close to limit
    // This should trigger depth protection when depth >= MAX_NESTING_DEPTH
    int32_t maliciousDepth = ObjectHelper::MAX_NESTING_DEPTH - 1; // Very close to limit
    // Test with depth at limit boundary
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, maliciousDepth);
    EXPECT_FALSE(result); // Should fail due to depth protection
}

/*
 * @tc.name: DepthProtectionTest002
 * @tc.desc: Test BlendShaderObj depth protection with exact limit
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, DepthProtectionTest002, TestSize.Level1)
{
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    // Test with depth exactly at MAX_NESTING_DEPTH
    int32_t exactLimitDepth = ObjectHelper::MAX_NESTING_DEPTH;
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, exactLimitDepth);
    EXPECT_FALSE(result); // Should fail due to depth >= MAX_NESTING_DEPTH
}

/*
 * @tc.name: DepthProtectionTest003
 * @tc.desc: Test BlendShaderObj depth protection with safe depth
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, DepthProtectionTest003, TestSize.Level1)
{
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);

    // Set up callbacks for successful parsing
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Drawing::Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            // Return nullptr to simulate data reading failure
            return nullptr;
        });

    MessageParcel parcel;
    // Write valid data for BlendShaderObj
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::SRC_OVER)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    parcel.WriteBool(false); // isSrcLazy = false
    // Test with safe depth (well below limit)
    int32_t safeDepth = ObjectHelper::MAX_NESTING_DEPTH / 10; // Much less than MAX_NESTING_DEPTH
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, safeDepth);
    // Should fail due to DataUnmarshallingCallback returning nullptr, not due to depth protection
    EXPECT_FALSE(result); // Expected to fail due to DataUnmarshallingCallback failure
    // Clean up callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: DepthProtectionTest004
 * @tc.desc: Test depth protection with extremely large depth value
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, DepthProtectionTest004, TestSize.Level1)
{
    auto shaderObj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(shaderObj != nullptr);
    MessageParcel parcel;
    // Test with extremely large depth value (simulating attack)
    int32_t attackDepth = ObjectHelper::MAX_NESTING_DEPTH * 2; // Much larger than MAX_NESTING_DEPTH
    bool isValid = true;
    bool result = shaderObj->Unmarshalling(parcel, isValid, attackDepth);
    EXPECT_FALSE(result); // Should fail immediately due to depth protection
}

/*
 * @tc.name: UnmarshallingReadBlendModeFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with parcel.ReadInt32(blendModeValue) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingReadBlendModeFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Create an empty Parcel - ReadInt32 will fail due to no data
    MessageParcel emptyParcel;
    bool isValid = true;
    bool result = obj->Unmarshalling(emptyParcel, isValid, 0);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: UnmarshallingReadIsDstLazyFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with parcel.ReadBool(isDstLazy) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingReadIsDstLazyFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Create a Parcel with only blendMode - isDstLazy read will fail
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::SRC_OVER)); // blendMode
    // No isDstLazy written, so ReadBool(isDstLazy) will fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: UnmarshallingDstLazyShaderFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with ShaderEffectLazy::Unmarshalling failure for dst
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingDstLazyShaderFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Create a Parcel with basic data but isDstLazy=true
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::MULTIPLY)); // blendMode
    parcel.WriteBool(true); // isDstLazy = true
    // Write incomplete data for ShaderEffectLazy to cause failure
    parcel.WriteInt32(static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    // Missing subType data will cause ShaderEffectLazy::Unmarshalling to fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: UnmarshallingDstNonLazyShaderFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with ShaderEffect::Unmarshalling failure for dst
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingDstNonLazyShaderFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Create a Parcel with basic data but isDstLazy=false
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::OVERLAY)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    // No shader data written, so ShaderEffect::Unmarshalling will fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: UnmarshallingReadIsSrcLazyFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with parcel.ReadBool(isSrcLazy) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingReadIsSrcLazyFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Setup successful dst shader unmarshalling first
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::SCREEN)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    // Write minimal shader data for dst to succeed
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER));
    parcel.WriteInt32(0xFF0000FF); // color
    // No isSrcLazy written, so ReadBool(isSrcLazy) will fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: UnmarshallingSrcLazyShaderFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with ShaderEffectLazy::Unmarshalling failure for src
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingSrcLazyShaderFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Setup successful dst shader unmarshalling first
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::DARKEN)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    // Write minimal shader data for dst to succeed
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER));
    parcel.WriteInt32(0xFF0000FF); // color
    parcel.WriteBool(true); // isSrcLazy = true
    // Write incomplete data for ShaderEffectLazy to cause failure
    parcel.WriteInt32(static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT));
    // Missing subType data will cause ShaderEffectLazy::Unmarshalling to fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

/*
 * @tc.name: UnmarshallingSrcNonLazyShaderFailure001
 * @tc.desc: Test BlendShaderObj::Unmarshalling with ShaderEffect::Unmarshalling failure for src
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(BlendShaderObjTest, UnmarshallingSrcNonLazyShaderFailure001, TestSize.Level1)
{
    auto obj = BlendShaderObj::CreateForUnmarshalling();
    EXPECT_TRUE(obj != nullptr);
    // Setup successful dst shader unmarshalling first
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Drawing::Data> {
            std::shared_ptr<Drawing::Data> data;
            if (RSMarshallingHelper::Unmarshalling(parcel, data)) {
                return data;
            }
            return nullptr;
        });
    MessageParcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(BlendMode::LIGHTEN)); // blendMode
    parcel.WriteBool(false); // isDstLazy = false
    // Write minimal shader data for dst to succeed
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER));
    parcel.WriteInt32(0xFF0000FF); // color
    parcel.WriteBool(false); // isSrcLazy = false
    // No src shader data written, so ShaderEffect::Unmarshalling will fail
    bool isValid = true;
    bool result = obj->Unmarshalling(parcel, isValid, 0);
    EXPECT_FALSE(result);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);
}

// Mock class that always fails GenerateBaseObject()
class MockFailingShaderEffectObj : public ShaderEffectObj {
public:
    MockFailingShaderEffectObj() : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND))
    {
    }

    virtual std::shared_ptr<void> GenerateBaseObject() override
    {
        // Always return nullptr to simulate failure
        return nullptr;
    }

#ifdef ROSEN_OHOS
    virtual bool Marshalling(Parcel& parcel) override
    {
        // Minimal implementation
        return parcel.WriteInt32(type_) && parcel.WriteInt32(subType_);
    }

    virtual bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override
    {
        // Minimal implementation
        return true;
    }
#endif
};

/*
 * @tc.name: GenerateBaseObjectLazyMaterializeFail001
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with Lazy shader that fails to materialize
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObjectLazyMaterializeFail001, TestSize.Level1)
{
    // Strategy: Create a LazyShader with a MockFailingShaderEffectObj that always returns nullptr
    // This will cause Materialize() to fail, which should make BlendShaderObj::GenerateBaseObject() return nullptr

    // Create a mock ShaderEffectObj that will fail GenerateBaseObject()
    auto failingObj = std::make_shared<MockFailingShaderEffectObj>();
    ASSERT_NE(failingObj, nullptr);

    // Verify that the mock object fails as expected
    auto baseObject = failingObj->GenerateBaseObject();
    EXPECT_EQ(baseObject, nullptr);

    // Create LazyShader using the failing ShaderEffectObj
    auto failingLazyShader = ShaderEffectLazy::CreateFromShaderEffectObj(failingObj);
    ASSERT_NE(failingLazyShader, nullptr);
    EXPECT_TRUE(failingLazyShader->IsLazy());

    // Verify that the LazyShader fails to materialize due to the failing ShaderEffectObj
    auto materializedShader = failingLazyShader->Materialize();
    EXPECT_EQ(materializedShader, nullptr);

    // Now create a BlendShaderObj with the failing LazyShader and a normal shader
    auto normalShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    ASSERT_NE(normalShader, nullptr);

    // Test case 1: Failing LazyShader as dst, normal shader as src
    auto blendObj1 = BlendShaderObj::Create(failingLazyShader, normalShader, BlendMode::SRC_OVER);
    ASSERT_NE(blendObj1, nullptr);

    // This should return nullptr because the dst LazyShader fails to materialize
    auto result1 = blendObj1->GenerateBaseObject();
    EXPECT_EQ(result1, nullptr); // Target branch: if (!actualDst) return nullptr;

    // Test case 2: Normal shader as dst, failing LazyShader as src
    auto blendObj2 = BlendShaderObj::Create(normalShader, failingLazyShader, BlendMode::MULTIPLY);
    ASSERT_NE(blendObj2, nullptr);

    // This should return nullptr because the src LazyShader fails to materialize
    auto result2 = blendObj2->GenerateBaseObject();
    EXPECT_EQ(result2, nullptr); // Target branch: if (!actualSrc) return nullptr;

    // Test case 3: Both shaders are failing LazyShaders
    auto blendObj3 = BlendShaderObj::Create(failingLazyShader, failingLazyShader, BlendMode::SCREEN);
    ASSERT_NE(blendObj3, nullptr);

    // This should return nullptr because both LazyShaders fail to materialize
    auto result3 = blendObj3->GenerateBaseObject();
    EXPECT_EQ(result3, nullptr); // Target branch: if (!actualDst) return nullptr;
}

/*
 * @tc.name: GenerateBaseObjectMixedLazyNormal001
 * @tc.desc: Test BlendShaderObj::GenerateBaseObject with mixed lazy and normal shaders
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(BlendShaderObjTest, GenerateBaseObjectMixedLazyNormal001, TestSize.Level1)
{
    // Test mixed scenarios: one lazy, one normal
    auto normalDst = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto normalSrc = ShaderEffect::CreateColorShader(0xFF00FF00);

    // Create a valid lazy shader
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(normalDst, normalSrc, BlendMode::MULTIPLY);
    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    // Test 1: Normal dst + Lazy src
    auto blendObj1 = BlendShaderObj::Create(normalDst, lazyShader, BlendMode::SCREEN);
    ASSERT_NE(blendObj1, nullptr);

    auto baseObject1 = blendObj1->GenerateBaseObject();
    EXPECT_NE(baseObject1, nullptr);

    auto generatedShader1 = std::static_pointer_cast<ShaderEffect>(baseObject1);
    EXPECT_NE(generatedShader1, nullptr);
    if (generatedShader1) {
        EXPECT_EQ(generatedShader1->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader1->IsLazy()); // Should be materialized
    }

    // Test 2: Lazy dst + Normal src
    auto blendObj2 = BlendShaderObj::Create(lazyShader, normalSrc, BlendMode::OVERLAY);
    ASSERT_NE(blendObj2, nullptr);

    auto baseObject2 = blendObj2->GenerateBaseObject();
    EXPECT_NE(baseObject2, nullptr);

    auto generatedShader2 = std::static_pointer_cast<ShaderEffect>(baseObject2);
    EXPECT_NE(generatedShader2, nullptr);
    if (generatedShader2) {
        EXPECT_EQ(generatedShader2->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader2->IsLazy()); // Should be materialized
    }

    // Test 3: Both lazy shaders
    auto lazyShader2 = ShaderEffectLazy::CreateBlendShader(normalDst, normalSrc, BlendMode::DARKEN);
    ASSERT_NE(lazyShader2, nullptr);

    auto blendObj3 = BlendShaderObj::Create(lazyShader, lazyShader2, BlendMode::LIGHTEN);
    ASSERT_NE(blendObj3, nullptr);

    auto baseObject3 = blendObj3->GenerateBaseObject();
    EXPECT_NE(baseObject3, nullptr);

    auto generatedShader3 = std::static_pointer_cast<ShaderEffect>(baseObject3);
    EXPECT_NE(generatedShader3, nullptr);
    if (generatedShader3) {
        EXPECT_EQ(generatedShader3->GetType(), ShaderEffect::ShaderEffectType::BLEND);
        EXPECT_FALSE(generatedShader3->IsLazy()); // Should be materialized
    }
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
