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

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "utils/vertices.h"
#include "pixel_map.h"

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
        
        void Playback(Canvas& canvas, const Rect& rect,
            const SamplingOptions& sampling) override {};
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
 * @tc.name: AddSurfaceBufferToCmdList
 * @tc.desc: Test the AddSurfaceBufferToCmdList function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AddSurfaceBufferToCmdList, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    uint32_t handle = CmdListHelper::AddSurfaceBufferToCmdList(*cmdList, nullptr);
    auto sptr = CmdListHelper::GetSurfaceBufferFromCmdList(*cmdList, handle);
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
    sptr<SurfaceBuffer> sptr = cmdList->GetSurfaceBuffer(TEST_INVALIED_ID);
    EXPECT_EQ(sptr, nullptr);

    uint32_t id = cmdList->AddSurfaceBuffer(nullptr);
    sptr = cmdList->GetSurfaceBuffer(id);
    EXPECT_EQ(sptr, nullptr);
}

/**
 * @tc.name: AllSurfaceBuffer
 * @tc.desc: Test the AllSurfaceBuffer function.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(CmdListHelperTest, AllSurfaceBuffer, TestSize.Level1)
{
    auto cmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    std::vector<sptr<SurfaceBuffer>> objectList;
    uint32_t id = cmdList->GetAllSurfaceBuffer(objectList);
    
    auto cmdList1 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    id = cmdList1->SetupSurfaceBuffer(objectList);
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS