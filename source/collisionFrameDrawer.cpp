#include "collisionFrameDrawer.hpp"

collisionFrameDrawer::collisionFrameDrawer(std::shared_ptr<collisionFrame> frame) :
	m_frame{frame}
{
}

float collisionFrameDrawer::GetEdgeLength()
{
	return m_frame->GetEdgeLength();
}
