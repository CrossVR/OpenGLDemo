#pragma once

#include <string>
#include <vector>

#include <windows.h>
#include <gl/GL.h>

class Texture;

class ObjModel
{
private:
	class MaterialInfo
	{
	public:
		std::string name;
		Texture* texture;

		bool hasTexture;
	};

	class ObjGroup
	{
	public:
		std::string name;
		int start;
		int end;
		int materialIndex;
	};

	std::vector<ObjGroup*> groups;
	std::vector<MaterialInfo*> materials;
	GLuint _vertexArray;

	void loadMaterialFile(std::string fileName, std::string dirName);
public:
	ObjModel(std::string filename);
	~ObjModel(void);

	void draw();
};

