#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "split_bvh.h"

namespace GLSLPT
{	
	enum TextureType
	{
		albedo,
		metallic_roughness,
		normal
	};

	class Texture
	{
	public:
		Texture() : texData(nullptr) {};
		~Texture() { delete texData; }

		bool loadTexture(const std::string &filename, TextureType type);
		
		TextureType texType;
		int width, height;

		// Texture Data
		unsigned char* texData;
	};
}
