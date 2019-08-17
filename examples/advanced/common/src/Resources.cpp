#include <blue/Assertions.h>
#include <blue/ShaderUtils.h>
#include <blue/ModelLoader.h>
#include <blue/Context.hpp>

#include "Resources.hpp"

Resources* Resources::_instance = nullptr;

Resources& Resources::instance()
{
	BLUE_ASSERT(_instance);
	return *_instance;
}

void Resources::init()
{
	BLUE_ASSERT(!_instance);
	_instance = new Resources();
}

void Resources::dispose()
{
	BLUE_ASSERT(_instance);
	delete _instance;
	_instance = nullptr;
}

void Resources::load_shaders()
{
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/SimpleDepth.vertex.glsl", "resources/SimpleDepth.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.simple_depth_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/SimpleDepth_Instanced.vertex.glsl", "resources/SimpleDepth.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.simple_depth_instanced_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Decoration.vertex.glsl", "resources/Decoration.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.decoration_map_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Model.vertex.glsl", "resources/Model.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.model_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Model_Instanced.vertex.glsl", "resources/Model_Instanced.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.model_instanced_shader = shader_future.get();
	}
	{
		auto compile_shader_entity = ShaderUtils::make_entity("resources/Swinging_Instanced.vertex.glsl", "resources/Swinging_Instanced.fragment.glsl");
		auto shader_future = blue::Context::gpu_system().submit(compile_shader_entity);
		shader_future.wait();
		shaders.swinging_shader = shader_future.get();
	}
}

void Resources::load_models()
{
	Attributes attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
	};

	Attributes swinging_attributes =
	{
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::VERTEX_POSITION, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::NORMAL, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::VEC3, ShaderAttribute::Purpose::COLOR, ShaderAttribute::Buffer::VERTEX},
		{ ShaderAttribute::Type::FLOAT, ShaderAttribute::Purpose::NORMALIZED_HEIGHT, ShaderAttribute::Buffer::VERTEX},
	};

	{
		auto scene_ptr = models::load_scene("resources/PineTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, swinging_attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, swinging_attributes, vertex_counter });
		vertex_array_future.wait();
		models.pine_tree = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Hurdle.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.hurdle = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Wheat.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.wheat = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Boulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.boulder = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/SmallBoulder.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.small_boulder = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Grass.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.grass = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Pylon.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.pylon = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Bush.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.bush = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/CutTree.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.cut_tree = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Track.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.track = vertex_array_future.get();
	}
	{
		auto scene_ptr = models::load_scene("resources/Bridge.fbx");
		unsigned int vertex_counter = 0;
		auto vertices = models::parse_scene(scene_ptr, attributes, vertex_counter);

		auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ vertices, {}, attributes, vertex_counter });
		vertex_array_future.wait();
		models.bridge = vertex_array_future.get();
	}
}

void Resources::load_textures()
{
	// No textures so far.
}

void Resources::load_environment()
{
	// Create framebuffer with only depth component for shadows:
	light_environment.depth = blue::Context::gpu_system().submit(CreateFramebufferEntity{ true, 1024, 1024 }).get();

	// Create map_environment
	map_environment.environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();
	light_environment.environment = blue::Context::gpu_system().submit(CreateEnvironmentEntity{}).get();

	// Upload map environment camera matrices
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ map_environment.environment, map_environment.camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });

	// Set pale blue clear color
	blue::Context::gpu_system().submit(SetClearColorEntity{ {0.25f, 0.45f, 0.8f} });

	// Move camera to initial position
	map_environment.camera.set_pos({ 7.78816, 28.7423, 22.3805 });
	map_environment.camera.add_rotation(- 68.5f, - 44.25f);

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ map_environment.environment, map_environment.camera.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_CameraPos{ map_environment.environment, map_environment.camera.get_position() });

	// Move light-source camera to initial position
	light_environment.camera.set_far(500.0f);
	light_environment.camera.set_near(-100.0f);
	light_environment.camera.set_pos({ 64.0f, 20.4532f, 64.0f});
	// FIXME: Add functionality to camera to look at specific point.
	// light_environment.camera.look_at({64.0f, 0.0f, 64.0f});

	// Upload light-space matrices to both environments:
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_Projection{ light_environment.environment, light_environment.camera.get_projection() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_View{ light_environment.environment, light_environment.camera.get_view() });

	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightSpaceMatrix{ map_environment.environment, light_environment.camera.get_projection() * light_environment.camera.get_view() });
	blue::Context::gpu_system().submit(UpdateEnvironmentEntity_LightPos{ map_environment.environment, light_environment.camera.get_position() });
}
