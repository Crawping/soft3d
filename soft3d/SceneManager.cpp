#include "stdafx.h"
#include "SceneManager.h"
#include "VertexBufferObject.h"
#include "vmath.h"
#include "Soft3dPipeline.h"
#include <time.h>

using namespace std;
using namespace vmath;

namespace soft3d
{
	shared_ptr<SceneManager> SceneManager::s_instance(new SceneManager());

	SceneManager::SceneManager()
	{
	}


	SceneManager::~SceneManager()
	{
	}


	shared_ptr<VertexBufferObject> vbo(new VertexBufferObject(5));
	void SceneManager::Init()
	{
		float cube[] = {
			1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			0.0f,
			-1.0f, -1.0f, 1.0f, 1.0f,
			0.0f,

			1.0f, 1.0f, -1.0f, 1.0f,
			0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f,
			0.0f,
			1.0f, -1.0f, -1.0f, 1.0f,
			0.0f,
			-1.0f, -1.0f, -1.0f, 1.0f,
			0.0f,
		};
		(*(uint32*)&(cube[4])) = 0xff0000;
		(*(uint32*)&(cube[9])) = 0x00ff00;
		(*(uint32*)&(cube[14])) = 0x0000ff;
		(*(uint32*)&(cube[19])) = 0xff0000;

		(*(uint32*)&(cube[24])) = 0xffff00;
		(*(uint32*)&(cube[29])) = 0xff00ff;
		(*(uint32*)&(cube[34])) = 0x00ffff;
		(*(uint32*)&(cube[39])) = 0xffff00;

		uint32 cubeIndex[] = {
			0, 1, 2,
			1, 3, 2,
			4, 5, 0,
			5, 1, 0,
			6, 7, 4,
			4, 7, 5,
			2, 3, 6,
			3, 7, 6,
			6, 4, 0,
			0, 2, 6,
			1, 5, 7,
			7, 3, 1,
		};

		vbo->CopyFromBuffer(cube, sizeof(cube)/20);
		vbo->m_posOffset = 0;
		vbo->m_colorOffset = 4;

		vbo->CopyIndexBuffer(cubeIndex, sizeof(cubeIndex)/4);
	}

	void SceneManager::Update()
	{
		float aspect = (float)DirectXHelper::Instance()->GetWidth() / (float)DirectXHelper::Instance()->GetHeight();
		mat4 proj_matrix = perspective(60.0f, aspect, 0.1f, 1000.0f);
		mat4 view_matrix = lookat(vec3(0.0f, -2.0f, 4.0f),
			vec3(0.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f));
		float factor = GetTickCount() / 10 % 360;
		mat4 mv_matrix = view_matrix * translate(0.0f, 0.0f, 0.0f) * scale(1.0f) * rotate(factor, vec3(0.0f, 1.0f, 0.0f));

		vbo->mv_matrix = mv_matrix;
		vbo->proj_matrix = proj_matrix;

		Soft3dPipeline::Instance()->SetVBO(vbo);
	}

}