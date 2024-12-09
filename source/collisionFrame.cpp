#include "collisionFrame.hpp"

collisionFrame::collisionFrame(float a)
{
	Reset(a);
}

void collisionFrame::Reset(float a)
{
	m_a = a;
	m_a2 = a / 2;
}

float collisionFrame::GetEdgeLength()
{
	return m_a;
}

glm::vec3 collisionFrame::Collide(glm::vec3 P, float mu)
{
	return glm::vec3(0.0f);
}