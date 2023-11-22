#pragma once
#define GLFW_INCLUDE_VULKAN

#include "WarpBase.hpp"
#include "WarpMath.hpp"
#include <GLFW/glfw3.h>
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

#include "WarpEarCut.hpp"
#include "clipper2/clipper.h"
#include "clipper2/clipper.minkowski.h"
#include "Image/image.h"
#include <gainput/gainput.h>

namespace Warp {

template <> struct nth<0, Clipper2Lib::Point64> {
      inline static auto get(const Clipper2Lib::Point64& t) {
            return t.x;
      };
};
template <> struct nth<1, Clipper2Lib::Point64> {
      inline static auto get(const Clipper2Lib::Point64& t) {
            return t.y;
      };
};

template <> struct nth<0, Clipper2Lib::PointD> {
      inline static auto get(const Clipper2Lib::PointD& t) {
            return t.x;
      };
};
template <> struct nth<1, Clipper2Lib::PointD> {
      inline static auto get(const Clipper2Lib::PointD& t) {
            return t.y;
      };
};

struct Vertex2 {
      glm::vec3                              pos;
      glm::vec3                              color;
      glm::vec2                              texCoord;

      static VkVertexInputBindingDescription get_vertex_bind_desc() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding   = 0;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescription.stride    = sizeof(Vertex2);
            return std::move(bindingDescription);
      }
      static vector<VkVertexInputAttributeDescription> get_vertex_input_attr_desc() {
            vector<VkVertexInputAttributeDescription> attributeDescription(3);
            attributeDescription[0].binding  = 0;
            attributeDescription[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescription[0].location = 0;
            attributeDescription[0].offset   = offsetof(Vertex2, pos);

            attributeDescription[1].binding  = 0;
            attributeDescription[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescription[1].location = 1;
            attributeDescription[1].offset   = offsetof(Vertex2, color);

            attributeDescription[2].binding  = 0;
            attributeDescription[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescription[2].location = 2;
            attributeDescription[2].offset   = offsetof(Vertex2, texCoord);
            return std::move(attributeDescription);
      }
};

struct UniformBufferObject {
      alignas(16) glm::mat4 model;
      alignas(16) glm::mat4 view;
      alignas(16) glm::mat4 proj;
};

class Render {
      vector<Vertex2> vertices = {};
      vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

      struct QueueFamilyIndices {
            optional<uint32_t> graphicsFamily;
            optional<uint32_t> presentFamily;
            // 是否都有值
            bool isComplete() {
                  return graphicsFamily.has_value() && presentFamily.has_value();
            }
      };

      // 检查Surface的功能是否健全和支持性的反馈结构体
      struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR   capabilities; // 检查基本功能
            vector<VkSurfaceFormatKHR> formats;      // 检查Surface格式
            vector<VkPresentModeKHR>   presentModes; // 检查可用的显示模式
      };

      enum Button {
            BT_A,
            BT_D,
      };

      gainput::InputManager   manager;
      const gainput::DeviceId keyboardId;
      gainput::InputMap       map;

public:
      Render();
      ~Render();

      void run();

private:
      optional<bool> init_window();

      optional<bool> create_instance();
      optional<bool> init_vulkan_debug_message();
      optional<bool> create_surface();
      optional<bool> select_physical_device();
      optional<bool> create_logical_device_and_queue();
      optional<bool> create_swap_chain();
      optional<bool> create_swap_chain_image_view();
      optional<bool> create_render_pass();
      optional<bool> create_uniform_buffer();
      optional<bool> create_descriptor_set_layout();
      optional<bool> create_descriptor_pool();
      optional<bool> create_descriptor_sets();
      optional<bool> create_graphics_pipeline();
      optional<bool> create_framebuffers();
      optional<bool> create_command_pool();

      optional<bool> create_test_model();

      optional<bool> create_vertex_buffer();
      optional<bool> create_index_buffer();
      optional<bool> create_command_buffer();
      optional<bool> record_command(VkCommandBuffer, uint32_t imageIndex);
      optional<bool> create_semaphores();

      void           update_uniform_buffer(int32_t imageIndex);

      void           draw_window_frame();
      void           clean_swap_chain();
      void           recreate_swap_chain();

      void           create_test_image();
      void           create_depth_image();

      // helpers
      tuple<VkBuffer, VkDeviceMemory> create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
      void                            start_single_time_commands(std::function<void(VkCommandBuffer)> func);
      void                            translation_image_layout(VkImage img, VkFormat format, VkImageLayout old, VkImageLayout to);
      optional<VkImageView>           create_image_view(VkImage img, VkFormat format);
      void                            copy_buffer_2_image(VkBuffer buf, VkImage img, glm::ivec2 size);
      void                            copy_image_2_buffer(VkImage img, VkBuffer buf, glm::ivec2 size);
      uint32_t                        find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
      void                            copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
      optional<VkShaderModule>        create_shader_module(const string& moduleName, const vector<uint32_t>& code);
      optional<vector<uint32_t>>      compile_glsl_spriv(const string& shaderName, const string& shaderSource, glslang_stage_t stage);
      expected<std::tuple<VkImage, VkDeviceMemory>, WERROR> create_image(
          VkDevice device, glm::ivec2 size, VkFormat format, VkImageTiling tilingType, VkImageUsageFlags usage, VkMemoryPropertyFlags properties
      );

      // main funcs
      void main_loop();
      void end();

      // other
      void fill_vulkan_debug_message_create_info(VkDebugUtilsMessengerCreateInfoEXT& createinfo);

      // Configs
      SwapChainSupportDetails                         get_swap_chain_detail(VkPhysicalDevice device, const VkSurfaceKHR& surface);
      VkExtent2D                                      choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

      expected<vector<VkExtensionProperties>, WERROR> check_extensions();
      expected<vector<VkLayerProperties>, WERROR>     check_validation_layers();

      QueueFamilyIndices                              find_queue_families(VkPhysicalDevice device);

private:
      static VKAPI_ATTR VkBool32 vk_debug_callback(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
      );
      static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
      GLFWwindow*              window{};

      const int                MAX_FRAMES_IN_FLIGHTS = 2;
      int                      currentFrame          = 0;

      vector<const char*>      enableInstanceExtensions{ "VK_KHR_surface", "VK_KHR_win32_surface" };
      vector<const char*>      enableLayers{};

      vector<const char*>      enableDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

      VkInstance               vkInstance{};
      VkPhysicalDevice         vkPhysicalDevice{};
      VkDevice                 vkDevice{};
      VkQueue                  vkGraphicsQueue{};
      VkQueue                  vkPresentQueue{};

      VkSurfaceKHR             vkSurface{};
      VkSwapchainKHR           vkSwapChain{};

      VkDebugUtilsMessengerEXT vkDebugMessenger{};

      vector<VkImage>          vkSwapChainImages{};
      vector<VkImageView>      vkSwapChainImageViews{};
      vector<VkFramebuffer>    vkSwapChainFramebuffers{};

      uint32_t                 windowWidth{ 4 * 400 };
      uint32_t                 windowHeight{ 3 * 400 };

      VkPipelineLayout         vkPipelineLayout{};
      VkRenderPass             vkRenderPass{};
      VkPipeline               vkGraphicsPipeline{};
      VkCommandPool            vkCommandPool{};
      vector<VkCommandBuffer>  vkCommandBuffer{};

      vector<VkSemaphore>      vkImageAvailableSemaphore{};
      vector<VkSemaphore>      vkRenderFinishedSemaphore{};
      vector<VkFence>          inFlightFences{};

      VkExtent2D               swapChainCurrentExtent{};
      VkBuffer                 vkVertexBuffer{};
      VkDeviceMemory           vkVertexBufferMemory{};
      VkBuffer                 vkIndexBuffer{};
      VkDeviceMemory           vkIndexBufferMemory{};

      VkDescriptorSetLayout    vkDescLayout{};

      vector<VkDeviceMemory>   uniformMem{};
      vector<VkBuffer>         uniformBuffer{};
      vector<void*>            uniformMap{};

      VkDescriptorPool         vkDescPool{};
      vector<VkDescriptorSet>  vkDescSets{};

      VkImage                  testImage;
      VkDeviceMemory           testImageMem;

      VkImageView              testImageView{};
      VkSampler                testSampler{};

      VkImage                  depthImage;
      VkDeviceMemory           depthImageMem;
      VkImageView              depthImageView;

      // formats
      VkFormat vkSwapChainImageFormat{};

      bool     enableValidationLayers{ true };
      bool     framebufferResized{ false };
};

// createvk
VkResult create_debug_utils_message_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
);

void destroy_debug_utils_message_ext(VkInstance instance, VkDebugUtilsMessengerEXT* pDebugMessenger, const VkAllocationCallbacks* pAllocator);
}; // namespace Warp
