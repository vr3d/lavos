
#include "component/directional_light_component.h"

using namespace engine;

DirectionalLightComponent::DirectionalLightComponent(glm::vec3 intensity)
	: intensity(intensity)
{
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}