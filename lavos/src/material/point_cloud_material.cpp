
#include "lavos/engine.h"
#include "lavos/material/point_cloud_material.h"
#include "lavos/material/material_instance.h"

#include "lavos/shader_load.h"

using namespace lavos;

PointCloudMaterial::PointCloudMaterial(lavos::Engine *engine) : Material(engine)
{
	CreateDescriptorSetLayout();

	vert_shader_module = CreateShaderModule(engine->GetVkDevice(), "material/point_cloud.vert");
	frag_shader_module = CreateShaderModule(engine->GetVkDevice(), "material/point_cloud.frag");
}

PointCloudMaterial::~PointCloudMaterial()
{
	auto &device = engine->GetVkDevice();

	device.destroyShaderModule(vert_shader_module);
	device.destroyShaderModule(frag_shader_module);
}

void PointCloudMaterial::CreateDescriptorSetLayout()
{
	descriptor_set_layout = nullptr;
}

std::vector<vk::DescriptorPoolSize> PointCloudMaterial::GetDescriptorPoolSizes() const
{
	return {};
}

std::vector<vk::PipelineShaderStageCreateInfo> PointCloudMaterial::GetShaderStageCreateInfos() const
{
	return {
			vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
											  vk::ShaderStageFlagBits::eVertex,
											  vert_shader_module,
											  "main"),
			vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
											  vk::ShaderStageFlagBits::eFragment,
											  frag_shader_module,
											  "main")
	};
}


void PointCloudMaterial::WriteDescriptorSet(vk::DescriptorSet descriptor_set, MaterialInstance *instance)
{
	return;
}

void *PointCloudMaterial::CreateInstanceData()
{
	return nullptr;
}

void PointCloudMaterial::DestroyInstanceData(void *data_p)
{
}

void PointCloudMaterial::UpdateInstanceData(void *data_p, MaterialInstance *instance)
{
}