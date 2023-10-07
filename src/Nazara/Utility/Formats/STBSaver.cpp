// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/STBSaver.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/Utility/Formats/STBLoader.hpp>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <stdexcept>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		using FormatHandler = bool(*)(const Image& image, const ImageParams& parameters, Stream& stream);

		int ConvertToFloatFormat(Image& image)
		{
			switch (image.GetFormat())
			{
				case PixelFormat::R32Float:
					return 1;

				case PixelFormat::RG32Float:
					return 2;

				case PixelFormat::RGB32Float:
					return 3;

				case PixelFormat::RGBA32Float:
					return 4;

				default:
				{
					if (PixelFormatInfo::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormat::RGBA32Float))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormat::RGB32Float))
							break;

						return 3;
					}
				}
			}

			return 0;
		}

		int ConvertToIntegerFormat(Image& image)
		{
			switch (image.GetFormat())
			{
				case PixelFormat::L8Unorm:
				case PixelFormat::R8Unorm:
					return 1;

				case PixelFormat::LA8Unorm:
				case PixelFormat::RG8Unorm:
					return 2;

				case PixelFormat::RGB8Unorm:
					return 3;

				case PixelFormat::RGBA8Unorm:
					return 4;

				default:
				{
					if (PixelFormatInfo::HasAlpha(image.GetFormat()))
					{
						if (!image.Convert(PixelFormat::RGBA8Unorm))
							break;

						return 4;
					}
					else
					{
						if (!image.Convert(PixelFormat::RGB8Unorm))
							break;

						return 3;
					}
				}
			}

			return 0;
		}

		void WriteToStream(void* userdata, void* data, int size)
		{
			Stream* stream = static_cast<Stream*>(userdata);
			if (stream->Write(data, size) != static_cast<std::size_t>(size))
				throw std::runtime_error("Failed to write to stream");
		}

		bool SaveBMP(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required
			
			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_bmp_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels()))
			{
				NazaraError("failed to write BMP to stream");
				return false;
			}

			return true;
		}

		bool SaveJPEG(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("failed to convert image to suitable format");
				return false;
			}

			long long imageQuality = parameters.custom.GetIntegerParameter("JPEGQuality").GetValueOr(100);
			if (imageQuality <= 0 || imageQuality > 100)
			{
				NazaraError("NativeJPEGSaver_Quality value ({0}) does not fit in bounds ]0, 100], clamping...", imageQuality);
				imageQuality = Nz::Clamp(imageQuality, 1LL, 100LL);
			}

			if (!stbi_write_jpg_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels(), int(imageQuality)))
			{
				NazaraError("failed to write JPEG to stream");
				return false;
			}

			return true;
		}

		bool SaveHDR(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToFloatFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_hdr_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, reinterpret_cast<const float*>(tempImage.GetConstPixels())))
			{
				NazaraError("Failed to write HDR to stream");
				return false;
			}

			return true;
		}

		bool SavePNG(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_png_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels(), 0))
			{
				NazaraError("Failed to write PNG to stream");
				return false;
			}

			return true;
		}

		bool SaveTGA(const Image& image, const ImageParams& parameters, Stream& stream)
		{
			NazaraUnused(parameters);

			Image tempImage(image); //< We're using COW here to prevent Image copy unless required

			int componentCount = ConvertToIntegerFormat(tempImage);
			if (componentCount == 0)
			{
				NazaraError("Failed to convert image to suitable format");
				return false;
			}

			if (!stbi_write_tga_to_func(&WriteToStream, &stream, tempImage.GetWidth(), tempImage.GetHeight(), componentCount, tempImage.GetConstPixels()))
			{
				NazaraError("Failed to write TGA to stream");
				return false;
			}

			return true;
		}

		constexpr FormatHandler test = &SaveBMP;

		constexpr frozen::unordered_map s_formatHandlers = frozen::make_unordered_map<frozen::string, FormatHandler>({
			{ ".bmp",  &SaveBMP },
			{ ".hdr",  &SaveHDR },
			{ ".jpg",  &SaveJPEG },
			{ ".jpeg", &SaveJPEG },
			{ ".png",  &SavePNG },
			{ ".tga",  &SaveTGA }
		});
		
		bool FormatQuerier(std::string_view extension)
		{
			return s_formatHandlers.find(extension) != s_formatHandlers.end();
		}

		bool SaveToStream(const Image& image, const std::string& format, Stream& stream, const ImageParams& parameters)
		{
			NazaraUnused(parameters);

			if (!image.IsValid())
			{
				NazaraError("Invalid image");
				return false;
			}

			ImageType type = image.GetType();
			if (type != ImageType::E1D && type != ImageType::E2D)
			{
				NazaraError("Image type {0:#x}) is not in a supported format", UnderlyingCast(type));
				return false;
			}

			auto it = s_formatHandlers.find(std::string_view(format));
			NazaraAssert(it != s_formatHandlers.end(), "Invalid handler");

			const FormatHandler& handler = it->second;
			try
			{
				return handler(image, parameters, stream);
			}
			catch (const std::exception& e)
			{
				NazaraError(e.what());
				return false;
			}
		}
	}

	namespace Loaders
	{
		ImageSaver::Entry GetImageSaver_STB()
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			ImageSaver::Entry entry;
			entry.formatSupport = FormatQuerier;
			entry.streamSaver = SaveToStream;

			return entry;
		}
	}
}
