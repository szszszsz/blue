#pragma once

#include "blue/Renderer.h"
#include "blue/camera/PerspectiveCamera.hpp"
#include "terrain/Tile.hpp"
#include <mutex>

class Map
{
	friend class MapIntersectionJob;

public:

	static const int CHUNK_DIMENSION = 32;

	void upload_clickable_vertices();
	void upload_decoration_vertices();

	void toggle_tile(std::uint16_t x, std::uint16_t y);
	void dispose_current_map_on_gpu();
	void dispose_current_decoration_on_gpus();
	void upload_decoration();

	void elevate_points(float x, float y, float R, float elevation);
	void color_points(float x, float y, float R, const glm::vec3& color);
	void shuffle_color_points(float x, float y, float R);

	void import_from_file(const std::string& filename);
	void export_to_file(const std::string& filename);

private:

	std::uint32_t decoration_tiles = 0;

	Vertices decoration_vertices;
	Indices decoration_indices;

	std::mutex tiles_access;
	Tile tiles[CHUNK_DIMENSION][CHUNK_DIMENSION];

	RenderEntityId clickable_vertices_render_entity;
	RenderEntityId decoration_vertices_render_entity;
};
