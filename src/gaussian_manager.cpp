#include "gaussian_manager.hpp"
// #include "Rendering/ComputePass.hpp"
// #include "Wrapper/Image.hpp"
#include "Helper/CommandManager.hpp"
#include "Wrapper/Buffer.hpp"
#include "Wrapper/Pipeline/Compute_Pipeline.hpp"
#include "Wrapper/Shader_module.hpp"
#include "duplicateWithKeys_pass.hpp"
#include "identify_pass.hpp"
#include "ply_loader.hpp"
#include "precess_pass.hpp"
#include "raster_pass.hpp"
#include "shaders/push_contant.h"
#include "sort_pass.hpp"
#include "sum_pass.hpp"
namespace MCGS {
// using namespace MCRT;

void GaussianManager::Init()
{
    get_gaussian_raw_data();
    render_out.reset(new Image(800,
                               800,
                               // vk::Format::eR32G32B32A32Sfloat,
                               vk::Format::eR8G8B8A8Unorm,
                               vk::ImageType::e2D,
                               vk::ImageTiling::eOptimal,
                               vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
                               vk::ImageAspectFlagBits::eColor,
                               vk::SampleCountFlagBits::e1));
    render_out->SetImageLayout(vk::ImageLayout::eGeneral,
                               vk::AccessFlagBits::eNone,
                               vk::AccessFlagBits::eNone,
                               vk::PipelineStageFlagBits::eTopOfPipe,
                               vk::PipelineStageFlagBits::eBottomOfPipe);

    precess_context.reset(new ProcessPass);
    // precess_context->set_address(address);
    precess_context->Init();
    precess_context->Execute();

    // std::vector<uint64_t> data1(16257710);

    point_list_keyd.resize(1625771, 12);
    point_list_valued.resize(1625771);
    point_list_key = UniformManager::make_uniform(point_list_keyd,
                                                  vk::ShaderStageFlagBits::eCompute,
                                                  vk::DescriptorType::eStorageBuffer,
                                                  vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc);
    point_list_value = UniformManager::make_uniform(point_list_valued,
                                                    vk::ShaderStageFlagBits::eCompute,
                                                    vk::DescriptorType::eStorageBuffer,
                                                    vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc);

    sum_context.reset(new SumPass);
    // sum_context.set_address();
    sum_context->Init();
    duplicate_context.reset(new duplicatePass(point_list_key, point_list_value));
    duplicate_context->Init();

    sort_context.reset(new SortPass(point_list_key, point_list_value));
    sort_context->Init();

    sum_context->Execute();
    duplicate_context->Execute();
    sort_context->Execute();

    identify_content.reset(new IdentifyPass);
    identify_content->Init();
    identify_content->Execute();

    render_content.reset(new RasterPass);
    render_content->Init();
    render_content->Execute();

    std::vector<uint32_t>
        data1(800 * 800);
    std::shared_ptr<Buffer> tempbuffer;
    tempbuffer.reset(new Buffer(data1.size() * sizeof(data1[0]), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible));
    // Buffer::CopyBuffer(element_in_data->buffer, tempbuffer);

    Buffer::CopyBuffer(image_state.ranges_buffer, tempbuffer);

    // Buffer::CopyBuffer(binning_state.point_list_key_buffer, tempbuffer);

    // Buffer::CopyBuffer(point_list_value->buffer, tempbuffer);

    auto temp1 = tempbuffer->Get_mapped_data(0);
    std::memcpy(data1.data(), temp1.data(), data1.size() * sizeof(data1[0]));
    // int r = geometry_state.tiles_touched_d[168385];
    int rr = 0;
    for (int i = 0; i < data1.size(); i++) {
        if (data1[i] != 12345) {
            // std::cout << 12344 << std::endl;
        }
    }
    // 954565595133
    std::cout << "here" << std::endl;
}

GaussianManager::GeometryState::GeometryState(int size)
{
    depth_d.resize(size);
    clamped_d.resize(size * 3);
    radii_d.resize(size);
    mean2d_d.resize(size);
    cov3d_d.resize(size * 6);
    conic_opacity_d.resize(size);
    rgb_d.resize(size * 3);
    tiles_touched_d.resize(size);
    point_offsets_d.resize(size);

    // depth = UniformManager::make_uniform(depth_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);

    // clamped = UniformManager::make_uniform(clamped_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // radii = UniformManager::make_uniform(radii_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // mean2d = UniformManager::make_uniform(mean2d_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // conv3d = UniformManager::make_uniform(cov3d_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // conic_opacity = UniformManager::make_uniform(conic_opacity_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // rgb = UniformManager::make_uniform(rgb_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // tiles_touched = UniformManager::make_uniform(tiles_touched_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    // point_offsets = UniformManager::make_uniform(point_offsets_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
    auto flag = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddressKHR;
    depth_buffer = Buffer::CreateDeviceBuffer(depth_d.data(),
                                              depth_d.size() * sizeof(depth_d[0]),
                                              flag);
    clamped_buffer = Buffer::CreateDeviceBuffer(clamped_d.data(),
                                                clamped_d.size() * sizeof(clamped_d[0]),
                                                flag);
    radii_buffer = Buffer::CreateDeviceBuffer(radii_d.data(),
                                              radii_d.size() * sizeof(radii_d[0]),
                                              flag);
    mean2d_buffer = Buffer::CreateDeviceBuffer(mean2d_d.data(),
                                               mean2d_d.size() * sizeof(mean2d_d[0]),
                                               flag);
    conv3d_buffer = Buffer::CreateDeviceBuffer(cov3d_d.data(),
                                               cov3d_d.size() * sizeof(cov3d_d[0]),
                                               flag);
    conic_opacity_buffer = Buffer::CreateDeviceBuffer(conic_opacity_d.data(),
                                                      conic_opacity_d.size() * sizeof(conic_opacity_d[0]),
                                                      flag);
    rgb_buffer = Buffer::CreateDeviceBuffer(rgb_d.data(),
                                            rgb_d.size() * sizeof(rgb_d[0]),
                                            flag);
    tiles_touched_buffer = Buffer::CreateDeviceBuffer(tiles_touched_d.data(),
                                                      tiles_touched_d.size() * sizeof(tiles_touched_d[0]),
                                                      flag);
    point_offsets_buffer = Buffer::CreateDeviceBuffer(point_offsets_d.data(),
                                                      point_offsets_d.size() * sizeof(point_offsets_d[0]),
                                                      flag);
}

GaussianManager::BinningState::BinningState(int size)
{
    point_list_d.resize(size);
    point_list_key_d.resize(size);
    point_list_pingpong_d.resize(size);
    point_list_key_pingpong_d.resize(size);
    auto flag = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddressKHR | vk::BufferUsageFlagBits::eTransferSrc;
    point_list_buffer = Buffer::CreateDeviceBuffer(point_list_d.data(),
                                                   point_list_d.size() * sizeof(point_list_d[0]),
                                                   flag);
    point_list_key_buffer = Buffer::CreateDeviceBuffer(point_list_key_d.data(),
                                                       point_list_key_d.size() * sizeof(point_list_key_d[0]),
                                                       flag);
    point_list_pingpong_buffer = Buffer::CreateDeviceBuffer(point_list_pingpong_d.data(),
                                                            point_list_pingpong_d.size() * sizeof(point_list_pingpong_d[0]),
                                                            flag);
    point_list_key_pingpong_buffer = Buffer::CreateDeviceBuffer(point_list_key_pingpong_d.data(),
                                                                point_list_key_pingpong_d.size() * sizeof(point_list_key_pingpong_d[0]),
                                                                flag);
}

GaussianManager::ImageState::ImageState(int size)
{
    ranges_d.resize(size);
    n_contrib_d.resize(size);
    accum_alpha_d.resize(size);
    auto flag = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddressKHR | vk::BufferUsageFlagBits::eTransferSrc;
    ranges_buffer = Buffer::CreateDeviceBuffer(ranges_d.data(),
                                               ranges_d.size() * sizeof(ranges_d[0]),
                                               flag);
    n_contrib_buffer = Buffer::CreateDeviceBuffer(n_contrib_d.data(),
                                                  n_contrib_d.size() * sizeof(n_contrib_d[0]),
                                                  flag);
    accum_alpha_buffer = Buffer::CreateDeviceBuffer(accum_alpha_d.data(),
                                                    accum_alpha_d.size() * sizeof(accum_alpha_d[0]),
                                                    flag);
}

void GaussianManager::get_gaussian_raw_data()
{
    auto gs_data = MCGS::load_ply("/home/mocheng/project/MCGS/assets/point_cloud.ply");
    auto xyz_d = MCGS::get_xyz(gs_data);
    for (int i = 0; i < xyz_d.size() / 3; i++) {

        // glm::make_vec3({ xyz_d[i * 3], xyz_d[i * 3 + 1], xyz_d[i * 3 + 2] });
    }

    std::vector<glm::vec3> xyz_3 {
        xyz_d.begin(),
        xyz_d.end() - 3
    };
    auto scale_d = MCGS::get_scale(gs_data);
    auto dc_012 = MCGS::get_dc_012(gs_data);
    auto dc_rest = MCGS::get_dc_rest(gs_data);

    auto opacity_d = MCGS::get_opacity(gs_data);
    auto rotations_d = MCGS::get_rotation(gs_data);

    std::vector<float> feature_d(dc_012.size() + dc_rest.size());
    for (int i = 0; i < dc_012.size() / 3; i++) {
        for (int j = 0; j < 3; j++) {

            feature_d[i * 48 + j] = dc_012[i * 3 + j];
        }
        for (int j = 0; j < 45; j++) {
            feature_d[i * 48 + 3 + j] = dc_rest[i * 45 + j];
        }
    }
    point_num = opacity_d.size();

    // xyz = UniformManager::make_uniform(xyz_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eUniformBuffer);
    // scale = UniformManager::make_uniform(scale_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eUniformBuffer);
    // feature = UniformManager::make_uniform(feature_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eUniformBuffer);
    // opacity = UniformManager::make_uniform(opacity_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eUniformBuffer);
    // rotation = UniformManager::make_uniform(rotations_d, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eUniformBuffer);
    auto flag = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddressKHR;
    xyz_buffer = Buffer::CreateDeviceBuffer(xyz_d.data(),
                                            xyz_d.size() * sizeof(xyz_d[0]),
                                            flag);
    opacity_buffer = Buffer::CreateDeviceBuffer(opacity_d.data(),
                                                opacity_d.size() * sizeof(opacity_d[0]),
                                                flag);
    scale_buffer = Buffer::CreateDeviceBuffer(scale_d.data(),
                                              scale_d.size() * sizeof(scale_d[0]),
                                              flag);
    rotation_buffer = Buffer::CreateDeviceBuffer(rotations_d.data(),
                                                 rotations_d.size() * sizeof(rotations_d[0]),
                                                 flag);
    feature_buffer = Buffer::CreateDeviceBuffer(feature_d.data(),
                                                feature_d.size() * sizeof(feature_d[0]),
                                                flag);

    geometry_state = GeometryState(xyz_d.size());
    // binning_state = BinningState(xyz_d.size() * 10);

    binning_state = BinningState(1625771);
    image_state = ImageState(800 * 800);
    auto addr = GS_Address {
        .xyz_address = xyz_buffer->get_address(),
        .scale_address = scale_buffer->get_address(),
        .feature_address = feature_buffer->get_address(),
        .opacity_address = xyz_buffer->get_address(),
        .rotation_address = rotation_buffer->get_address(),
        .depth_address = geometry_state.depth_buffer->get_address(),
        .clamped_address = geometry_state.clamped_buffer->get_address(),
        .radii_address = geometry_state.clamped_buffer->get_address(),
        .mean2d_address = geometry_state.mean2d_buffer->get_address(),
        .conv3d_address = geometry_state.conv3d_buffer->get_address(),
        .conic_opacity_address = geometry_state.conic_opacity_buffer->get_address(),
        .rgb_address = geometry_state.rgb_buffer->get_address(),
        .tiles_touched_address = geometry_state.tiles_touched_buffer->get_address(),
        .point_offsets_address = geometry_state.point_offsets_buffer->get_address(),
        .point_list_keys_address = binning_state.point_list_key_buffer->get_address(),
        .point_list_keys_pingpong_address = binning_state.point_list_key_pingpong_buffer->get_address(),
        .point_list_address = binning_state.point_list_buffer->get_address(),
        .point_list_pingpong_address = binning_state.point_list_pingpong_buffer->get_address(),
        // Image
        .ranges_address = image_state.ranges_buffer->get_address(),
        .n_contrib_address = image_state.n_contrib_buffer->get_address(),
        .accum_alpha_address = image_state.accum_alpha_buffer->get_address()
    };
    address = UniformManager::make_uniform({ addr }, vk::ShaderStageFlagBits::eCompute, vk::DescriptorType::eStorageBuffer);
}
}