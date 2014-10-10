#include <gl/glew.h>
#include "ObjModel.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "Texture.h"
#include <algorithm>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::string replace(std::string str, std::string toReplace, std::string replacement)
{
	size_t index = 0;
	while (true) 
	{
		 index = str.find(toReplace, index);
		 if (index == std::string::npos) 
			 break;
		 str.replace(index, toReplace.length(), replacement);
		 ++index;
	}
	return str;
}

std::vector<std::string> split(std::string str, std::string sep)
{
	std::vector<std::string> ret;
	size_t index;
	while(true)
	{
		index = str.find(sep);
		if(index == std::string::npos)
			break;
		ret.push_back(str.substr(0, index));
		str = str.substr(index+1);
	}
	ret.push_back(str);
	return ret;
}

inline std::string toLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}


ObjModel::ObjModel(std::string fileName)
{
	std::string dirName = fileName;
	if(dirName.rfind("/") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("/"));
	if(dirName.rfind("\\") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("\\"));
	if(fileName == dirName)
		dirName = "";


	std::ifstream pFile(fileName.c_str());

	std::vector<float>	vertices;
	std::vector<float>	normals;
	std::vector<float>	texcoords;

	std::vector<float> finalVertices;


	ObjGroup* currentGroup = new ObjGroup();
	currentGroup->end = -1;
	currentGroup->start = 0;
	currentGroup->materialIndex = -1;


	while(!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		
		line = replace(line, "\t", " ");
		while(line.find("  ") != std::string::npos)
			line = replace(line, "  ", " ");
		if(line == "")
			continue;
		if(line[0] == ' ')
			line = line.substr(1);
		if(line == "")
			continue;
		if(line[line.length()-1] == ' ')
			line = line.substr(0, line.length()-1);
		if(line == "")
			continue;
		if(line[0] == '#')
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if(params[0] == "v")
		{
			vertices.push_back(atof(params[1].c_str()));
			vertices.push_back(atof(params[2].c_str()));
			vertices.push_back(atof(params[3].c_str()));
		}
		else if(params[0] == "vn")
		{
			normals.push_back(atof(params[1].c_str()));
			normals.push_back(atof(params[2].c_str()));
			normals.push_back(atof(params[3].c_str()));
		}
		else if(params[0] == "vt")
		{
			texcoords.push_back(atof(params[1].c_str()));
			texcoords.push_back(atof(params[2].c_str()));
		}
		else if(params[0] == "f")
		{
			for(size_t ii = 4; ii <= params.size(); ii++)
			{
				for(size_t i = ii-3; i < ii; i++)
				{
					std::vector<std::string> indices = split(params[i == (ii-3) ? 1 : i], "/");
					float px = 0, py = 0, pz = 0, tx = 0, ty = 0, nx = 0, ny = 0, nz = 0;
					if(indices.size() >= 1)
					{
						px = vertices[(atoi(indices[0].c_str())-1) * 3+0];
						py = vertices[(atoi(indices[0].c_str())-1) * 3+1];
						pz = vertices[(atoi(indices[0].c_str())-1) * 3+2];
					}
					if(indices.size() == 2) //texture 
					{
						tx = texcoords[(atoi(indices[1].c_str())-1) * 2+0];
						ty = texcoords[(atoi(indices[1].c_str())-1) * 2+1];
					}
					if(indices.size() == 3)
					{
						if( indices[1] != "")
						{
							tx = texcoords[(atoi(indices[1].c_str())-1) * 2+0];
							ty = texcoords[(atoi(indices[1].c_str())-1) * 2+1];
						}
						nx = normals[(atoi(indices[2].c_str())-1) * 3+0];
						ny = normals[(atoi(indices[2].c_str())-1) * 3+1];
						nz = normals[(atoi(indices[2].c_str())-1) * 3+2];
					}
					finalVertices.push_back(px);
					finalVertices.push_back(py);
					finalVertices.push_back(pz);
					finalVertices.push_back(nx);
					finalVertices.push_back(ny);
					finalVertices.push_back(nz);
					finalVertices.push_back(tx);
					finalVertices.push_back(ty);
					currentGroup->end = finalVertices.size() / 8;
				}
			}
		}
		else if(params[0] == "s")
		{
		}
        else if(params[0] == "mtllib")
        {
            loadMaterialFile(dirName + "/" + params[1], dirName);
        }
		else if(params[0] == "usemtl")
		{
			if(currentGroup->end != -1)
				groups.push_back(currentGroup);
			currentGroup = new ObjGroup();
			currentGroup->start = finalVertices.size()/8;
			currentGroup->materialIndex = -1;

			for(size_t i = 0; i < materials.size(); i++)
			{
				MaterialInfo* info = materials[i];
				if(info->name == params[1])
				{
					currentGroup->materialIndex = i;
					break;
				}
			}
			if(currentGroup->materialIndex == -1)
				std::cout<<"Could not find material name "<<params[1]<<std::endl;
		}
		//else
		//	std::cout<<line<<std::endl;

	}

	groups.push_back(currentGroup);


    glGenVertexArrays(1, &_vertexArray);
    glBindVertexArray(_vertexArray);
        
    GLuint _vertexBuffer;
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, finalVertices.size()*sizeof(GLfloat), &finalVertices[0], GL_STATIC_DRAW);
        
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(24));
        
    glBindVertexArray(0);        


}


ObjModel::~ObjModel(void)
{
}




void ObjModel::draw()
{
    glBindVertexArray(_vertexArray);
	for(size_t i = 0; i < groups.size(); i++)
	{
		ObjGroup* group = groups[i];
		MaterialInfo* material = materials[group->materialIndex];
		if(material->hasTexture)
			glBindTexture(GL_TEXTURE_2D, material->texture->textureId);
		
		glDrawArrays(GL_TRIANGLES, group->start, group->end - group->start);
	}
}

void ObjModel::loadMaterialFile( std::string fileName, std::string dirName )
{
	std::ifstream pFile(fileName.c_str());

	MaterialInfo* currentMaterial = NULL;

	while(!pFile.eof())
	{
		std::string line;
		std::getline(pFile, line);
		
		line = replace(line, "\t", " ");
		while(line.find("  ") != std::string::npos)
			line = replace(line, "  ", " ");
		if(line == "")
			continue;
		if(line[0] == ' ')
			line = line.substr(1);
		if(line == "")
			continue;
		if(line[line.length()-1] == ' ')
			line = line.substr(0, line.length()-1);
		if(line == "")
			continue;
		if(line[0] == '#')
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if(params[0] == "newmtl")
		{
			if(currentMaterial != NULL)
			{
				materials.push_back(currentMaterial);
			}
			currentMaterial = new MaterialInfo();
			currentMaterial->name = params[1];
			currentMaterial->hasTexture = false;
			currentMaterial->texture = NULL;
		}
		else if(params[0] == "map_kd")
		{
			currentMaterial->hasTexture = true;
			try
			{
				currentMaterial->texture = new Texture(dirName + "/" + params[1]);
			}
			catch(char* e)
			{
				std::cout<<e<<std::endl;
			}
		}
		else
			std::cout<<"Didn't parse "<<params[0]<<" in material file"<<std::endl;
	}
	if(currentMaterial != NULL)
		materials.push_back(currentMaterial);

}
