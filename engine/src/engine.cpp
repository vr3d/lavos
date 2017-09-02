
#include <iostream>
#include "engine.h"

using namespace engine;

static const std::vector<const char *> validation_layers = {
		"VK_LAYER_LUNARG_standard_validation"

		/*"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_LUNARG_image",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_GOOGLE_unique_objects"*/
};




VkResult vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
										const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
		func(instance, callback, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
													uint64_t obj, size_t location, int32_t code,
													const char *layerPrefix, const char *msg, void *userData)
{
	std::cerr << "vulkan: " << msg << std::endl;
	return VK_FALSE;
}




Engine::Engine(const CreateInfo &info)
	: info(info)
{
	CreateInstance();
	SetupDebugCallback();
}

Engine::~Engine()
{
	if(debug_report_callback)
		instance.destroyDebugReportCallbackEXT(debug_report_callback);

	instance.destroy(nullptr);
}


std::vector<const char *> Engine::GetRequiredInstanceExtensions()
{
	std::vector<const char *> extensions;

	for(const auto &extension : info.required_instance_extensions)
		extensions.push_back(extension.c_str());

	if(info.enable_validation_layers)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	return extensions;
}

std::vector<const char *> Engine::GetRequiredDeviceExtensions()
{
	std::vector<const char *> extensions;

	for(const auto &extension : info.required_device_extensions)
		extensions.push_back(extension.c_str());

	return extensions;
}

bool Engine::CheckValidationLayerSupport()
{
	std::cout << "Available Layers:" << std::endl;

	auto layers_available = vk::enumerateInstanceLayerProperties();

	for(const auto &layer_props : layers_available)
	{
		std::cout << "\t" << layer_props.layerName << std::endl;
	}

	for(const char *layer_name : validation_layers)
	{
		bool layer_found = false;

		for(const auto &layer_props : layers_available)
		{
			if(strcmp(layer_name, layer_props.layerName) == 0)
			{
				layer_found = true;
				break;
			}
		}

		if(!layer_found)
			return false;
	}

	return true;
}

void Engine::CreateInstance()
{
	vk::ApplicationInfo app_info(info.app_info.c_str(), VK_MAKE_VERSION(1, 0, 0),
								 "no engine", VK_MAKE_VERSION(1, 0, 0),
								 VK_API_VERSION_1_0);

	vk::InstanceCreateInfo create_info;
	create_info.setPApplicationInfo(&app_info);


	// extensions

	auto extensions_available = vk::enumerateInstanceExtensionProperties();
	std::cout << "Available Extensions:" << std::endl;
	for(const auto &extension : extensions_available)
		std::cout << "\t" << extension.extensionName << std::endl;

	auto required_extensions = GetRequiredInstanceExtensions();
	create_info.setEnabledExtensionCount(static_cast<uint32_t>(required_extensions.size()));
	create_info.setPpEnabledExtensionNames(required_extensions.data());


	// layers

	if(info.enable_validation_layers)
	{
		if(!CheckValidationLayerSupport())
			throw std::runtime_error("validation layers requested, but not available!");

		create_info.setEnabledLayerCount(static_cast<uint32_t>(validation_layers.size()));
		create_info.setPpEnabledLayerNames(validation_layers.data());
	}
	else
	{
		create_info.setEnabledLayerCount(0);
	}


	instance = vk::createInstance(create_info);
}



void Engine::SetupDebugCallback()
{
	if(!info.enable_validation_layers)
		return;

	vk::DebugReportCallbackCreateInfoEXT create_info(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
													 DebugCallback, this);
	debug_report_callback = instance.createDebugReportCallbackEXT(create_info);
}



bool Engine::CheckDeviceExtensionSupport(vk::PhysicalDevice physical_device)
{
	auto available_extensions = physical_device.enumerateDeviceExtensionProperties();

	std::vector<const char *> device_extensions = GetRequiredDeviceExtensions();
	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	for(const auto &extension : available_extensions)
		required_extensions.erase(extension.extensionName);

	return required_extensions.empty();
}


void Engine::InitializeForSurface(vk::SurfaceKHR surface)
{
	PickPhysicalDevice(surface);
	CreateLogicalDevice(surface);
}

bool Engine::IsPhysicalDeviceSuitable(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface)
{
	auto props = physical_device.getProperties();
	auto features = physical_device.getFeatures();


	// extensions

	if(!CheckDeviceExtensionSupport(physical_device))
		return false;


	// surface

	auto surface_formats = physical_device.getSurfaceFormatsKHR(surface);
	auto surface_present_modes = physical_device.getSurfacePresentModesKHR(surface);

	if(surface_formats.empty() || surface_present_modes.empty())
		return false;

	return true;
}

void Engine::PickPhysicalDevice(vk::SurfaceKHR surface)
{
	auto physical_devices = instance.enumeratePhysicalDevices();
	if(physical_devices.empty())
		throw std::runtime_error("failed to find GPUs with Vulkan support!");


	std::cout << "Available Physical Devices:" << std::endl;
	for(const auto &physical_device : physical_devices)
	{
		auto props = physical_device.getProperties();
		std::cout << "\t" << props.deviceName << std::endl;
	}

	for(const auto &physical_device : physical_devices)
	{
		if(IsPhysicalDeviceSuitable(physical_device, surface))
		{
			this->physical_device = physical_device;
			break;
		}
	}

	if(!physical_device)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

Engine::QueueFamilyIndices Engine::FindQueueFamilies(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface)
{
	QueueFamilyIndices indices;

	auto queue_families = physical_device.getQueueFamilyProperties();
	int i = 0;
	for(const auto &queue_family : queue_families)
	{
		if(queue_family.queueCount > 0 && queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.graphics_family = i;

		if(queue_family.queueCount > 0 && physical_device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
			indices.present_family = i;

		if(indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

void Engine::CreateLogicalDevice(vk::SurfaceKHR surface)
{
	queue_family_indices = FindQueueFamilies(physical_device, surface);

	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	std::set<int> unique_queue_families = { queue_family_indices.graphics_family, queue_family_indices.present_family };

	for(int queue_family : unique_queue_families)
	{
		float queue_priority = 1.0f;

		auto queue_info = vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(static_cast<uint32_t>(queue_family))
				.setQueueCount(1)
				.setPQueuePriorities(&queue_priority);

		queue_create_infos.push_back(queue_info);
	}

	vk::PhysicalDeviceFeatures features;


	std::vector<const char *> device_extensions = GetRequiredDeviceExtensions();

	auto create_info = vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(static_cast<uint32_t>(queue_create_infos.size()))
			.setPQueueCreateInfos(queue_create_infos.data())
			.setPEnabledFeatures(&features)
			.setEnabledExtensionCount(static_cast<uint32_t>(device_extensions.size()))
			.setPpEnabledExtensionNames(device_extensions.data());

	if(info.enable_validation_layers)
	{
		create_info
				.setEnabledLayerCount(static_cast<uint32_t>(validation_layers.size()))
				.setPpEnabledLayerNames(validation_layers.data());
	}
	else
	{
		create_info.setEnabledLayerCount(0);
	}

	device = physical_device.createDevice(create_info);

	graphics_queue = device.getQueue(static_cast<uint32_t>(queue_family_indices.graphics_family), 0);
	present_queue = device.getQueue(static_cast<uint32_t>(queue_family_indices.present_family), 0);
}