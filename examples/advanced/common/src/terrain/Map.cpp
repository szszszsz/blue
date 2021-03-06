#include "terrain/Map.hpp"
#include "blue/Context.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include "blue/ShaderUtils.h"
#include "Resources.hpp"

#include <fstream>
#include <istream>
#include "blue/ResourcesPath.h"

#include <glm/gtx/normal.hpp>
#include <Resources.hpp>
#include "glm/geometric.hpp"


void Map::shuffle_color_points(float x, float y, float R)
{
	// TODO: Implement
}

void Map::import_from_file(const std::string& filename)
{
	auto path_extended = paths::getResourcesPath() + filename;

	std::vector<char> data;

	SDL_RWops* file = SDL_RWFromFile(path_extended.c_str(), "rb");
	if (file != NULL) {
		auto size = file->size(file);
		//Initialize data
		data.resize(size);
		auto read = file->read(file, &data[0], size, 1);
		//Close file handler
		SDL_RWclose(file);
	}
	else
	{
		blue::Context::logger().error("Error: Unable to open map file. SDL Error: {}", SDL_GetError());
		return;
	}

	std::size_t offset = 0;
	std::size_t number_of_non_clickable = 0;
	std::size_t number_of_clickable = 0;

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			tiles[x][y].clickable = data.data()[offset];
			offset += sizeof(bool);

			if (!tiles[x][y].clickable)
			{
				number_of_non_clickable++;
			}
			else
			{
				number_of_clickable++;
			}
		}
	}

	std::size_t number_of_vertices_non_clickable = number_of_non_clickable * Tile::get_vertices_translated(0, 0, { 0, 0,0 }, { 0, 0, 0 }).size();
	decoration_vertices.resize(number_of_vertices_non_clickable);

	std::size_t number_of_vertices_clickable = number_of_clickable * Tile::get_vertices_translated(0, 0, { 0, 0,0 }, { 0, 0, 0 }).size();
	clickable_vertices.resize(number_of_vertices_clickable);

	for (std::size_t index = 0; index < decoration_vertices.size(); index++)
	{
		const auto float_ptr = reinterpret_cast<float*>(&data[offset]);
		decoration_vertices[index] = *float_ptr;
		offset += sizeof(float);
	}

	for (std::size_t index = 0; index < clickable_vertices.size(); index++)
	{
		const auto float_ptr = reinterpret_cast<float*>(&data[offset]);
		clickable_vertices[index] = *float_ptr;
		offset += sizeof(float);
	}

	for (std::size_t index = 0; index < number_of_non_clickable; index++)
	{
		for (const auto& i : Tile::get_indices(index))
		{
			decoration_indices.push_back(i);
		}
	}

	for (std::size_t index = 0; index < number_of_clickable; index++)
	{
		for (const auto& i : Tile::get_indices(index))
		{
			clickable_indices.push_back(i);
		}
	}

	decoration_tiles = number_of_non_clickable;
	clickable_tiles = number_of_clickable;

	blue::Context::logger().info("Found decoration vertices: {} clickable vertices {}", number_of_vertices_non_clickable, number_of_vertices_clickable);
}

void Map::set_tile_occupant(UnitType occupant, std::uint16_t tile_x, std::uint16_t tile_y)
{
	tiles[tile_x][tile_y].occupant = occupant;
}

UnitType Map::get_tile_occupant(std::uint16_t tile_x, std::uint16_t tile_y)
{
	return tiles[tile_x][tile_y].occupant;
}

void Map::set_tile_occupant(UnitType occupant, std::uint16_t tile_x, std::uint16_t tile_y)
{
	tiles[tile_x][tile_y].occupant = occupant;
}

UnitType Map::get_tile_occupant(std::uint16_t tile_x, std::uint16_t tile_y)
{
	return tiles[tile_x][tile_y].occupant;
}

void Map::export_to_file(const std::string& filename)
{
	std::fstream out(paths::getResourcesPath() + filename, std::fstream::binary | std::fstream::out);

	if (!out.is_open())
	{
		blue::Context::logger().error("Failed to open file: {} for saving map", filename);
		return;
	}

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			out.write(reinterpret_cast<char*>(&tiles[x][y].clickable), sizeof(bool));
		}
	}

	for (std::size_t index = 0; index < decoration_vertices.size(); index++)
	{
		out.write(reinterpret_cast<char*>(&decoration_vertices[index]), sizeof(float));
	}

	for (std::size_t index = 0; index < clickable_vertices.size(); index++)
	{
		out.write(reinterpret_cast<char*>(&clickable_vertices[index]), sizeof(float));
	}

	out.close();
}


void Map::upload_clickable_vertices()
{
	// randomize color a bit
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;

	clickable_vertices.clear();
	clickable_indices.clear();

	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (!tiles[x][y].clickable)
			{
				continue;
			}

			glm::vec3 clickable_color_1 = { 0.2f, 0.7f, 0.2f };
			glm::vec3 clickable_color_2 = { 0.2f, 0.7f, 0.2f };

			int random_variable = 10 + std::rand() / ((RAND_MAX + 1u) / 16);  // Note: 1+rand()%6 is biased
			float color_delta = 0.75f / random_variable;
			if (random_variable % 2)
			{
				clickable_color_1.y += color_delta;
				clickable_color_2.y -= color_delta;
			}
			else
			{
				clickable_color_1.y -= color_delta;
				clickable_color_2.y += color_delta;
			}

			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y), clickable_color_1, clickable_color_2))
			{
				clickable_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(tile_index))
			{
				clickable_indices.push_back(index);
			}
			tile_index++;
		}
	}

	blue::Context::logger().info("Clickable vertices: {} Clickable indices: {}", clickable_vertices.size(), clickable_indices.size());
	if (clickable_vertices.empty())
	{
		return;
	}

	auto attributes = Tile::get_attributes();

	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, tile_index * 6 });
	VertexArray clickable_vertices_vertex_array = vertex_array_future.get();

	clickable_tiles = tile_index;

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.clickable_map;
	entity.vertex_array = clickable_vertices_vertex_array;
	entity.scale = { 1.0f,1.0f,1.0f, };
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Resources::instance().map_environment.environment;

	clickable_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::upload_decoration_vertices()
{
	decoration_vertices.clear();
	decoration_indices.clear();

	std::unique_lock<std::mutex> lock(tiles_access);
	std::uint32_t tile_index = 0;


	for (std::size_t x = 0; x < CHUNK_DIMENSION; x++)
	{
		for (std::size_t y = 0; y < CHUNK_DIMENSION; y++)
		{
			if (tiles[x][y].clickable)
			{
				continue;
			}

			glm::vec3 clickable_color_1 = { 0.2f, 0.7f, 0.2f };
			glm::vec3 clickable_color_2 = { 0.2f, 0.7f, 0.2f };

			clickable_color_1 *= 0.83f;
			clickable_color_2 *= 0.83f;

			int random_variable = 10 + std::rand() / ((RAND_MAX + 1u) / 16);  // Note: 1+rand()%6 is biased
			float color_delta = 0.75f / random_variable;
			if (random_variable % 2)
			{
				clickable_color_1.y +=  color_delta;
				clickable_color_2.y -= color_delta;
			}
			else
			{
				clickable_color_1.y -= color_delta;
				clickable_color_2.y += color_delta;
			}

			for (const auto& vertex : Tile::get_vertices_translated(static_cast<float>(x), static_cast<float>(y), clickable_color_1, clickable_color_2))
			{
				decoration_vertices.push_back(vertex);
			}
			for (const auto& index : Tile::get_indices(tile_index))
			{
				decoration_indices.push_back(index);
			}
			tile_index++;
		}
	}

	auto attributes = Tile::get_attributes();
	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ decoration_vertices, decoration_indices, attributes, tile_index * 6 });
	vertex_array_future.wait();
	VertexArray decoration_vertices_vertex_array = vertex_array_future.get();

	decoration_tiles = tile_index;

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.decoration_map;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = {1.0f,1.0f,1.0f};
	entity.rotation = glm::identity<glm::quat>();
	entity.environment = Resources::instance().map_environment.environment;

	decoration_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::toggle_tile(std::uint16_t x, std::uint16_t y)
{
	std::unique_lock<std::mutex> lock(tiles_access);
	tiles[x][y].clickable = !tiles[x][y].clickable;
}

void Map::dispose_current_map_on_gpu()
{
	blue::Context::renderer().remove_render_entity(clickable_vertices_render_entity);
	blue::Context::renderer().remove_render_entity(decoration_vertices_render_entity);
	clickable_vertices_render_entity = 0;
	decoration_vertices_render_entity = 0;
}

void Map::dispose_current_decoration_on_gpus()
{
	blue::Context::renderer().remove_render_entity(decoration_vertices_render_entity);
	decoration_vertices_render_entity = 0;
}

void Map::upload_decoration()
{
	auto attributes = Tile::get_attributes();
	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ decoration_vertices, decoration_indices, attributes, decoration_tiles * 6 });
	vertex_array_future.wait();
	VertexArray decoration_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.decoration_map;
	entity.vertex_array = decoration_vertices_vertex_array;
	entity.scale = {1.0f,1.0f,1.0f};
	entity.rotation = glm::identity<glm::quat>();
//    entity.environment = Resources::instance().light_environment.environment;
	entity.environment = Resources::instance().map_environment.environment;
	entity.textures[0] = Resources::instance().light_environment.depth.texture;
	entity.framebuffer.framebuffer = 0;

	RenderEntity shadow;
	shadow.position = { 0, 0, 0 };
	shadow.shader = Resources::instance().shaders.simple_depth;
	shadow.vertex_array = decoration_vertices_vertex_array;
	shadow.scale = {1.0f,1.0f,1.0f,};
	shadow.rotation = glm::identity<glm::quat>();
	shadow.environment = Resources::instance().light_environment.environment;
	shadow.framebuffer = Resources::instance().light_environment.depth;

	shadow.id = blue::Context::renderer().add(shadow);
	decoration_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::upload_clickable()
{
	auto attributes = Tile::get_attributes();
	auto vertex_array_future = blue::Context::gpu_system().submit(CreateMeshEntity{ clickable_vertices, clickable_indices, attributes, clickable_tiles * 6 });
	vertex_array_future.wait();
	VertexArray clickable_vertices_vertex_array = vertex_array_future.get();

	// Submit render command consisting of compiled shader, uploaded mesh and following geometry properties:

	RenderEntity entity;
	entity.position = { 0, 0, 0 };
	entity.shader = Resources::instance().shaders.clickable_map;
	entity.vertex_array = clickable_vertices_vertex_array;
	entity.scale = {1.0f,1.0f,1.0f};
	entity.rotation = glm::identity<glm::quat>();
	//    entity.environment = Resources::instance().light_environment.environment;
	entity.environment = Resources::instance().map_environment.environment;
	entity.textures[0] = Resources::instance().light_environment.depth.texture;
	entity.framebuffer.framebuffer = 0;

	RenderEntity shadow;
	shadow.position = { 0, 0, 0 };
	shadow.shader = Resources::instance().shaders.simple_depth;
	shadow.vertex_array = clickable_vertices_vertex_array;
	shadow.scale = {1.0f,1.0f,1.0f};
	shadow.rotation = glm::identity<glm::quat>();
	shadow.environment = Resources::instance().light_environment.environment;
	shadow.framebuffer = Resources::instance().light_environment.depth;

	shadow.id = blue::Context::renderer().add(shadow);
	clickable_vertices_render_entity = blue::Context::renderer().add(entity);
}

void Map::color_clickable_points(float x, float y, float R, const glm::vec3& color)
{
	color_points(x, y, R, color, clickable_vertices);
}

void Map::color_decoration_points(float x, float y, float R, const glm::vec3& color)
{
	color_points(x, y, R, color, decoration_vertices);
}

void Map::elevate_clickable_points(float x, float y, float R, float elevation)
{
	elevate_points(x, y, R, elevation, clickable_vertices);
}

void Map::elevate_decoration_points(float x, float y, float R, float elevation)
{
	elevate_points(x, y, R, elevation, decoration_vertices);
}

void Map::elevate_points(float x, float y, float R, float elevation, Vertices& vertices)
{
	// Iterate over every 2 triangles
	for (std::size_t index = 0; index < vertices.size(); index += (9 * 4))
	{
		// TODO: Check if this vertex is adjacent to clickable tile, if so, ignore it
		// to avoid voids in terrain.

		bool visited[6] = { false };

		// First triangle indexes:  0, 1, 2
		{
			glm::vec3 p_1 = { vertices[index + 0 + 0] , vertices[index + 0 + 1],vertices[index + 0 + 2] };
			glm::vec3 p_2 = { vertices[index + 9 + 0] , vertices[index + 9 + 1],vertices[index + 9 + 2] };
			glm::vec3 p_3 = { vertices[index + 18 + 0] , vertices[index + 18 + 1],vertices[index + 18 + 2] };

			if (glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_1.y += elevation;
				visited[0] = true;
			}

			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += elevation;
				visited[1] = true;
			}

			if (glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += elevation;
				visited[2] = true;
			}

			const auto normal = glm::triangleNormal(p_1, p_2, p_3);

			vertices[index + 0 + 0] = p_1.x;
			vertices[index + 0 + 1] = p_1.y;
			vertices[index + 0 + 2] = p_1.z;

			vertices[index + 0 + 3] = normal.x;
			vertices[index + 0 + 4] = normal.y;
			vertices[index + 0 + 5] = normal.z;

			vertices[index + 9 + 0] = p_2.x;
			vertices[index + 9 + 1] = p_2.y;
			vertices[index + 9 + 2] = p_2.z;

			vertices[index + 9 + 3] = normal.x;
			vertices[index + 9 + 4] = normal.y;
			vertices[index + 9 + 5] = normal.z;

			vertices[index + 18 + 0] = p_3.x;
			vertices[index + 18 + 1] = p_3.y;
			vertices[index + 18 + 2] = p_3.z;

			vertices[index + 18 + 3] = normal.x;
			vertices[index + 18 + 4] = normal.y;
			vertices[index + 18 + 5] = normal.z;
		}
		// Second traingle indexes: 2, 3, 0
		{
			glm::vec3 p_1 = { vertices[index + 18 + 0] , vertices[index + 18 + 1],vertices[index + 18 + 2] };
			glm::vec3 p_2 = { vertices[index + 27 + 0] , vertices[index + 27 + 1],vertices[index + 27 + 2] };
			glm::vec3 p_3 = { vertices[index + 0 + 0] , vertices[index + 0 + 1],vertices[index + 0 + 2] };

			if (!visited[2] && glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_1.y += elevation;
			}

			if (glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_2.y += elevation;
			}

			if (!visited[0] && glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R)
			{
				p_3.y += elevation;
			}

			const auto normal = glm::triangleNormal(p_1, p_2, p_3);

			vertices[index + 18 + 0] = p_1.x;
			vertices[index + 18 + 1] = p_1.y;
			vertices[index + 18 + 2] = p_1.z;

			vertices[index + 18 + 3] = normal.x;
			vertices[index + 18 + 4] = normal.y;
			vertices[index + 18 + 5] = normal.z;

			vertices[index + 27 + 0] = p_2.x;
			vertices[index + 27 + 1] = p_2.y;
			vertices[index + 27 + 2] = p_2.z;

			vertices[index + 27 + 3] = normal.x;
			vertices[index + 27 + 4] = normal.y;
			vertices[index + 27 + 5] = normal.z;

			vertices[index + 0 + 0] = p_3.x;
			vertices[index + 0 + 1] = p_3.y;
			vertices[index + 0 + 2] = p_3.z;

			vertices[index + 0 + 3] = normal.x;
			vertices[index + 0 + 4] = normal.y;
			vertices[index + 0 + 5] = normal.z;
		}
	}
}

void Map::color_points(float x, float y, float R, const glm::vec3& color, Vertices& vertices)
{
	// Iterate over every 2 triangles
	for (std::size_t index = 0; index < vertices.size(); index += (9 * 4))
	{
		// TODO: Check if this vertex is adjacent to clickable tile, if so, ignore it
		// to avoid voids in terrain.

		glm::vec3 clickable_color_1 = color;
		glm::vec3 clickable_color_2 = color;
		int random_variable = 10 + std::rand() / ((RAND_MAX + 1u) / 16);  // Note: 1+rand()%6 is biased
		float color_delta = 0.75f / random_variable;
		if (random_variable % 2)
		{
			clickable_color_1.x += color_delta;
			clickable_color_1.y += color_delta;
			clickable_color_1.z += color_delta;

			clickable_color_2.x -= color_delta;
			clickable_color_2.y -= color_delta;
			clickable_color_2.z -= color_delta;
		}
		else
		{
			clickable_color_1.x -= color_delta;
			clickable_color_1.y -= color_delta;
			clickable_color_1.z -= color_delta;

			clickable_color_2.x += color_delta;
			clickable_color_2.y += color_delta;
			clickable_color_2.z += color_delta;
		}

		// First triangle indexes:  0, 1, 2
		{
			glm::vec3 p_1 = { vertices[index + 0 + 0] , vertices[index + 0 + 1],vertices[index + 0 + 2] };
			glm::vec3 p_2 = { vertices[index + 9 + 0] , vertices[index + 9 + 1],vertices[index + 9 + 2] };
			glm::vec3 p_3 = { vertices[index + 18 + 0] , vertices[index + 18 + 1],vertices[index + 18 + 2] };

			if (glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R
				|| glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R
				|| glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R
					)
			{
				vertices[index + 0 + 6] = clickable_color_1.x;
				vertices[index + 0 + 7] = clickable_color_1.y;
				vertices[index + 0 + 8] = clickable_color_1.z;

				vertices[index + 9 + 6] = clickable_color_2.x;
				vertices[index + 9 + 7] = clickable_color_2.y;
				vertices[index + 9 + 8] = clickable_color_2.z;

				vertices[index + 18 + 6] = clickable_color_2.x;
				vertices[index + 18 + 7] = clickable_color_2.y;
				vertices[index + 18 + 8] = clickable_color_2.z;
			}
		}
		// Second traingle indexes: 2, 3, 0
		{
			glm::vec3 p_1 = { vertices[index + 18 + 0] , vertices[index + 18 + 1],vertices[index + 18 + 2] };
			glm::vec3 p_2 = { vertices[index + 27 + 0] , vertices[index + 27 + 1],vertices[index + 27 + 2] };
			glm::vec3 p_3 = { vertices[index + 0 + 0] , vertices[index + 0 + 1],vertices[index + 0 + 2] };

			if (glm::distance(glm::vec3{ p_1.x, 0, p_1.z }, glm::vec3{ x, 0, y }) <= R ||
				glm::distance(glm::vec3{ p_2.x, 0, p_2.z }, glm::vec3{ x, 0, y }) <= R ||
				glm::distance(glm::vec3{ p_3.x, 0, p_3.z }, glm::vec3{ x, 0, y }) <= R
					)
			{
				vertices[index + 18 + 6] = clickable_color_2.x;
				vertices[index + 18 + 7] = clickable_color_2.y;
				vertices[index + 18 + 8] = clickable_color_2.z;

				vertices[index + 27 + 6] = clickable_color_2.x;
				vertices[index + 27 + 7] = clickable_color_2.y;
				vertices[index + 27 + 8] = clickable_color_2.z;

				vertices[index + 0 + 6] = clickable_color_1.x;
				vertices[index + 0 + 7] = clickable_color_1.y;
				vertices[index + 0 + 8] = clickable_color_1.z;
			}
		}
	}
}
