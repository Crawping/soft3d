#pragma once
#include <memory>
#include <boost/noncopyable.hpp>

namespace soft3d
{

	class SceneManager : public boost::noncopyable
	{
	public:
		static std::shared_ptr<SceneManager> Instance() {
			return s_instance;
		}
		virtual ~SceneManager();
		virtual void InitScene(uint16 width, uint16 height);
		virtual void Update();

	protected:
		SceneManager();
		static std::shared_ptr<SceneManager> s_instance;

		uint16 m_width;
		uint16 m_height;
	};

}