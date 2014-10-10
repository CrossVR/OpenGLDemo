#include "Texture.h"
#include <FreeImage.h>

Texture::Texture(std::string filename)
{
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		throw "Unknown file";

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename.c_str());
	//if the image failed to load, return failure
	if(!dib)
		throw "Failed to load";

	dib = FreeImage_ConvertTo32Bits(dib);

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
		throw "wtf";
	

	for(int i = 0; i < width*height; i++)
	{
		BYTE b = bits[4*i];
		bits[4*i] = bits[4*i+2];
		bits[4*i+2] = b;
	}


	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &textureId);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, textureId);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);
}


Texture::~Texture(void)
{
	glDeleteTextures(1, &textureId);
}
