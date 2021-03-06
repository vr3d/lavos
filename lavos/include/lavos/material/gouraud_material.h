
#ifndef LAVOS_GOURAUD_MATERIAL_H
#define LAVOS_GOURAUD_MATERIAL_H

#include "material.h"

namespace lavos
{

class GouraudMaterial: public Material
{
	private:
		struct alignas(sizeof(float)) UniformBuffer
		{
			glm::vec4 color_factor;
			float specular_exponent;
		};

		struct InstanceData
		{
			lavos::Buffer uniform_buffer;
		};

		void CreateDescriptorSetLayout();

		vk::ShaderModule vert_shader_module;
		vk::ShaderModule frag_shader_module;

		Texture texture_default_base_color;
		Texture texture_default_normal;

	public:
		static const MaterialInstance::ParameterSlot parameter_slot_specular_exponent = 1000;

		GouraudMaterial(Engine *engine);
		~GouraudMaterial();

		virtual std::vector<vk::DescriptorPoolSize> GetDescriptorPoolSizes() const override;
		virtual std::vector<vk::PipelineShaderStageCreateInfo> GetShaderStageCreateInfos() const override;

		virtual void WriteDescriptorSet(vk::DescriptorSet descriptor_set, MaterialInstance *instance) override;

		virtual void *CreateInstanceData() override;
		virtual void DestroyInstanceData(void *data) override;
		virtual void UpdateInstanceData(void *data, MaterialInstance *instance) override;
};

}


#endif //LAVOS_GOURAUD_MATERIAL_H
