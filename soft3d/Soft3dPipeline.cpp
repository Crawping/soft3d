#include <stdlib.h>
#include "soft3d.h"
#include "SceneManager.h"
#include "DirectXHelper.h"
#include "VertexProcessor.h"
#include "FragmentProcessor.h"
#include "Rasterizer.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace vmath;

namespace soft3d
{

	/*Color operator*(const Color& lf, float ratio)
	{
		Color cc;
		cc.B = lf.B * ratio;
		cc.G = lf.G * ratio;
		cc.R = lf.R * ratio;
		cc.A = lf.A * ratio;
		return cc;
	}*/

	/*Color operator+(const Color& lf, const Color& rf)
	{
		Color cc;
		cc.B = lf.B + rf.B;
		cc.G = lf.G + rf.G;
		cc.R = lf.R + rf.R;
		cc.A = lf.A + rf.A;
		return cc;
	}*/


	std::shared_ptr<Soft3dPipeline> Soft3dPipeline::s_instance(new Soft3dPipeline());

	Soft3dPipeline::Soft3dPipeline()
	{
		for (int i = 0; i < 16; i++)
		{
			m_uniforms[i] = nullptr;
		}
	}


	Soft3dPipeline::~Soft3dPipeline()
	{
		for (int i = 0; i < 16; i++)
		{
			if (m_uniforms[i] != nullptr)
				delete m_uniforms[i];
		}
	}

	void Soft3dPipeline::InitPipeline(HWND hwnd, uint16 width, uint16 height)
	{
		m_width = width;
		m_height = height;
		m_rasterizer = shared_ptr<Rasterizer>(new Rasterizer(width, height));
		soft3d::DirectXHelper::Instance()->Init(hwnd);
		SceneManager::Instance()->InitScene(width, height);
	}

	void Soft3dPipeline::SetVBO(shared_ptr<VertexBufferObject> vbo)
	{
		shared_ptr<PipeLineData> pd(new PipeLineData());
		pd->vp = boost::shared_array<VertexProcessor>(new VertexProcessor[vbo->GetSize()]);
		pd->cullMode = vbo->m_cullMode;
		pd->capacity = vbo->GetSize();

		m_pipeDataVector.push_back(pd);
		m_vboVector.push_back(vbo);
	}

	void Soft3dPipeline::SetUniform(uint16 index, void* uniform)
	{
		if (index < 16)
		{
			if (m_uniforms[index] != nullptr)
			{
				delete m_uniforms[index];
				m_uniforms[index] = nullptr;
			}
			m_uniforms[index] = uniform;
		}
	}

	void Soft3dPipeline::SetTexture(shared_ptr<Texture> tex)
	{
		m_tex = tex;
	}

	int Soft3dPipeline::Clear(uint32 color)
	{
		m_rasterizer->Clear(color);
		return 0;
	}

	void Soft3dPipeline::Process()
	{
		SceneManager::Instance()->Update();

		for (int idx = 0; idx < m_pipeDataVector.size(); idx++)
		{
			shared_ptr<PipeLineData>& pipeData = m_pipeDataVector[idx];
			VertexBufferObject* vbo = m_vboVector[idx].get();
			for (int i = 0; i < pipeData->capacity; i++)
			{
				VertexProcessor& cur_vp = pipeData->vp[i];
				const uint32* colorptr = nullptr;
				if (vbo->useIndex())
				{
					colorptr = vbo->GetColor(vbo->GetIndex(i));
					cur_vp.pos = vbo->GetPos(vbo->GetIndex(i));
				}
				else
				{
					colorptr = vbo->GetColor(i);
					cur_vp.pos = vbo->GetPos(i);
				}
				if (colorptr != nullptr)
					cur_vp.color = colorptr;
				else
					cur_vp.color = (uint32*)this;//随机颜色
				cur_vp.normal = vbo->GetNormal(i);

				if (vbo->hasUV())
					cur_vp.vs_out.uv = *(vbo->GetUV(i));

				cur_vp.uniforms = m_uniforms;
				cur_vp.Process();//这一步进行视图变换和投影变换

				//除以w
				float rhw = 1.0f / cur_vp.vs_out.pos[3];
				cur_vp.vs_out.pos[0] *= rhw;
				cur_vp.vs_out.pos[1] *= rhw;
				cur_vp.vs_out.pos[2] *= rhw;
				cur_vp.vs_out.pos[3] = 1.0f;
				cur_vp.vs_out.rhw = rhw;

				cur_vp.vs_out.pos[0] = (cur_vp.vs_out.pos[0] + 1.0f) * 0.5f * m_width;
				cur_vp.vs_out.pos[1] = (cur_vp.vs_out.pos[1] + 1.0f) * 0.5f * m_height;

				cur_vp.vs_out.uv *= rhw;//uv在这里除以w，以后乘回来，为了能正确计算纹理uv
			}

			for (int i = 0; i < pipeData->capacity; i += 3)
			{
				VertexProcessor& vp1 = pipeData->vp[i];
				VertexProcessor& vp2 = pipeData->vp[i + 1];
				VertexProcessor& vp3 = pipeData->vp[i + 2];

				VertexBufferObject::CULL_MODE cull_mode = VertexBufferObject::CULL_NONE;
				//进行背面拣选
				vec4 a = vp1.vs_out.pos - vp2.vs_out.pos;
				vec4 b = vp2.vs_out.pos - vp3.vs_out.pos;
				vec3 c = vec3(a[0], a[1], a[2]);
				vec3 d = vec3(b[0], b[1], b[2]);
				vec3 r = cross(c, d);
				if (r[2] < 0.0f)
					cull_mode = VertexBufferObject::CULL_CW;
				else if (r[2] > 0.0f)
					cull_mode = VertexBufferObject::CULL_CCW;

				if (vbo->m_cullMode != VertexBufferObject::CULL_NONE && vbo->m_cullMode != cull_mode)
					continue;

				//todo:进行裁剪
				//

				//make triangle always ccw sorting
				uint32 index[3] = { i, i + 1, i + 2 };
				if (cull_mode == VertexBufferObject::CULL_CW)
				{
					index[0] = i + 2;
					index[2] = i;
				}

				switch (vbo->m_mode)
				{
				case VertexBufferObject::RENDER_LINE:
				{
					m_rasterizer->BresenhamLine(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out));
					m_rasterizer->BresenhamLine(&(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
					m_rasterizer->BresenhamLine(&(pipeData->vp[index[2]].vs_out), &(pipeData->vp[index[0]].vs_out));
					break;
				}

				case VertexBufferObject::RENDER_TRIANGLE:
				{
					m_rasterizer->Triangle(&(pipeData->vp[index[0]].vs_out), &(pipeData->vp[index[1]].vs_out), &(pipeData->vp[index[2]].vs_out));
					break;
				}

				default:
					break;
				}
			}
		}

		DirectXHelper::Instance()->Paint(m_rasterizer->GetFrameBuffer(), m_width, m_height);
	}

}