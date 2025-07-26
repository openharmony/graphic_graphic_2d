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

#include "gtest/gtest.h"

#include "draw/brush.h"
#include "effect/filter.h"
#include "effect/image_filter_lazy.h"
#include "effect/shader_effect_lazy.h"
#include "image/image.h"
#include "message_parcel.h"
#include "pixel_map.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_canvas.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/vertices.h"
#include "utils/object_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
#define TEST_MEM_SIZE 10
#define TEST_INVALIED_ID 9999
class CmdListHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CmdListHelperTest::SetUpTestCase() {}
void CmdListHelperTest::TearDownTestCase() {}
void CmdListHelperTest::SetUp() {}
void CmdListHelperTest::TearDown() {}

/**
 * @tc.name: AddImageToCmdList001
 * @tc.desc: Test the AddImageToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddImageToCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::shared_ptr<Image> image = nullptr;
    OpDataHandle handle = CmdListHelper::AddImageToCmdList(*cmdList, image);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);
}

/**
 * @tc.name: GetImageFromCmdList001
 * @tc.desc: Test the GetImageFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetImageFromCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    OpDataHandle handle = {};
    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    handle.offset = cmdList->AddImageData(imageData->GetData(), imageData->GetSize());
    handle.size = imageData->GetSize();
    auto image = CmdListHelper::GetImageFromCmdList(*cmdList, handle);
    EXPECT_EQ(image, nullptr);
}

/**
 * @tc.name: AddBitmapToCmdList001
 * @tc.desc: Test the AddBitmapToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddBitmapToCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Bitmap bitmap;
    ImageHandle handle = CmdListHelper::AddBitmapToCmdList(*cmdList, bitmap);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    BitmapFormat format = { ColorType::COLORTYPE_ALPHA_8, AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(100, 100, format);
    handle = CmdListHelper::AddBitmapToCmdList(*cmdList, bitmap);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 100 * 100);
}

/**
 * @tc.name: GetBitmapFromCmdList001
 * @tc.desc: Test the GetBitmapFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetBitmapFromCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    ImageHandle handle = {};
    auto bitmap = CmdListHelper::GetBitmapFromCmdList(*cmdList, handle);
    EXPECT_EQ(bitmap, nullptr);

    handle.size = 100;
    bitmap = CmdListHelper::GetBitmapFromCmdList(*cmdList, handle);
    EXPECT_EQ(bitmap, nullptr);
}

/**
 * @tc.name: GetPictureFromCmdList001
 * @tc.desc: Test the GetPictureFromCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetPictureFromCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    OpDataHandle handle = {};
    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    handle.offset = cmdList->AddImageData(imageData->GetData(), imageData->GetSize());
    handle.size = imageData->GetSize();
    auto picture = CmdListHelper::GetPictureFromCmdList(*cmdList, handle);
    EXPECT_EQ(picture, nullptr);
}

/**
 * @tc.name: AddCompressDataToCmdList001
 * @tc.desc: Test the AddCompressDataToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddCompressDataToCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Bitmap bitmap;
    std::shared_ptr<Data> data = nullptr;
    OpDataHandle handle = CmdListHelper::AddCompressDataToCmdList(*cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    data = std::make_shared<Data>();
    handle = CmdListHelper::AddCompressDataToCmdList(*cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    data->BuildUninitialized(10);
    handle = CmdListHelper::AddCompressDataToCmdList(*cmdList, data);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, data->GetSize());

    auto newData = CmdListHelper::GetCompressDataFromCmdList(*cmdList, handle);
    EXPECT_NE(newData, nullptr);
}

/**
 * @tc.name: AddChildToCmdList001
 * @tc.desc: Test the AddChildToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddChildToCmdList001, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::shared_ptr<CmdList> childCmdList = nullptr;
    auto handle = CmdListHelper::AddChildToCmdList(*cmdList, childCmdList);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    childCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    handle = CmdListHelper::AddChildToCmdList(*cmdList, childCmdList);
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);

    auto imageData = std::make_shared<Data>();
    imageData->BuildUninitialized(10);
    childCmdList->AddOp<ClearOpItem::ConstructorHandle>(Color::COLOR_BLACK);
    childCmdList->AddImageData(imageData->GetData(), imageData->GetSize());
    handle = CmdListHelper::AddChildToCmdList(*cmdList, childCmdList);
    EXPECT_EQ(handle.size, childCmdList->GetData().second);
    EXPECT_EQ(handle.imageSize, imageData->GetSize());
}

/**
 * @tc.name: AddVerticesToCmdList
 * @tc.desc: Test the AddVerticesToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddVerticesToCmdList, TestSize.Level1)
{
    int indexCount = 0;
    int vertexCount = 1;
    Drawing::BuilderFlags flags = Drawing::BuilderFlags::HAS_COLORS_BUILDER_FLAG;
    Vertices::Builder build(Drawing::VertexMode::TRIANGLESSTRIP_VERTEXMODE,
                            vertexCount, indexCount, static_cast<uint32_t>(flags));

    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddVerticesToCmdList(*cmdList, *build.Detach());
    std::shared_ptr<Vertices> vertices = CmdListHelper::GetVerticesFromCmdList(*cmdList, handle);
    EXPECT_NE(vertices, nullptr);
}

/**
 * @tc.name: ImageObject
 * @tc.desc: Test the ImageObject function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, ImageObject, TestSize.Level1)
{
    class ImageObject : public Drawing::ExtendImageObject {
        public:
        ImageObject() = default;
        ~ImageObject() override {};

        void Playback(Canvas& canvas, const Rect& rect,
            const SamplingOptions& sampling, bool isBackground = false) override {};
    };
    std::shared_ptr<ExtendImageObject> object = std::make_shared<ImageObject>();
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddImageObjectToCmdList(*cmdList, object);
    std::shared_ptr<ExtendImageObject> obj = CmdListHelper::GetImageObjectFromCmdList(*cmdList, handle);
    EXPECT_NE(obj, nullptr);
}

/**
 * @tc.name: ImageBaseObj
 * @tc.desc: Test the ImageBaseObj function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, ImageBaseObj, TestSize.Level1)
{
    class ImageBaseObj : public Drawing::ExtendImageBaseObj {
        public:
        ImageBaseObj() = default;
        ~ImageBaseObj() override {};

        void Playback(Canvas& canvas, const Rect& rect, const SamplingOptions& sampling,
            SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override {};
    };
    std::shared_ptr<ExtendImageBaseObj> object = std::make_shared<ImageBaseObj>();
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddImageBaseObjToCmdList(*cmdList, object);
    EXPECT_EQ(cmdList->GetImageBaseObj(TEST_INVALIED_ID), nullptr);
    std::shared_ptr<ExtendImageBaseObj> obj = CmdListHelper::GetImageBaseObjFromCmdList(*cmdList, handle);
    EXPECT_NE(obj, nullptr);
}

/**
 * @tc.name: Symbol
 * @tc.desc: Test the Symbol function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, Symbol, TestSize.Level1)
{
    DrawingHMSymbolData symbol;
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddSymbolToCmdList(*cmdList, symbol);
    auto obj = CmdListHelper::GetSymbolFromCmdList(*cmdList, handle);
    EXPECT_EQ(obj.symbolId, 0);
}

/**
 * @tc.name: SymbolLayers
 * @tc.desc: Test the SymbolLayers function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, SymbolLayers, TestSize.Level1)
{
    DrawingSymbolLayers symbolLayers{};
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddSymbolLayersToCmdList(*cmdList, symbolLayers);
    DrawingSymbolLayers obj = CmdListHelper::GetSymbolLayersFromCmdList(*cmdList, handle);

    std::vector<size_t> layers;
    symbolLayers.layers.emplace_back(layers);
    DrawingRenderGroup group;
    symbolLayers.renderGroups.push_back(group);

    handle = CmdListHelper::AddSymbolLayersToCmdList(*cmdList, symbolLayers);
    obj = CmdListHelper::GetSymbolLayersFromCmdList(*cmdList, handle);
    EXPECT_EQ(obj.symbolGlyphId, 0);
}

/**
 * @tc.name: RenderGroup
 * @tc.desc: Test the RenderGroup function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, RenderGroup, TestSize.Level1)
{
    DrawingRenderGroup renderGroup;
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawingGroupInfo info;
    renderGroup.groupInfos.push_back(info);
    auto handle = CmdListHelper::AddRenderGroupToCmdList(*cmdList, renderGroup);
    auto obj = CmdListHelper::GetRenderGroupFromCmdList(*cmdList, handle);
    EXPECT_EQ(obj.color.r, 0);
}

/**
 * @tc.name: GroupInfo
 * @tc.desc: Test the GroupInfo function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GroupInfo, TestSize.Level1)
{
    DrawingGroupInfo groupInfo;
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    auto handle = CmdListHelper::AddGroupInfoToCmdList(*cmdList, groupInfo);
    auto obj = CmdListHelper::GetGroupInfoFromCmdList(*cmdList, handle);
    EXPECT_NE(cmdList, nullptr);
}

/**
 * @tc.name: Data
 * @tc.desc: Test the Data function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, Data, TestSize.Level1)
{
    Data data;
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    char str[TEST_MEM_SIZE];
    OpDataHandle handle = CmdListHelper::AddDataToCmdList(*cmdList, nullptr);
    std::shared_ptr<Data> datasptr = CmdListHelper::GetDataFromCmdList(*cmdList, handle);
    handle = CmdListHelper::AddDataToCmdList(*cmdList, &data);
    datasptr = CmdListHelper::GetDataFromCmdList(*cmdList, handle);
    EXPECT_EQ(datasptr, nullptr);
    data.BuildWithoutCopy(str, TEST_MEM_SIZE);
    handle = CmdListHelper::AddDataToCmdList(*cmdList, &data);
    datasptr = CmdListHelper::GetDataFromCmdList(*cmdList, handle);
    EXPECT_NE(datasptr, nullptr);
}

/**
 * @tc.name: ColorSpace
 * @tc.desc: Test the ColorSpace function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, ColorSpace, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    OpDataHandle handle = CmdListHelper::AddColorSpaceToCmdList(*cmdList, nullptr);
    std::shared_ptr<ColorSpace> sptr = CmdListHelper::GetColorSpaceFromCmdList(*cmdList, handle);

    const std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    handle = CmdListHelper::AddColorSpaceToCmdList(*cmdList, colorSpace);
    sptr = CmdListHelper::GetColorSpaceFromCmdList(*cmdList, handle);
    EXPECT_NE(sptr, nullptr);
}

/**
 * @tc.name: ShaderEffect
 * @tc.desc: Test the ShaderEffect function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, ShaderEffect, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, nullptr);

    const std::shared_ptr<ShaderEffect> effect = ShaderEffect::CreateColorShader(0);
    handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, effect);
    EXPECT_EQ(handle.offset, 0);
}

/**
 * @tc.name: PathEffect
 * @tc.desc: Test the PathEffect function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, PathEffect, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    FlattenableHandle handle = CmdListHelper::AddPathEffectToCmdList(*cmdList, nullptr);
    std::shared_ptr<PathEffect> sptr = CmdListHelper::GetPathEffectFromCmdList(*cmdList, handle);

    const std::shared_ptr<PathEffect> effect = PathEffect::CreateCornerPathEffect(10);
    handle = CmdListHelper::AddPathEffectToCmdList(*cmdList, effect);
    sptr = CmdListHelper::GetPathEffectFromCmdList(*cmdList, handle);
    EXPECT_NE(sptr, nullptr);
}

/**
 * @tc.name: MaskFilter
 * @tc.desc: Test the MaskFilter function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, MaskFilter, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    FlattenableHandle handle = CmdListHelper::AddMaskFilterToCmdList(*cmdList, nullptr);
    std::shared_ptr<MaskFilter> sptr = CmdListHelper::GetMaskFilterFromCmdList(*cmdList, handle);

    const std::shared_ptr<MaskFilter> filter = MaskFilter::CreateBlurMaskFilter(BlurType::NORMAL, 0);
    handle = CmdListHelper::AddMaskFilterToCmdList(*cmdList, filter);
    sptr = CmdListHelper::GetMaskFilterFromCmdList(*cmdList, handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: ColorFilter
 * @tc.desc: Test the ColorFilter function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, ColorFilter, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    FlattenableHandle handle = CmdListHelper::AddColorFilterToCmdList(*cmdList, nullptr);
    std::shared_ptr<ColorFilter> sptr = CmdListHelper::GetColorFilterFromCmdList(*cmdList, handle);

    const std::shared_ptr<ColorFilter> filter = ColorFilter::CreateSrgbGammaToLinear();
    handle = CmdListHelper::AddColorFilterToCmdList(*cmdList, filter);
    sptr = CmdListHelper::GetColorFilterFromCmdList(*cmdList, handle);
    EXPECT_NE(sptr, nullptr);
}

#ifdef ROSEN_OHOS
/**
 * @tc.name: AddSurfaceBufferEntryToCmdList
 * @tc.desc: Test the AddSurfaceBufferEntryToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddSurfaceBufferToCmdList, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    uint32_t handle = CmdListHelper::AddSurfaceBufferEntryToCmdList(*cmdList, nullptr);
    auto sptr = CmdListHelper::GetSurfaceBufferEntryFromCmdList(*cmdList, handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: SurfaceBuffer
 * @tc.desc: Test the SurfaceBuffer function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, SurfaceBuffer, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::shared_ptr<SurfaceBufferEntry> surfaceBufferEntry = cmdList->GetSurfaceBufferEntry(TEST_INVALIED_ID);
    EXPECT_EQ(surfaceBufferEntry, nullptr);

    uint32_t id = cmdList->AddSurfaceBufferEntry(nullptr);
    surfaceBufferEntry = cmdList->GetSurfaceBufferEntry(id);
    EXPECT_EQ(surfaceBufferEntry, nullptr);
}

/**
 * @tc.name: AllSurfaceBufferEntry
 * @tc.desc: Test the AllSurfaceBufferEntry function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AllSurfaceBuffer, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::vector<std::shared_ptr<SurfaceBufferEntry>> objectList;
    uint32_t id = cmdList->GetAllSurfaceBufferEntry(objectList);

    auto cmdList1 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    id = cmdList1->SetupSurfaceBufferEntry(objectList);
    EXPECT_EQ(id, 0);
}
#endif

/**
 * @tc.name: DrawFuncObj
 * @tc.desc: Test the DrawFuncObj function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, DrawFuncObj, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    uint32_t handle = CmdListHelper::AddDrawFuncObjToCmdList(*cmdList, nullptr);
    EXPECT_EQ(cmdList->GetDrawFuncObj(TEST_INVALIED_ID), nullptr);
    auto sptr = CmdListHelper::GetDrawFuncObjFromCmdList(*cmdList, handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: AddBitmapToCmdList002
 * @tc.desc: Test the AddBitmapToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddBitmapToCmdList002, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Bitmap bitmap;
    BitmapFormat format = { ColorType::COLORTYPE_ARGB_4444, AlphaType::ALPHATYPE_PREMUL };
    bitmap.Build(100, 100, format);
    ImageHandle handel = CmdListHelper::AddBitmapToCmdList(*cmdList, bitmap);
    EXPECT_EQ(handel.offset, 0);

    Bitmap bitmap1;
    BitmapFormat format1 = { ColorType::COLORTYPE_RGBA_F16, AlphaType::ALPHATYPE_PREMUL };
    bitmap1.Build(100, 100, format1);
    handel = CmdListHelper::AddBitmapToCmdList(*cmdList, bitmap1);
    EXPECT_NE(handel.offset, 0);
}

/**
 * @tc.name: Picture
 * @tc.desc: Test the Picture function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, Picture, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Picture picture;
    OpDataHandle handle = CmdListHelper::AddPictureToCmdList(*cmdList, picture);
    auto sptr = CmdListHelper::GetPictureFromCmdList(*cmdList, handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: TextBlob
 * @tc.desc: Test the TextBlob function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, TextBlob, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    OpDataHandle handle = CmdListHelper::AddTextBlobToCmdList(*cmdList, nullptr);
    auto sptr = CmdListHelper::GetTextBlobFromCmdList(*cmdList, handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: Region
 * @tc.desc: Test the Region function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, Region, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Region region;
    OpDataHandle handle = CmdListHelper::AddRegionToCmdList(*cmdList, region);
    auto sptr = CmdListHelper::GetRegionFromCmdList(*cmdList, handle);
    EXPECT_NE(sptr, nullptr);
}

/**
 * @tc.name: Image
 * @tc.desc: Test the Image function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, Image, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Image image;
    OpDataHandle handle = cmdList->AddImage(image);
    auto sptr = cmdList->GetImage(handle);
    EXPECT_EQ(sptr, nullptr);
    handle.offset = TEST_INVALIED_ID;
    sptr = cmdList->GetImage(handle);
    EXPECT_EQ(sptr, nullptr);

    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image1;
    image1.BuildFromBitmap(bmp);
    handle = cmdList->AddImage(image);
    handle = cmdList->AddImage(image);
    sptr = cmdList->GetImage(handle);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: SetUpBitmapData
 * @tc.desc: Test the SetUpBitmapData function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, SetUpBitmapData, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    char str[TEST_MEM_SIZE];
    bool set = cmdList->SetUpBitmapData(static_cast<void *>(str), TEST_MEM_SIZE);
    EXPECT_EQ(set, true);
}

/**
 * @tc.name: AllObject
 * @tc.desc: Test the AllObject function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AllObject, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::vector<std::shared_ptr<ExtendImageObject>> objectList;
    uint32_t id = cmdList->GetAllObject(objectList);

    auto cmdList1 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    id = cmdList1->SetupObject(objectList);
    EXPECT_EQ(id, 0);
}

/**
 * @tc.name: AllBaseObj
 * @tc.desc: Test the AllBaseObj function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AllBaseObj, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::vector<std::shared_ptr<ExtendImageBaseObj>> objectList;
    uint32_t id = cmdList->GetAllBaseObj(objectList);

    auto cmdList1 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    id = cmdList1->SetupBaseObj(objectList);
    EXPECT_EQ(id, 0);
}

/**
 * @tc.name: CopyObjectTo
 * @tc.desc: Test the CopyObjectTo function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, CopyObjectTo, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    (void)cmdList->GetOpCnt();
    auto cmdList1 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    cmdList->CopyObjectTo(*cmdList1);
    EXPECT_NE(cmdList1, nullptr);
}

/**
 * @tc.name: AddCmdListData
 * @tc.desc: Test the AddCmdListData function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddCmdListData, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    CmdListData listData = std::make_pair<const void*, size_t>(nullptr, 0);
    cmdList->AddCmdListData(listData);
    EXPECT_NE(cmdList, nullptr);
}

/**
 * @tc.name: AddImageData
 * @tc.desc: Test the AddImageData function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddImageData, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    EXPECT_EQ(cmdList->AddImageData(nullptr, 0), 0);
}

/**
 * @tc.name: AddBitmapData
 * @tc.desc: Test the AddBitmapData function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddBitmapData, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    EXPECT_EQ(cmdList->AddBitmapData(nullptr, 0), 0);
}

/**
 * @tc.name: GetImageObject
 * @tc.desc: Test the GetImageObject function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, GetImageObject, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    EXPECT_EQ(cmdList->GetImageObject(TEST_INVALIED_ID), nullptr);
}

/*
 * @tc.name: ShaderEffectLazyMarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for Lazy ShaderEffect using RSMarshallingHelper
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyMarshallingRoundTrip001, TestSize.Level1)
{
    // Create RecordingCanvas with IMMEDIATE mode to generate valid DrawCmdList
    RecordingCanvas canvas(800, 600, true);
    // Create Lazy ShaderEffect
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    // Create a Brush with the Lazy ShaderEffect and perform actual drawing
    Brush brush;
    brush.SetShaderEffect(lazyShader);
    canvas.AttachBrush(brush);
    // Perform actual drawing operation to populate the CmdList
    Rect rect(0, 0, 100, 100);
    canvas.DrawRect(rect);

    // Get the populated DrawCmdList
    auto originalCmdList = canvas.GetDrawCmdList();
    ASSERT_NE(originalCmdList, nullptr);
    EXPECT_FALSE(originalCmdList->IsEmpty());

    // Serialize using RSMarshallingHelper (should work now with registered callbacks)
    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, originalCmdList, 0);
    EXPECT_TRUE(marshalResult);

    // Verify marshalled data exists
    size_t originalDataSize = parcel.GetDataSize();
    EXPECT_GT(originalDataSize, 0);

    // Deserialize using RSMarshallingHelper
    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);
    ASSERT_NE(deserializedCmdList, nullptr);
    // Verify the deserialized CmdList is not empty
    EXPECT_FALSE(deserializedCmdList->IsEmpty());
}

/*
 * @tc.name: ShaderEffectLazyBasicFunctionality001
 * @tc.desc: Test basic lazy shader functionality without complex operations
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyBasicFunctionality001, TestSize.Level1)
{
    // Test 1: Verify lazy shader creation and basic properties
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);

    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());
    EXPECT_EQ(lazyShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Test 2: Verify materialization works correctly
    auto materializedShader = lazyShader->Materialize();
    ASSERT_NE(materializedShader, nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);

    // Test 3: Test normal (non-lazy) shader for comparison
    auto normalShader = ShaderEffect::CreateColorShader(0xFF00FFFF);
    ASSERT_NE(normalShader, nullptr);
    EXPECT_FALSE(normalShader->IsLazy());
    EXPECT_EQ(normalShader->GetType(), ShaderEffect::ShaderEffectType::COLOR_SHADER);
}

/*
 * @tc.name: ShaderEffectLazyDirectRoundTrip001
 * @tc.desc: Test direct AddShaderEffectToCmdList and GetShaderEffectFromCmdList for Lazy ShaderEffect
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyDirectRoundTrip001, TestSize.Level1)
{
    // Create a simple DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create Lazy ShaderEffect
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    ASSERT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    // Add ShaderEffect to CmdList - this tests AddShaderEffectToCmdList lazy handling
    FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, lazyShader);
    EXPECT_EQ(handle.type, static_cast<uint32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER));
    // First add might return offset 0, let's test with multiple adds
    FlattenableHandle handle2 = CmdListHelper::AddShaderEffectToCmdList(*cmdList, lazyShader);
    EXPECT_EQ(handle2.type, static_cast<uint32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER));

    // The first add might be at offset 0, so let's check the second add has higher offset
    EXPECT_GE(handle2.offset, handle.offset);

    // Retrieve ShaderEffect from CmdList - this tests GetShaderEffectFromCmdList lazy handling
    auto retrievedShader = CmdListHelper::GetShaderEffectFromCmdList(*cmdList, handle);
    ASSERT_NE(retrievedShader, nullptr);
    EXPECT_FALSE(retrievedShader->IsLazy()); // Should be materialized
    EXPECT_EQ(retrievedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
}

/*
 * @tc.name: ShaderEffectLazyDataComparison001
 * @tc.desc: Test memory content comparison for Lazy ShaderEffect Data serialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyDataComparison001, TestSize.Level1)
{
    // Create Lazy ShaderEffect
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto originalLazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    ASSERT_NE(originalLazyShader, nullptr);

    // Get materialized version for Data comparison
    auto originalMaterialized = originalLazyShader->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);

    // Serialize original materialized shader to Data
    auto originalData = originalMaterialized->Serialize();
    ASSERT_NE(originalData, nullptr);
    ASSERT_GT(originalData->GetSize(), 0);

    // Round trip through CmdList + RSMarshallingHelper
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, originalLazyShader);

    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, cmdList, 0);
    EXPECT_TRUE(marshalResult);

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);

    // Retrieve and serialize the round-trip result
    auto retrievedShader = CmdListHelper::GetShaderEffectFromCmdList(*deserializedCmdList, handle);
    ASSERT_NE(retrievedShader, nullptr);

    auto retrievedData = retrievedShader->Serialize();
    ASSERT_NE(retrievedData, nullptr);
    ASSERT_GT(retrievedData->GetSize(), 0);

    // Compare Data memory content
    EXPECT_EQ(originalData->GetSize(), retrievedData->GetSize());

    const uint8_t* originalBytes = reinterpret_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* retrievedBytes = reinterpret_cast<const uint8_t*>(retrievedData->GetData());

    ASSERT_NE(originalBytes, nullptr);
    ASSERT_NE(retrievedBytes, nullptr);

    // Compare memory content using memcmp
    int memcmpResult = memcmp(originalBytes, retrievedBytes, originalData->GetSize());
    EXPECT_EQ(memcmpResult, 0);
}

/*
 * @tc.name: ShaderEffectNestedLazyDataIntegrity001
 * @tc.desc: Test data integrity for nested Lazy ShaderEffect through complete marshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectNestedLazyDataIntegrity001, TestSize.Level1)
{
    // Create nested Lazy ShaderEffect
    auto baseShader1 = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto baseShader2 = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader1 = ShaderEffectLazy::CreateBlendShader(baseShader1, baseShader2, BlendMode::SRC_OVER);
    auto lazyShader2 = ShaderEffectLazy::CreateBlendShader(lazyShader1, baseShader1, BlendMode::MULTIPLY);

    ASSERT_NE(lazyShader2, nullptr);

    // Establish baseline: materialize and serialize original
    auto originalMaterialized = lazyShader2->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);
    auto originalData = originalMaterialized->Serialize();
    ASSERT_NE(originalData, nullptr);
    ASSERT_GT(originalData->GetSize(), 0);

    // Multiple round trips to test stability
    std::shared_ptr<ShaderEffect> currentShader = lazyShader2;

    for (int round = 0; round < 3; ++round) {
        // Create new CmdList for this round
        auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
        FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, currentShader);

        // Full marshalling round trip
        MessageParcel parcel;
        EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, cmdList, 0));

        std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
        EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList));

        // Retrieve and verify
        auto roundTripShader = CmdListHelper::GetShaderEffectFromCmdList(*deserializedCmdList, handle);
        ASSERT_NE(roundTripShader, nullptr);
        EXPECT_FALSE(roundTripShader->IsLazy());

        // Serialize and compare Data
        auto roundTripData = roundTripShader->Serialize();
        ASSERT_NE(roundTripData, nullptr);
        ASSERT_GT(roundTripData->GetSize(), 0);
        EXPECT_EQ(originalData->GetSize(), roundTripData->GetSize());

        // Memory content should remain identical after multiple round trips
        int memcmpResult = memcmp(originalData->GetData(), roundTripData->GetData(), originalData->GetSize());
        EXPECT_EQ(memcmpResult, 0);

        // Prepare for next round
        currentShader = roundTripShader;
    }
}

/*
 * @tc.name: ShaderEffectLazyBatchDataIntegrity001
 * @tc.desc: Test data integrity for batch of different Lazy ShaderEffect types
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyBatchDataIntegrity001, TestSize.Level1)
{
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);

    // Create multiple different Lazy ShaderEffects
    std::vector<std::shared_ptr<ShaderEffectLazy>> lazyShaders;
    std::vector<std::shared_ptr<Data>> originalDataList;
    std::vector<FlattenableHandle> handles;

    // Type 1: Simple BlendShader
    auto shader1 = ShaderEffectLazy::CreateBlendShader(
        ShaderEffect::CreateColorShader(0xFF0000FF),
        ShaderEffect::CreateColorShader(0xFF00FF00),
        BlendMode::SRC_OVER
    );
    lazyShaders.push_back(shader1);

    // Type 2: Different BlendMode
    auto shader2 = ShaderEffectLazy::CreateBlendShader(
        ShaderEffect::CreateColorShader(0xFFFF0000),
        ShaderEffect::CreateColorShader(0xFF0000FF),
        BlendMode::MULTIPLY
    );
    lazyShaders.push_back(shader2);

    // Collect original data and add to CmdList
    for (auto& lazyShader : lazyShaders) {
        ASSERT_NE(lazyShader, nullptr);

        auto materialized = lazyShader->Materialize();
        ASSERT_NE(materialized, nullptr);

        auto originalData = materialized->Serialize();
        ASSERT_NE(originalData, nullptr);
        ASSERT_GT(originalData->GetSize(), 0);
        originalDataList.push_back(originalData);

        FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, lazyShader);
        handles.push_back(handle);
    }

    // Full marshalling round trip
    MessageParcel parcel;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, cmdList, 0));

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList));

    // Verify each shader's data integrity
    for (size_t i = 0; i < lazyShaders.size(); ++i) {
        auto retrievedShader = CmdListHelper::GetShaderEffectFromCmdList(*deserializedCmdList, handles[i]);
        ASSERT_NE(retrievedShader, nullptr);
        EXPECT_FALSE(retrievedShader->IsLazy());

        auto retrievedData = retrievedShader->Serialize();
        ASSERT_NE(retrievedData, nullptr);
        ASSERT_GT(retrievedData->GetSize(), 0);

        auto& originalData = originalDataList[i];
        EXPECT_EQ(originalData->GetSize(), retrievedData->GetSize());

        int memcmpResult = memcmp(originalData->GetData(), retrievedData->GetData(), originalData->GetSize());
        EXPECT_EQ(memcmpResult, 0);
    }
}

/*
 * @tc.name: ShaderEffectLazyVsNormalMarshalling001
 * @tc.desc: Compare complete marshalling process between Lazy and Normal ShaderEffect
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyVsNormalMarshalling001, TestSize.Level1)
{
    // Create shared DrawCmdList for both shaders
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    // Test Normal ShaderEffect
    auto normalShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto originalNormalData = normalShader->Serialize();
    ASSERT_NE(originalNormalData, nullptr);
    ASSERT_GT(originalNormalData->GetSize(), 0);
    FlattenableHandle normalHandle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, normalShader);

    // Test Lazy ShaderEffect
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    auto originalLazyMaterialized = lazyShader->Materialize();
    ASSERT_NE(originalLazyMaterialized, nullptr);
    auto originalLazyData = originalLazyMaterialized->Serialize();
    ASSERT_NE(originalLazyData, nullptr);
    ASSERT_GT(originalLazyData->GetSize(), 0);
    FlattenableHandle lazyHandle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, lazyShader);

    // Shared marshalling/unmarshalling process
    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, cmdList, 0);
    EXPECT_TRUE(marshalResult);
    EXPECT_GT(parcel.GetDataSize(), 0);

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);
    ASSERT_NE(deserializedCmdList, nullptr);

    // Verify both shaders can be retrieved and compare data integrity
    auto retrievedNormal = CmdListHelper::GetShaderEffectFromCmdList(*deserializedCmdList, normalHandle);
    auto retrievedLazy = CmdListHelper::GetShaderEffectFromCmdList(*deserializedCmdList, lazyHandle);

    ASSERT_NE(retrievedNormal, nullptr);
    ASSERT_NE(retrievedLazy, nullptr);
    EXPECT_FALSE(retrievedNormal->IsLazy());
    EXPECT_FALSE(retrievedLazy->IsLazy());

    // Compare normal shader: original vs retrieved after marshalling
    auto retrievedNormalData = retrievedNormal->Serialize();
    ASSERT_NE(retrievedNormalData, nullptr);
    ASSERT_GT(retrievedNormalData->GetSize(), 0);
    EXPECT_EQ(originalNormalData->GetSize(), retrievedNormalData->GetSize());
    const uint8_t* originalNormalBytes = reinterpret_cast<const uint8_t*>(originalNormalData->GetData());
    const uint8_t* retrievedNormalBytes = reinterpret_cast<const uint8_t*>(retrievedNormalData->GetData());
    ASSERT_NE(originalNormalBytes, nullptr);
    ASSERT_NE(retrievedNormalBytes, nullptr);
    int normalMemcmpResult = memcmp(originalNormalBytes, retrievedNormalBytes, originalNormalData->GetSize());
    EXPECT_EQ(normalMemcmpResult, 0);

    // Compare lazy shader: original materialized vs retrieved after marshalling
    auto retrievedLazyData = retrievedLazy->Serialize();
    ASSERT_NE(retrievedLazyData, nullptr);
    ASSERT_GT(retrievedLazyData->GetSize(), 0);
    EXPECT_EQ(originalLazyData->GetSize(), retrievedLazyData->GetSize());
    const uint8_t* originalLazyBytes = reinterpret_cast<const uint8_t*>(originalLazyData->GetData());
    const uint8_t* retrievedLazyBytes = reinterpret_cast<const uint8_t*>(retrievedLazyData->GetData());
    ASSERT_NE(originalLazyBytes, nullptr);
    ASSERT_NE(retrievedLazyBytes, nullptr);
    int lazyMemcmpResult = memcmp(originalLazyBytes, retrievedLazyBytes, originalLazyData->GetSize());
    EXPECT_EQ(lazyMemcmpResult, 0);
}

/*
 * @tc.name: ShaderEffectLazyNullObjHandling001
 * @tc.desc: Test AddShaderEffectToCmdList with ShaderEffectLazy that has null shaderEffectObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ShaderEffectLazyNullObjHandling001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a ShaderEffectLazy with null shaderEffectObj using CreateForUnmarshalling()
    auto lazyShaderWithNullObj = ShaderEffectLazy::CreateForUnmarshalling();
    ASSERT_NE(lazyShaderWithNullObj, nullptr);
    EXPECT_TRUE(lazyShaderWithNullObj->IsLazy());
    // Verify that GetShaderEffectObj() returns nullptr
    EXPECT_EQ(lazyShaderWithNullObj->GetShaderEffectObj(), nullptr);

    // This should trigger the (!shaderEffectObj) branch and return { 0 }
    FlattenableHandle handle = CmdListHelper::AddShaderEffectToCmdList(*cmdList, lazyShaderWithNullObj);
    // Verify the expected return value when shaderEffectObj is null
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);
    EXPECT_EQ(handle.type, 0);
}

/*
 * @tc.name: ImageFilterLazyNullObjHandling001
 * @tc.desc: Test AddImageFilterToCmdList with ImageFilterLazy that has null imageFilterObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyNullObjHandling001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a ImageFilterLazy with null imageFilterObj using default constructor
    auto lazyFilterWithNullObj = std::shared_ptr<ImageFilterLazy>(new ImageFilterLazy());
    ASSERT_NE(lazyFilterWithNullObj, nullptr);
    EXPECT_TRUE(lazyFilterWithNullObj->IsLazy());
    // Verify that GetImageFilterObj() returns nullptr
    EXPECT_EQ(lazyFilterWithNullObj->GetImageFilterObj(), nullptr);

    // This should trigger the (!imageFilterObj) branch and return { 0 }
    FlattenableHandle handle = CmdListHelper::AddImageFilterToCmdList(*cmdList, lazyFilterWithNullObj);
    // Verify the expected return value when imageFilterObj is null
    EXPECT_EQ(handle.offset, 0);
    EXPECT_EQ(handle.size, 0);
    EXPECT_EQ(handle.type, 0);
}

/*
 * @tc.name: GetShaderEffectFromCmdListNullDrawingObj001
 * @tc.desc: Test GetShaderEffectFromCmdList with invalid offset causing null drawingObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, GetShaderEffectFromCmdListNullDrawingObj001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a FlattenableHandle with LAZY_SHADER type but invalid offset
    FlattenableHandle invalidHandle;
    invalidHandle.offset = 9999; // Use a large invalid offset
    invalidHandle.size = 1;
    invalidHandle.type = static_cast<uint32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // This should trigger the (!drawingObj) branch and return nullptr
    auto retrievedShader = CmdListHelper::GetShaderEffectFromCmdList(*cmdList, invalidHandle);
    // Verify the expected return value when drawingObj is null
    EXPECT_EQ(retrievedShader, nullptr);
}

/*
 * @tc.name: GetImageFilterFromCmdListNullDrawingObj001
 * @tc.desc: Test GetImageFilterFromCmdList with invalid offset causing null drawingObj
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, GetImageFilterFromCmdListNullDrawingObj001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a FlattenableHandle with LAZY_IMAGE_FILTER type but invalid offset
    FlattenableHandle invalidHandle;
    invalidHandle.offset = 9999; // Use a large invalid offset
    invalidHandle.size = 1;
    invalidHandle.type = static_cast<uint32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // This should trigger the (!drawingObj) branch and return nullptr
    auto retrievedFilter = CmdListHelper::GetImageFilterFromCmdList(*cmdList, invalidHandle);
    // Verify the expected return value when drawingObj is null
    EXPECT_EQ(retrievedFilter, nullptr);
}

/*
 * @tc.name: ImageFilterLazyMarshallingRoundTrip001
 * @tc.desc: Test complete marshalling/unmarshalling round trip for Lazy ImageFilter using RSMarshallingHelper
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyMarshallingRoundTrip001, TestSize.Level1)
{
    // Create RecordingCanvas with IMMEDIATE mode to generate valid DrawCmdList
    RecordingCanvas canvas(800, 600, true);

    // Create Lazy ImageFilter
    auto lazyFilter = ImageFilterLazy::CreateBlur(5.0f, 5.0f, TileMode::CLAMP, nullptr);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // Create a Brush with the Lazy ImageFilter and perform actual drawing
    Brush brush;
    Filter filter;
    filter.SetImageFilter(lazyFilter);
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);

    // Perform actual drawing operation to populate the CmdList
    Rect rect(0, 0, 100, 100);
    canvas.DrawRect(rect);

    // Get the populated DrawCmdList
    auto originalCmdList = canvas.GetDrawCmdList();
    ASSERT_NE(originalCmdList, nullptr);
    EXPECT_FALSE(originalCmdList->IsEmpty());

    // Serialize using RSMarshallingHelper
    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, originalCmdList, 0);
    EXPECT_TRUE(marshalResult);

    // Verify marshalled data exists
    size_t originalDataSize = parcel.GetDataSize();
    EXPECT_GT(originalDataSize, 0);

    // Deserialize using RSMarshallingHelper
    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);
    ASSERT_NE(deserializedCmdList, nullptr);

    // Verify the deserialized CmdList is not empty
    EXPECT_FALSE(deserializedCmdList->IsEmpty());
}

/*
 * @tc.name: ImageFilterLazyBasicFunctionality001
 * @tc.desc: Test basic lazy image filter functionality without complex operations
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyBasicFunctionality001, TestSize.Level1)
{
    // Test 1: Verify lazy image filter creation and basic properties
    auto lazyFilter = ImageFilterLazy::CreateBlur(3.0f, 3.0f, TileMode::CLAMP, nullptr);

    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());
    EXPECT_EQ(lazyFilter->GetType(), ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // Test 2: Verify materialization works correctly
    auto materializedFilter = lazyFilter->Materialize();
    ASSERT_NE(materializedFilter, nullptr);
    EXPECT_FALSE(materializedFilter->IsLazy());
    EXPECT_EQ(materializedFilter->GetType(), ImageFilter::FilterType::BLUR);

    // Test 3: Test normal (non-lazy) image filter for comparison
    auto normalFilter = ImageFilter::CreateBlurImageFilter(2.0f, 2.0f, TileMode::CLAMP, nullptr);
    ASSERT_NE(normalFilter, nullptr);
    EXPECT_FALSE(normalFilter->IsLazy());
    EXPECT_EQ(normalFilter->GetType(), ImageFilter::FilterType::BLUR);
}

/*
 * @tc.name: ImageFilterLazyDirectRoundTrip001
 * @tc.desc: Test direct AddImageFilterToCmdList and GetImageFilterFromCmdList for Lazy ImageFilter
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyDirectRoundTrip001, TestSize.Level1)
{
    // Create a simple DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create Lazy ImageFilter
    auto lazyFilter = ImageFilterLazy::CreateBlur(4.0f, 4.0f, TileMode::REPEAT, nullptr);
    ASSERT_NE(lazyFilter, nullptr);
    EXPECT_TRUE(lazyFilter->IsLazy());

    // Add ImageFilter to CmdList - this tests AddImageFilterToCmdList lazy handling
    FlattenableHandle handle = CmdListHelper::AddImageFilterToCmdList(*cmdList, lazyFilter);
    EXPECT_EQ(handle.type, static_cast<uint32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER));
    // First add might return offset 0, let's test with multiple adds
    FlattenableHandle handle2 = CmdListHelper::AddImageFilterToCmdList(*cmdList, lazyFilter);
    EXPECT_EQ(handle2.type, static_cast<uint32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER));

    // The first add might be at offset 0, so let's check the second add has higher offset
    EXPECT_GE(handle2.offset, handle.offset);

    // Retrieve ImageFilter from CmdList - this tests GetImageFilterFromCmdList lazy handling
    auto retrievedFilter = CmdListHelper::GetImageFilterFromCmdList(*cmdList, handle);
    ASSERT_NE(retrievedFilter, nullptr);
    EXPECT_FALSE(retrievedFilter->IsLazy()); // Should be materialized
    EXPECT_EQ(retrievedFilter->GetType(), ImageFilter::FilterType::BLUR);
}

/*
 * @tc.name: ImageFilterLazyDataComparison001
 * @tc.desc: Test memory content comparison for Lazy ImageFilter Data serialization
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyDataComparison001, TestSize.Level1)
{
    // Create Lazy ImageFilter
    auto originalLazyFilter = ImageFilterLazy::CreateBlur(6.0f, 6.0f, TileMode::MIRROR, nullptr);
    ASSERT_NE(originalLazyFilter, nullptr);

    // Get materialized version for Data comparison
    auto originalMaterialized = originalLazyFilter->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);

    // Serialize original materialized filter to Data
    auto originalData = originalMaterialized->Serialize();
    ASSERT_NE(originalData, nullptr);
    ASSERT_GT(originalData->GetSize(), 0);

    // Round trip through CmdList + RSMarshallingHelper
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    FlattenableHandle handle = CmdListHelper::AddImageFilterToCmdList(*cmdList, originalLazyFilter);

    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, cmdList, 0);
    EXPECT_TRUE(marshalResult);

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);

    // Retrieve and serialize the round-trip result
    auto retrievedFilter = CmdListHelper::GetImageFilterFromCmdList(*deserializedCmdList, handle);
    ASSERT_NE(retrievedFilter, nullptr);

    auto retrievedData = retrievedFilter->Serialize();
    ASSERT_NE(retrievedData, nullptr);
    ASSERT_GT(retrievedData->GetSize(), 0);

    // Compare Data memory content
    EXPECT_EQ(originalData->GetSize(), retrievedData->GetSize());

    const uint8_t* originalBytes = reinterpret_cast<const uint8_t*>(originalData->GetData());
    const uint8_t* retrievedBytes = reinterpret_cast<const uint8_t*>(retrievedData->GetData());

    ASSERT_NE(originalBytes, nullptr);
    ASSERT_NE(retrievedBytes, nullptr);

    // Compare memory content using memcmp
    int memcmpResult = memcmp(originalBytes, retrievedBytes, originalData->GetSize());
    EXPECT_EQ(memcmpResult, 0);
}

/*
 * @tc.name: ImageFilterNestedLazyDataIntegrity001
 * @tc.desc: Test data integrity for nested Lazy ImageFilter through complete marshalling
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterNestedLazyDataIntegrity001, TestSize.Level1)
{
    // Create nested Lazy ImageFilter
    auto lazyFilter1 = ImageFilterLazy::CreateBlur(2.0f, 2.0f, TileMode::CLAMP, nullptr);
    auto lazyFilter2 = ImageFilterLazy::CreateBlur(4.0f, 4.0f, TileMode::REPEAT, lazyFilter1);
    ASSERT_NE(lazyFilter2, nullptr);

    // Establish baseline: materialize and serialize original
    auto originalMaterialized = lazyFilter2->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);
    auto originalData = originalMaterialized->Serialize();
    ASSERT_NE(originalData, nullptr);
    ASSERT_GT(originalData->GetSize(), 0);

    // Multiple round trips to test stability
    std::shared_ptr<ImageFilter> currentFilter = lazyFilter2;

    for (int round = 0; round < 3; ++round) {
        // Create new CmdList for this round
        auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
        FlattenableHandle handle = CmdListHelper::AddImageFilterToCmdList(*cmdList, currentFilter);

        // Full marshalling round trip
        MessageParcel parcel;
        EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, cmdList, 0));

        std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
        EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList));

        // Retrieve and verify
        auto roundTripFilter = CmdListHelper::GetImageFilterFromCmdList(*deserializedCmdList, handle);
        ASSERT_NE(roundTripFilter, nullptr);
        EXPECT_FALSE(roundTripFilter->IsLazy());

        // Serialize and compare Data
        auto roundTripData = roundTripFilter->Serialize();
        ASSERT_NE(roundTripData, nullptr);
        ASSERT_GT(roundTripData->GetSize(), 0);
        EXPECT_EQ(originalData->GetSize(), roundTripData->GetSize());

        // Memory content should remain identical after multiple round trips
        int memcmpResult = memcmp(originalData->GetData(), roundTripData->GetData(), originalData->GetSize());
        EXPECT_EQ(memcmpResult, 0);

        // Prepare for next round
        currentFilter = roundTripFilter;
    }
}

/*
 * @tc.name: ImageFilterLazyBatchDataIntegrity001
 * @tc.desc: Test data integrity for batch of different Lazy ImageFilter types
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyBatchDataIntegrity001, TestSize.Level1)
{
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);

    // Create multiple different Lazy ImageFilters
    std::vector<std::shared_ptr<ImageFilterLazy>> lazyFilters;
    std::vector<std::shared_ptr<Data>> originalDataList;
    std::vector<FlattenableHandle> handles;

    // Type 1: Blur ImageFilter
    auto filter1 = ImageFilterLazy::CreateBlur(3.0f, 3.0f, TileMode::CLAMP, nullptr);
    lazyFilters.push_back(filter1);

    // Type 2: Offset ImageFilter
    auto filter2 = ImageFilterLazy::CreateOffset(10.0f, 15.0f, nullptr);
    lazyFilters.push_back(filter2);

    // Collect original data and add to CmdList
    for (auto& lazyFilter : lazyFilters) {
        ASSERT_NE(lazyFilter, nullptr);

        auto materialized = lazyFilter->Materialize();
        ASSERT_NE(materialized, nullptr);

        auto originalData = materialized->Serialize();
        ASSERT_NE(originalData, nullptr);
        ASSERT_GT(originalData->GetSize(), 0);
        originalDataList.push_back(originalData);

        FlattenableHandle handle = CmdListHelper::AddImageFilterToCmdList(*cmdList, lazyFilter);
        handles.push_back(handle);
    }

    // Full marshalling round trip
    MessageParcel parcel;
    EXPECT_TRUE(RSMarshallingHelper::Marshalling(parcel, cmdList, 0));

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    EXPECT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList));

    // Verify each filter's data integrity
    for (size_t i = 0; i < lazyFilters.size(); ++i) {
        auto retrievedFilter = CmdListHelper::GetImageFilterFromCmdList(*deserializedCmdList, handles[i]);
        ASSERT_NE(retrievedFilter, nullptr);
        EXPECT_FALSE(retrievedFilter->IsLazy());

        auto retrievedData = retrievedFilter->Serialize();
        ASSERT_NE(retrievedData, nullptr);
        ASSERT_GT(retrievedData->GetSize(), 0);

        auto& originalData = originalDataList[i];
        EXPECT_EQ(originalData->GetSize(), retrievedData->GetSize());

        int memcmpResult = memcmp(originalData->GetData(), retrievedData->GetData(), originalData->GetSize());
        EXPECT_EQ(memcmpResult, 0);
    }
}

/*
 * @tc.name: ImageFilterLazyVsNormalMarshalling001
 * @tc.desc: Compare complete marshalling process between Lazy and Normal ImageFilter
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, ImageFilterLazyVsNormalMarshalling001, TestSize.Level1)
{
    // Create shared DrawCmdList for both filters
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    // Test Normal ImageFilter
    auto normalFilter = ImageFilter::CreateBlurImageFilter(3.0f, 3.0f, TileMode::CLAMP, nullptr);
    auto originalNormalData = normalFilter->Serialize();
    ASSERT_NE(originalNormalData, nullptr);
    ASSERT_GT(originalNormalData->GetSize(), 0);
    FlattenableHandle normalHandle = CmdListHelper::AddImageFilterToCmdList(*cmdList, normalFilter);

    // Test Lazy ImageFilter
    auto lazyFilter = ImageFilterLazy::CreateBlur(3.0f, 3.0f, TileMode::CLAMP, nullptr);
    auto originalLazyMaterialized = lazyFilter->Materialize();
    ASSERT_NE(originalLazyMaterialized, nullptr);
    auto originalLazyData = originalLazyMaterialized->Serialize();
    ASSERT_NE(originalLazyData, nullptr);
    ASSERT_GT(originalLazyData->GetSize(), 0);
    FlattenableHandle lazyHandle = CmdListHelper::AddImageFilterToCmdList(*cmdList, lazyFilter);

    // Shared marshalling/unmarshalling process
    MessageParcel parcel;
    bool marshalResult = RSMarshallingHelper::Marshalling(parcel, cmdList, 0);
    EXPECT_TRUE(marshalResult);
    EXPECT_GT(parcel.GetDataSize(), 0);

    std::shared_ptr<Drawing::DrawCmdList> deserializedCmdList;
    bool unmarshalResult = RSMarshallingHelper::Unmarshalling(parcel, deserializedCmdList);
    EXPECT_TRUE(unmarshalResult);
    ASSERT_NE(deserializedCmdList, nullptr);

    // Verify both filters can be retrieved and compare data integrity
    auto retrievedNormal = CmdListHelper::GetImageFilterFromCmdList(*deserializedCmdList, normalHandle);
    auto retrievedLazy = CmdListHelper::GetImageFilterFromCmdList(*deserializedCmdList, lazyHandle);

    ASSERT_NE(retrievedNormal, nullptr);
    ASSERT_NE(retrievedLazy, nullptr);
    EXPECT_FALSE(retrievedNormal->IsLazy());
    EXPECT_FALSE(retrievedLazy->IsLazy());

    // Compare normal filter: original vs retrieved after marshalling
    auto retrievedNormalData = retrievedNormal->Serialize();
    ASSERT_NE(retrievedNormalData, nullptr);
    ASSERT_GT(retrievedNormalData->GetSize(), 0);
    EXPECT_EQ(originalNormalData->GetSize(), retrievedNormalData->GetSize());
    const uint8_t* originalNormalBytes = reinterpret_cast<const uint8_t*>(originalNormalData->GetData());
    const uint8_t* retrievedNormalBytes = reinterpret_cast<const uint8_t*>(retrievedNormalData->GetData());
    ASSERT_NE(originalNormalBytes, nullptr);
    ASSERT_NE(retrievedNormalBytes, nullptr);
    int normalMemcmpResult = memcmp(originalNormalBytes, retrievedNormalBytes, originalNormalData->GetSize());
    EXPECT_EQ(normalMemcmpResult, 0);

    // Compare lazy filter: original materialized vs retrieved after marshalling
    auto retrievedLazyData = retrievedLazy->Serialize();
    ASSERT_NE(retrievedLazyData, nullptr);
    ASSERT_GT(retrievedLazyData->GetSize(), 0);
    EXPECT_EQ(originalLazyData->GetSize(), retrievedLazyData->GetSize());
    const uint8_t* originalLazyBytes = reinterpret_cast<const uint8_t*>(originalLazyData->GetData());
    const uint8_t* retrievedLazyBytes = reinterpret_cast<const uint8_t*>(retrievedLazyData->GetData());
    ASSERT_NE(originalLazyBytes, nullptr);
    ASSERT_NE(retrievedLazyBytes, nullptr);
    int lazyMemcmpResult = memcmp(originalLazyBytes, retrievedLazyBytes, originalLazyData->GetSize());
    EXPECT_EQ(lazyMemcmpResult, 0);
}

/*
 * @tc.name: GetShaderEffectFromCmdListWrongObjectType001
 * @tc.desc: Test GetShaderEffectFromCmdList with wrong object type to cover type mismatch branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, GetShaderEffectFromCmdListWrongObjectType001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a DrawingObject with IMAGE_FILTER type instead of SHADER_EFFECT type
    class MockImageFilterObj : public Object {
    public:
        MockImageFilterObj() : Object(Object::ObjectType::IMAGE_FILTER, 0) {}
        bool Marshalling(Parcel& parcel) override {
            return parcel.WriteInt32(42);
        }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override
        {
            parcel.ReadInt32();
            return true;
        }
        std::shared_ptr<void> GenerateBaseObject() override
        {
            return nullptr;
        }
    };

    // Add the wrong-type object to DrawCmdList as DrawingObject
    auto wrongTypeObj = std::make_shared<MockImageFilterObj>();
    uint32_t objIndex = CmdListHelper::AddDrawingObjectToCmdList(*cmdList, wrongTypeObj);

    // Create a FlattenableHandle with LAZY_SHADER type that points to the wrong-type object
    FlattenableHandle handle;
    handle.offset = objIndex;
    handle.size = 1;
    handle.type = static_cast<uint32_t>(ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // This should trigger the type mismatch branch:
    // drawingObj->GetType() != static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT)
    auto retrievedShader = CmdListHelper::GetShaderEffectFromCmdList(*cmdList, handle);

    // Verify the expected return value when object type doesn't match
    EXPECT_EQ(retrievedShader, nullptr);
}

/*
 * @tc.name: GetImageFilterFromCmdListWrongObjectType001
 * @tc.desc: Test GetImageFilterFromCmdList with wrong object type to cover type mismatch branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CmdListHelperTest, GetImageFilterFromCmdListWrongObjectType001, TestSize.Level1)
{
    // Create a DrawCmdList for testing
    auto cmdList = std::make_shared<DrawCmdList>(800, 600, DrawCmdList::UnmarshalMode::IMMEDIATE);
    ASSERT_NE(cmdList, nullptr);

    // Create a DrawingObject with SHADER_EFFECT type instead of IMAGE_FILTER type
    class MockShaderEffectObj : public Object {
    public:
        MockShaderEffectObj() : Object(Object::ObjectType::SHADER_EFFECT, 0) {}
        bool Marshalling(Parcel& parcel) override {
            return parcel.WriteInt32(42);
        }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override
        {
            parcel.ReadInt32();
            return true;
        }

        std::shared_ptr<void> GenerateBaseObject() override
        {
            return nullptr;
        }
    };

    // Add the wrong-type object to DrawCmdList as DrawingObject
    auto wrongTypeObj = std::make_shared<MockShaderEffectObj>();
    uint32_t objIndex = CmdListHelper::AddDrawingObjectToCmdList(*cmdList, wrongTypeObj);

    // Create a FlattenableHandle with LAZY_IMAGE_FILTER type that points to the wrong-type object
    FlattenableHandle handle;
    handle.offset = objIndex;
    handle.size = 1;
    handle.type = static_cast<uint32_t>(ImageFilter::FilterType::LAZY_IMAGE_FILTER);

    // This should trigger the type mismatch branch:
    // drawingObj->GetType() != static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER)
    auto retrievedFilter = CmdListHelper::GetImageFilterFromCmdList(*cmdList, handle);

    // Verify the expected return value when object type doesn't match
    EXPECT_EQ(retrievedFilter, nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS