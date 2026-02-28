#include <gtest/gtest.h>
#include <chrono>

#include "CommonTestUtilities.h"
#include "src/Frame/Frame.h"

class CubeViewModeTest : public ::testing::Test {
public:
    void SetUp() override {
        // Use a common test image
        test_root = FileFinder::FitsImagePath();
        ASSERT_FALSE(test_root.empty());
    }

    std::string test_root;
};

TEST_F(CubeViewModeTest, TestSliceCachePerformance) {
    std::string filename = test_root + "/noise_3d.fits";
    auto path_string = fmt::format("{}", filename);
    
    // Load frame
    std::unique_ptr<carta::FileLoader> loader(carta::FileLoader::GetLoader(filename));
    ASSERT_NE(loader, nullptr) << path_string;
    auto frame = std::make_shared<Frame>(0, loader.get(), "0");
    ASSERT_NE(frame, nullptr);

    // Set to YZ cube view mode
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_YZ);
    
    // Get dimensions
    auto dims = frame->ImageShape();
    int height = dims[1];
    int depth = dims[2];
    
    std::vector<float> slice_data;
    
    // First call - should populate cache
    auto start_time = std::chrono::high_resolution_clock::now();
    bool success = frame->GetYZSlice(slice_data, 10, 0);
    auto first_call_time = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(success);
    ASSERT_EQ(slice_data.size(), height * depth);
    
    // Store first result for comparison
    std::vector<float> first_result = slice_data;
    
    // Second call - should use cache
    auto second_start = std::chrono::high_resolution_clock::now();
    success = frame->GetYZSlice(slice_data, 10, 0);
    auto second_call_time = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(success);
    ASSERT_EQ(slice_data.size(), height * depth);
    
    // Verify data is identical
    ASSERT_EQ(slice_data, first_result);
    
    // Calculate timing
    auto first_duration = std::chrono::duration_cast<std::chrono::microseconds>(first_call_time - start_time);
    auto second_duration = std::chrono::duration_cast<std::chrono::microseconds>(second_call_time - second_start);
    
    std::cout << "First call (cache miss): " << first_duration.count() << " us" << std::endl;
    std::cout << "Second call (cache hit): " << second_duration.count() << " us" << std::endl;
    
    // Cache hit should be significantly faster (at least 2x)
    EXPECT_LT(second_duration.count() * 2, first_duration.count()) 
        << "Cache hit should be at least 2x faster than cache miss";
}

TEST_F(CubeViewModeTest, TestSliceCacheInvalidation) {
    std::string filename = test_root + "/noise_3d.fits";
    auto path_string = fmt::format("{}", filename);
    
    // Load frame
    std::unique_ptr<carta::FileLoader> loader(carta::FileLoader::GetLoader(filename));
    ASSERT_NE(loader, nullptr) << path_string;
    auto frame = std::make_shared<Frame>(0, loader.get(), "0");
    ASSERT_NE(frame, nullptr);

    // Set to YZ cube view mode
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_YZ);
    
    std::vector<float> slice_data1, slice_data2;
    
    // Get slice to populate cache
    bool success = frame->GetYZSlice(slice_data1, 10, 0);
    ASSERT_TRUE(success);
    
    // Switch to XZ mode (should invalidate cache)
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_XZ);
    
    // Switch back to YZ mode
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_YZ);
    
    // Get slice again (should regenerate, not use stale cache)
    success = frame->GetYZSlice(slice_data2, 10, 0);
    ASSERT_TRUE(success);
    
    // Data should be identical (same slice parameters)
    ASSERT_EQ(slice_data1, slice_data2);
}

TEST_F(CubeViewModeTest, TestBothSliceTypes) {
    std::string filename = test_root + "/noise_3d.fits";
    auto path_string = fmt::format("{}", filename);
    
    // Load frame
    std::unique_ptr<carta::FileLoader> loader(carta::FileLoader::GetLoader(filename));
    ASSERT_NE(loader, nullptr) << path_string;
    auto frame = std::make_shared<Frame>(0, loader.get(), "0");
    ASSERT_NE(frame, nullptr);

    auto dims = frame->ImageShape();
    int width = dims[0];
    int height = dims[1];
    int depth = dims[2];
    
    std::vector<float> yz_slice_data, xz_slice_data;
    
    // Test YZ slice
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_YZ);
    bool success = frame->GetYZSlice(yz_slice_data, 10, 0);
    ASSERT_TRUE(success);
    ASSERT_EQ(yz_slice_data.size(), height * depth);
    
    // Test XZ slice
    frame->SetCubeViewMode(CARTA::CubeViewMode::VIEW_MODE_XZ);
    success = frame->GetXZSlice(xz_slice_data, 10, 0);
    ASSERT_TRUE(success);
    ASSERT_EQ(xz_slice_data.size(), width * depth);
    
    // Slices should have different sizes (unless width == height)
    if (width != height) {
        EXPECT_NE(yz_slice_data.size(), xz_slice_data.size());
    }
    
    std::cout << "YZ slice size: " << yz_slice_data.size() << " (" << height << "x" << depth << ")" << std::endl;
    std::cout << "XZ slice size: " << xz_slice_data.size() << " (" << width << "x" << depth << ")" << std::endl;
}