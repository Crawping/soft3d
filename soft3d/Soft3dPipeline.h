#pragma once

#include <windows.h>
#include <vector>
#include <map>
#include "VertexBufferObject.h"
#include "Texture.h"
#include "VertexProcessor.h"
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <dinput.h>

namespace soft3d
{
	class Rasterizer;
	class RasterizerManager;
	struct PipeLineData
	{
		boost::shared_array<VertexProcessor> vp;

		VertexBufferObject::CULL_MODE cullMode;
		VertexBufferObject::RENDER_MODE renderMode;
		uint32 capacity;
	};

	typedef void* UniformPtr;
	typedef UniformPtr* UniformStack;

	typedef char DIKEYBOARD[256];
	typedef boost::function<void(const DIMOUSESTATE& dimouse)> MOUSE_EVENT_CB;
	typedef boost::function<void(const DIKEYBOARD& dikeyboard)> KEYBOARD_EVENT_CB;

	enum UNIFORM_LABEL
	{
		UNIFORM_MV_MATRIX = 0,
		UNIFORM_PROJ_MATRIX,
		UNIFORM_VIEW_MATRIX,

		UNIFORM_LIGHT_DIR = 14,
		UNIFORM_LIGHT_POS = 15,
	};

	class Soft3dPipeline
	{
	public:
		static Soft3dPipeline* Instance()
		{
			return s_instance.get();
		}
		~Soft3dPipeline();
		void InitPipeline(HINSTANCE hInstance, HWND hwnd, uint16 width, uint16 height);
		int SetVBO(std::shared_ptr<VertexBufferObject> vbo);
		void SelectVBO(uint32 vboIndex);
		void SetUniform(uint16 index, void* uniform);
		void SetTexture(std::shared_ptr<Texture> tex);
		const Texture* CurrentTex() {
			return m_tex.get();
		}
		void Process();
		int Clear(uint32 color);

		//input
		void AddMouseEventCB(MOUSE_EVENT_CB cb) {
			m_mouseCB.push_back(cb);
		}
		void AddKeyboardEventCB(KEYBOARD_EVENT_CB cb) {
			m_keyboardCB.push_back(cb);
		}
		void LoseFocus();
		void GetFocus();

		inline uint16 GetWidth() { return m_width; }
		inline uint16 GetHeight() { return m_height; }

		void Quit();

	protected:
		Soft3dPipeline();
		Soft3dPipeline(Soft3dPipeline&) {};
		static std::shared_ptr<Soft3dPipeline> s_instance;

	private:
		std::vector<std::shared_ptr<VertexBufferObject> > m_vboVector;
		uint32 m_curVBO;
		std::shared_ptr<Texture> m_tex;
		std::shared_ptr<RasterizerManager> m_rasterizerManager;
		std::shared_ptr<Rasterizer> m_rasterizer;
		std::vector<std::shared_ptr<Rasterizer>> m_rasterizers;
		std::vector<std::shared_ptr<PipeLineData> > m_pipeDataVector;
		std::vector<UniformStack> m_UniformVector;

		uint16 m_width;
		uint16 m_height;

		LPDIRECTINPUT8 m_pDirectInput;
		LPDIRECTINPUTDEVICE8 m_pMouseDevice;
		LPDIRECTINPUTDEVICE8 m_pKeyboardDevice;

		std::vector<MOUSE_EVENT_CB> m_mouseCB;
		std::vector<KEYBOARD_EVENT_CB> m_keyboardCB;

		enum THREAD_MODE
		{
			THREAD_ONE,
			THREAD_MULTI_RASTERIZER,
			THREAD_MULTI_FRAGMENT,
		};

		bool m_haveFocus;
		bool m_valid = false;
		THREAD_MODE m_threadMode = THREAD_MULTI_RASTERIZER;
		int THREAD_COUNT = 7;
	};

	template<typename T>
	void SetUniform(uint16 index, const T& val)
	{
		T* p = new T(val);
		Soft3dPipeline::Instance()->SetUniform(index, (void*)p);
	}

}