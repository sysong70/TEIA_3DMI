#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Control.h"
#include "Color.h"

namespace H3DF
{
	// The Material class is a concept class for material-related enum classes.
	class API_3DF Material
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
			DiffuseColor = 0,		// Material Channel
			DiffuseTexture = 1,		// Material Channel
			Specular = 2,			// Material Channel
			Emission = 3,			// Material Channel
			Transmission = 4,		// Material Channel
			Mirror = 5,				// Material Channel
			Bump = 6,				// Material Channel
			EnvironmentTexture = 7,	// Material Channel
			EnvironmentCubeMap = 8,	// Material Channel
			Count = 9
			// 		Gloss = 9,				// Material Channel
			// 		Alpha = 10,				// Material Channel
		};

		// The Material Color Class*/
		class Color
		{
		public:
			// Material Color Channel
			enum class Channel : uint32_t
			{
				DiffuseColor = 0,				// Material Color Channel
				Specular = 2,					// Material Color Channel
				Emission = 3,					// Material Color Channel
				Mirror = 5						// Material Color Channel
			};

		private:
			Color() {}
		};

		// The Material::Texture class is a concept class for texture-related enum classes.
		class API_3DF Texture
		{
		public:

			// Material Texture Channel
			enum class Channel : uint32_t
			{
				DiffuseTexture = 1,			// Material Texture Channel
				Specular = 2,				// Material Texture Channel
				Emission = 3,				// Material Texture Channel
				Transmission = 4,			// Material Texture Channel
				Mirror = 5,					// Material Texture Channel
				Bump = 6,					// Material Texture Channel
				EnvironmentTexture = 7,		// Material Texture Channel
				EnvironmentCubeMap = 8		// Material Texture Channel
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
				World,
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
				Trim, 
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

	class API_3DF MaterialKit : public Kit
	{
	public:
		MaterialKit();
		MaterialKit(MaterialKit const & cInKit);

		static const H3DF::Type staticType = H3DF::Type::MaterialKit;
		H3DF::Type ObjectType() const { return staticType; };

		MaterialKit & operator = (MaterialKit const & cInThat);
		bool operator == (MaterialKit const & cInThat) const;
		bool operator != (MaterialKit const & cInThat) const;

		void Show(MaterialKit & cOutKit) const;
		bool Empty() const;

		// Applies an RGB color to the whole diffuse channel, replacing any diffuse textures or shaders.
		MaterialKit & SetDiffuse(RGBColor const & cInColor);
		MaterialKit & SetDiffuse(RGBAColor const & cInColor);

		// Applies an RGB color to the diffuse color channel
		MaterialKit & SetDiffuseColor(RGBColor const & cInColor);
		MaterialKit & SetDiffuseColor(RGBAColor const & cInColor);
		//Applies an alpha channel the diffuse color channel.
		MaterialKit & SetDiffuseAlpha(float fInAlpha);

		MaterialKit & SetDiffuseTexture(CStringA strTextureName);
		MaterialKit & SetDiffuseTexture(CStringA strTextureName, RGBAColor const & cInModulatingColor);
		MaterialKit & SetDiffuseTextureOption(CStringA strTextureOption);

		MaterialKit & SetSpecular(RGBAColor const & cInColor);
		MaterialKit & SetSpecular(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetMirror(RGBAColor const & cInColor);
		MaterialKit & SetMirror(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetTransmission(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetEmission(RGBAColor const & cInColor);
		MaterialKit & SetEmission(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetEnvironmentTexture(CStringA strTextureName);
		MaterialKit & SetEnvironmentTexture(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetEnvironmentCubeMap(CStringA strTextureName);
		MaterialKit & SetEnvironmentCubeMap(CStringA strTextureName, RGBAColor const & cInModulatingColor);

		MaterialKit & SetBump(CStringA strTextureName);
		MaterialKit & SetGloss(float fInGloss);


		// Removes all settings applied to the diffuse rgb channel.
		MaterialKit & UnsetDiffuseColorRGB();
		// Removes all settings applied to the diffuse color channel including alpha.
		MaterialKit & UnsetDiffuseColor();
		MaterialKit & UnsetDiffuseAlpha();
		MaterialKit & UnsetDiffuseTexture();
		MaterialKit & UnsetSpecular();
		MaterialKit & UnsetMirror();
		MaterialKit & UnsetTransmission();
		MaterialKit & UnsetEmission();
		MaterialKit & UnsetEnvironment();
		MaterialKit & UnsetBump();
		MaterialKit & UnsetGloss();
		MaterialKit & UnsetEverything();

		bool ShowDiffuseColor(RGBColor & cOutColor) const;
		bool ShowDiffuseColor(RGBAColor & cOutColor) const;
		bool ShowDiffuseAlpha(float & fOutAlpha) const;
		bool ShowDiffuseTexture(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowSpecular(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;

		bool ShowMirror(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowTransmission(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowEmission(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowEnvironment(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowBump(CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowGloss(float & fOutGloss) const;

		void SetMaterial(CStringA strInGeometryName, bool bFaseFlag = true);
	};

	class API_3DF MaterialMappingKit : public Kit
	{
	public:
		MaterialMappingKit();
		MaterialMappingKit(MaterialMappingKit const & cInKit);

		static const H3DF::Type staticType = H3DF::Type::MaterialMappingKit;
		H3DF::Type ObjectType() const { return staticType; };

		MaterialMappingKit & operator = (MaterialMappingKit const & cInThat);

		//----- Color 설정 -----
		MaterialMappingKit & SetAmbientLightUpColor(RGBAColor const & cInRgbaColor);
		MaterialMappingKit & SetAmbientLightDownColor(RGBAColor const & cInRgbaColor);

		MaterialMappingKit & SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingKit & SetFaceAlpha(float fInAlpha);
		MaterialMappingKit & SetFaceTexture(CStringA strTextureName);
		MaterialMappingKit & SetFaceTextureOption(CStringA strTextureOption);
		MaterialMappingKit & SetFaceGloss(float fInValue);
		MaterialMappingKit & SetFaceMaterial(MaterialKit const & cInMaterial);

		MaterialMappingKit & SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingKit & SetBackFaceAlpha(float fInAlpha);
		MaterialMappingKit & SetBackFaceTexture(CStringA strTextureName);
		MaterialMappingKit & SetBackFaceTextureOption(CStringA strTextureOption);
		MaterialMappingKit & SetBackFaceGloss(float fInValue);
		MaterialMappingKit & SetBackFaceMaterial(MaterialKit const & cInMaterial);

		MaterialMappingKit & SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingKit & SetFrontFaceAlpha(float fInAlpha);
		MaterialMappingKit & SetFrontFaceTexture(CStringA strTextureName);
		MaterialMappingKit & SetFrontFaceTextureOption(CStringA strTextureOption);
		MaterialMappingKit & SetFrontFaceGloss(float fInValue);
		MaterialMappingKit & SetFrontFaceMaterial(MaterialKit const & cInMaterial);

		MaterialMappingKit & SetEdgeAlpha(float fInAlpha);
		MaterialMappingKit & SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingKit & SetEdgeTexture(char const * strTextureName);
		MaterialMappingKit & SetEdgeGloss(float fInValue);
		MaterialMappingKit & SetEdgeMaterial(MaterialKit const & cInMaterial);

		MaterialMappingKit & SetLineAlpha(float fInAlpha);
		MaterialMappingKit & SetLineColor(RGBAColor const & cInRgbaColor);

		MaterialMappingKit & SetMarkerColor(RGBAColor const & cInRgbaColor);

		MaterialMappingKit & SetTextColor(RGBAColor const & cInRgbaColor);

		MaterialMappingKit & SetVertexAlpha(float fInAlpha);
		MaterialMappingKit & SetVertexColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingKit & SetVertexTexture(CStringA strTextureName);
		MaterialMappingKit & SetVertexGloss(float fInValue);
		MaterialMappingKit & SetVertexMaterial(MaterialKit const & cInMaterial);

		MaterialMappingKit & SetGeometryColor(RGBAColor const & cInRgbaColor);

		//----- Operator -----
		bool operator == (MaterialMappingKit const & cInThat) const;
		bool operator != (MaterialMappingKit const & cInThat) const;

		bool ShowAmbientLightUpColor(Material::Type & cOutType, RGBAColor & cOutColor) const;
		bool ShowAmbientLightDownColor(Material::Type & cOutType, RGBAColor & cOutColor) const;

		bool ShowBackFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowBackFaceMaterial(MaterialKit & cOutKit) const;

		bool ShowFrontFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowFrontFaceMaterial(MaterialKit & cOutKit) const;

		bool ShowEdgeChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowEdgeMaterial(MaterialKit & cOutKit) const;

		bool ShowFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		//bool ShowTextureOptions(Material::Channel eInChannel, CStringA & strOutTextureOptions);
		bool ShowFaceMaterial(MaterialKit & cOutKit) const;

		bool ShowLineAlpha(float & fOutAlpha) const;

		bool ShowLineColor(RGBAColor & cOutColor) const;

		bool ShowMarkerColor(RGBAColor & cOutColor) const;

		bool ShowTextColor(RGBAColor & cOutColor) const;

		bool ShowVertexChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowVertexMaterial(MaterialKit & cOutKit) const;
	};

	class API_3DF MaterialMappingControl : public Control
	{
	public:
		MaterialMappingControl(SegmentKey const & cInThat);
		MaterialMappingControl(MaterialMappingControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::MaterialMappingControl;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(MaterialMappingControl const & cInThat);
		MaterialMappingControl & operator = (MaterialMappingControl const & cInThat);

		//== Color 설정 =============================================================================
		MaterialMappingControl & SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingControl & SetFaceAlpha(float fInAlpha);
		MaterialMappingControl & SetFaceTexture(CStringA strInTextureName, Material::Texture::Channel eInChannel = Material::Texture::Channel::DiffuseTexture, size_t nInLayer = 0);

		MaterialMappingControl & SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingControl & SetBackFaceAlpha(float fInAlpha);

		MaterialMappingControl & SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
		MaterialMappingControl & SetFrontFaceAlpha(float fInAlpha);

		MaterialMappingControl & SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);

		MaterialMappingControl & SetMarkerColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
	
		//== Texture 설정 ===========================================================================
		void InitPopulateTextures();
		void InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData);
		void InsertDifaultPicture(UINT nIndex, UINT nSize, UCHAR * pucBinaryData);
		void SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment);
		void SetDefineLocalTexture(UINT nIndex, CStringA strTextureOptions);
		void EndPopulateTextures();

	private:
		MaterialMappingControl & UnSetColor(CStringA strInType);
	};
}