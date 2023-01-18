#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

// The Material class is a concept class for material-related enum classes.
class Material
{
public:
	// Enumerates the types of materials that can be set on various Visualize entities.
	enum class Type : uint32_t
	{
		None = 0,		// No material was set.
		FullMaterial,						// A full material was set.  The MaterialKit is valid.
		RGBColor,							// An RGB color was set.  The RGBColor is valid.
		RGBAColor,							// An RGBA color was set.  The RGBAColor is valid.
		MaterialIndex,						// A material index was set.  The material index (float value) is valid.
		TextureName,						// An unmodulated texture was set.  The texture name is valid.
		ModulatedTexture,					// A modulated texture was set.  Both the RGBColor (or RGBAColor) and texture name are valid.
		CubeMapName,						// An unmodulated cube map texture was set.  The cube map texture name is valid.
		ModulatedCubeMap,					// A modulated cube map was set.  Both the RGBColor (or RGBAColor) and cube map texture name are valid.
		GlossValue,							// A gloss value was set.  The gloss (float value) is valid.
		DiffuseChannelAlpha					// An alpha value was set.  The alpha (float value) is valid.		
	};

	// Material Channel
	enum class Channel : uint32_t
	{
		DiffuseColor = 1,		// Material Channel
		DiffuseTexture = 2,		// Material Channel
		Specular = 3,			// Material Channel
		Emission = 4,			// Material Channel
		Transmission = 5,		// Material Channel
		Mirror = 6,				// Material Channel
		Bump = 7,				// Material Channel
		EnvironmentTexture = 8,	// Material Channel
		EnvironmentCubeMap = 9,	// Material Channel
		Gloss = 10,				// Material Channel
		Alpha = 11,				// Material Channel
		Count = 12
	};

	// The Material Color Class*/
	class Color
	{
	public:
		enum class Type : uint32_t
		{
			Diffuse,
			Specular,
			Emission,
			Count
		};

		// Material Color Channel
		enum class Channel : uint32_t
		{
			DiffuseColor = 1,				// Material Color Channel
			Specular = 3,					// Material Color Channel
			Emission = 4,					// Material Color Channel
			Mirror = 6						// Material Color Channel
		};

	private:
		Color() {}
	};

	// The Material::Texture class is a concept class for texture-related enum classes.
	class Texture
	{
	public:

		// Material Texture Channel
		enum class Channel : uint32_t
		{
			DiffuseTexture = 2,			// Material Texture Channel
			Specular = 3,				// Material Texture Channel
			Emission = 4,				// Material Texture Channel
			Transmission = 5,			// Material Texture Channel
			Mirror = 6,					// Material Texture Channel
			Bump = 7,					// Material Texture Channel
			EnvironmentTexture = 8,		// Material Texture Channel
			EnvironmentCubeMap = 9		// Material Texture Channel
		};

		// Enumeration of the parameterization sources for textures.
		enum class Parameterization : uint32_t
		{
			// Use a cylindrical mapping for the texture.
			Cylinder,

			// Texture Parameterization.
			PhysicalReflection,

			// Use the (x,y,z) coordinates for the object before transforms have been applied for the texture parameters.
			Object,

			// Use the natural mapping for meshes, NURBS surfaces and shells for the texture.
			// For meshes, textures will be stretched in the range [0, 1].
			// For NURBS surfaces, textures will be mapped in the range [0, (control point count - degree)].
			// For shells, textures will be mapped such that u = x + z and v = x + y
			// where (x, y, z) is in object space.
			NaturalUV,

			// Texture Parameterization.
			ReflectionVector,

			// Texture Parameterization.
			SurfaceNormal,

			// Use a spherical mapping for the texture.
			Sphere,

			// Use the mapping explicitly defined on the geometry for the texture.
			UV,

			// Use the (x, y, z) coordinates for the object after transforms have been applied for the texture parameters.
			World
		};

		// Enumeration of the tiling modes for textures.
		enum class Tiling : uint32_t
		{
			// The texture will get mapped normally for parameters in the range [0,1], but parameters less than 0
			// will be clamped to 0 and parameters greater than 1 will be clamped to 1.
			Clamp,

			// The texture will get mapped repeatedly in a modulo fashion.
			Repeat,

			// The texture will get mapped repeatedly in a modulo fashion.  Every other modulo will also be inverted.
			Reflect,

			// The texture will get mapped normally for parameters in the range [0,1], but parameters outside that range will act as if the
			// texture at that location is transparent.
			Trim
		};

		//Enumeration of the various interpolation filters for textures.  These are used when the texture needs to be magnified.
		enum class Interpolation : uint32_t
		{
			// The texture coordinate will map to the closest texture samples.
			None,

			// The texture data from the four closest texture samples to a texture coordinate will be blended via a weighted average.
			Bilinear
		};

		// Enumeration of the various decimation filters for textures.  These are used when the texture needs to minimized.
		enum class Decimation : uint32_t
		{
			// No down-sampling will be performed.  Data will be retrieved via the specified Texture::Interpolation filter.
			None,

			// The texture will be down-sampled to non-square image sizes which will get used for sampling and filtering based on the
			// angle between the normal of a textured surface and the view vector.
			Anisotropic,

			// The texture will be down-sampled to square, power-of-two-sized images which will get used for sampling and filtering.
			Mipmap
		};

		// Material Texture ChannelMapping
		enum class ChannelMapping : uint32_t
		{
			Red,		// Material Texture ChannelMapping
			Green,		// Material Texture ChannelMapping
			Blue,		// Material Texture ChannelMapping
			Alpha,		// Material Texture ChannelMapping
			Zero,		// Material Texture ChannelMapping
			One,		// Material Texture ChannelMapping
			Luminance	// Material Texture ChannelMapping
		};

	private:
		Texture() {}
	};

private:
	Material() {}
};

CLOSE_3DF_NAMESPACE