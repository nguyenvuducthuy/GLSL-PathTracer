#include "Material.h"
#include "Scene.h"

namespace GLSLPT
{
	void Material::setTexture(TexId texId)
	{
		switch (texId.type)
		{
			case TextureType::albedo:
				albedoTexID = texId.id;
				break;
			case TextureType::metallic_roughness:
				metallicRoughnessTexID = texId.id;
				break;
			case TextureType::normal:
				normalmapTexID = texId.id;
				break;
		}
	}
}
