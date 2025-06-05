# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication_0202sdk)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED
    hello.cpp
    plugin/plugin_manager.cpp
    my_xcomponent.cpp
    test_base.cpp
    test_common.cpp
    testcasefactory.cpp
    dm/aa_rect_modes.cpp
    dm/blur_circles.cpp
    dm/blur_large_rrects.cpp
    dm/add_arc.cpp
    dm/font_regen.cpp
    dm/alpha_image.cpp
    dm/shadowutils.cpp
    dm/clipped_cubic.cpp
    dm/circular_arcs.cpp
    dm/largeclippedpath.cpp
    dm/filltypes.cpp
    dm/dashing.cpp
    dm/path_interior.cpp
    dm/strokes.cpp
    dm/alpha_gradients.cpp
    dm/convex_paths.cpp
    dm/gradient_dirty_laundry.cpp
    dm/fill_types_persp.cpp
    dm/arcofzorro.cpp
    dm/stroke_rect_shader.cpp
    dm/luma_filter.cpp
    dm/bugfix7792.cpp
    dm/points_mask_filter.cpp
    dm/clip_cubic.cpp
    dm/points.cpp
    dm/onebadarc.cpp
    dm/skbug_8955.cpp
    dm/conic_paths.cpp
    dm/bitmaprect.cpp
    dm/newsurface.cpp
    dm/drawbitmaprect.cpp
    dm/anisotropic.cpp
    function/rect_test.cpp
    function/round_rect_test.cpp
    function/brush_null_test.cpp
    function/canvas_test.cpp
    function/filter_test.cpp
    function/path_test.cpp
    function/path_effect_test.cpp
    function/font_measure_text_test.cpp
    function/function_path_test.cpp
    function/function_pen_test.cpp
    interface/canvas_test.cpp
    interface/text_blob_test.cpp
    interface/path_test.cpp
    interface/bitmap_test.cpp
    interface/color_test.cpp
    interface/matrix_test.cpp
    interface/mask_filter_test.cpp
    interface/shader_effect_test.cpp
    interface/typeface_test.cpp
    interface/pen_test.cpp
    interface/brush_test.cpp
    interface/font_test.cpp
    interface/memory_stream_test.cpp
    interface/sample_option_test.cpp
    interface/surface_test.cpp
    reliability/bitmap_test.cpp
    reliability/pen_exception_test.cpp
    reliability/reliability_brush_test.cpp
    stability/bitmap_test.cpp
    stability/typeface_test.cpp
    stability/text_blob_test.cpp
    stability/surface_test.cpp
    stability/shadow_layer_test.cpp
    stability/shader_effect_test.cpp
    stability/sampling_options_test.cpp
    stability/round_rect_test.cpp
    stability/region_test.cpp
    stability/pen_test.cpp
    stability/mask_filter_test.cpp
    stability/matrix_test.cpp
    stability/memory_stream_test.cpp
    stability/path_effect_test.cpp
    stability/path_test.cpp
    stability/point_test.cpp
    stability/rect_test.cpp
    stability/brush_test.cpp
    stability/canvas_test.cpp
    stability/color_test.cpp
    stability/filter_test.cpp
    stability/surface_test.cpp
    performance/canvas_clip_path_test.cpp
    performance/canvas_draw_image_rect_with_src.cpp
    performance/canvas_draw_image_rect.cpp
    performance/canvas_draw_text_blob_text.cpp
    performance/canvas_draw_shadow_test.cpp
    performance/canvas_draw_performance.cpp
    performance/canvas_draw_performance_c_property.cpp
    performance/canvas_draw_performance_c_operation_switch.cpp
    performance/path_effect_performance.cpp
)

add_library(myxnode SHARED
    my_xnode.cpp
    test_base.cpp
    testcasefactory.cpp
    test_common.cpp
    dm/aa_rect_modes.cpp
    dm/blur_circles.cpp
    dm/blur_large_rrects.cpp
    dm/add_arc.cpp
    dm/font_regen.cpp
    dm/alpha_image.cpp
    dm/shadowutils.cpp
    dm/clipped_cubic.cpp
    dm/circular_arcs.cpp
    dm/largeclippedpath.cpp
    dm/filltypes.cpp
    dm/dashing.cpp
    dm/path_interior.cpp
    dm/strokes.cpp
    dm/alpha_gradients.cpp
    dm/convex_paths.cpp
    dm/gradient_dirty_laundry.cpp
    dm/fill_types_persp.cpp
    dm/arcofzorro.cpp
    dm/stroke_rect_shader.cpp
    dm/luma_filter.cpp
    dm/bugfix7792.cpp
    dm/points_mask_filter.cpp
    dm/clip_cubic.cpp
    dm/points.cpp
    dm/onebadarc.cpp
    dm/skbug_8955.cpp
    dm/conic_paths.cpp
    dm/bitmaprect.cpp
    dm/newsurface.cpp
    dm/drawbitmaprect.cpp
    dm/anisotropic.cpp
    function/rect_test.cpp
    function/round_rect_test.cpp
    function/brush_null_test.cpp
    function/canvas_test.cpp
    function/filter_test.cpp
    function/path_test.cpp
    function/path_effect_test.cpp
    function/font_measure_text_test.cpp
    function/function_path_test.cpp
    function/function_pen_test.cpp
    interface/canvas_test.cpp
    interface/text_blob_test.cpp
    interface/path_test.cpp
    interface/bitmap_test.cpp
    interface/color_test.cpp
    interface/matrix_test.cpp
    interface/mask_filter_test.cpp
    interface/shader_effect_test.cpp
    interface/typeface_test.cpp
    interface/pen_test.cpp
    interface/brush_test.cpp
    interface/font_test.cpp
    interface/memory_stream_test.cpp
    interface/sample_option_test.cpp
    interface/surface_test.cpp
    reliability/bitmap_test.cpp
    reliability/pen_exception_test.cpp
    reliability/reliability_brush_test.cpp
    stability/bitmap_test.cpp
    stability/typeface_test.cpp
    stability/text_blob_test.cpp
    stability/surface_test.cpp
    stability/shadow_layer_test.cpp
    stability/shader_effect_test.cpp
    stability/sampling_options_test.cpp
    stability/round_rect_test.cpp
    stability/region_test.cpp
    stability/pen_test.cpp
    stability/mask_filter_test.cpp
    stability/matrix_test.cpp
    stability/memory_stream_test.cpp
    stability/path_effect_test.cpp
    stability/path_test.cpp
    stability/point_test.cpp
    stability/rect_test.cpp
    stability/brush_test.cpp
    stability/canvas_test.cpp
    stability/color_test.cpp
    stability/filter_test.cpp
    stability/surface_test.cpp
    performance/canvas_clip_path_test.cpp
    performance/canvas_draw_image_rect_with_src.cpp
    performance/canvas_draw_image_rect.cpp
    performance/canvas_draw_text_blob_text.cpp
    performance/canvas_draw_shadow_test.cpp
    performance/canvas_draw_performance.cpp
    performance/canvas_draw_performance_c_property.cpp
    performance/canvas_draw_performance_c_operation_switch.cpp
    performance/path_effect_performance.cpp
)

find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)
target_link_libraries(entry PUBLIC EGL)
target_link_libraries(entry PUBLIC GLESv3)
target_link_libraries(entry PUBLIC ${hilog-lib})
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC libace_ndk.z.so)
target_link_libraries(entry PUBLIC libnative_window.so)
target_link_libraries(entry PUBLIC libnative_drawing.so)
target_link_libraries(entry PUBLIC libimage_packer_ndk.z.so)
target_link_libraries(entry PUBLIC libpixelmap_ndk.z.so)
target_link_libraries(entry PUBLIC libimage_source.so)
target_link_libraries(entry PUBLIC libimage_packer.so)
target_link_libraries(entry PUBLIC libpixelmap.so)
target_link_libraries(entry PUBLIC libnative_color_space_manager.so)


target_link_libraries(myxnode PUBLIC ${hilog-lib})
target_link_libraries(myxnode PUBLIC libace_napi.z.so)
target_link_libraries(myxnode PUBLIC libace_ndk.z.so)
target_link_libraries(myxnode PUBLIC libnative_window.so)
target_link_libraries(myxnode PUBLIC libnative_drawing.so)
target_link_libraries(myxnode PUBLIC libimage_packer_ndk.z.so)
target_link_libraries(myxnode PUBLIC libpixelmap_ndk.z.so)
target_link_libraries(myxnode PUBLIC libimage_source.so)
target_link_libraries(myxnode PUBLIC libimage_packer.so)
target_link_libraries(myxnode PUBLIC libpixelmap.so)
target_link_libraries(myxnode PUBLIC libnative_color_space_manager.so)