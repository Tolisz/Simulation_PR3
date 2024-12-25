#pragma once 

#include <array>
#include <glm/vec3.hpp>
#include <optional>

class collisionFrame
{
public:

	collisionFrame(float a);
	~collisionFrame() = default;

	collisionFrame(const collisionFrame&) = delete;
	collisionFrame(collisionFrame&&) = delete;
	collisionFrame& operator=(const collisionFrame&) = delete;
	collisionFrame& operator=(collisionFrame&&) = delete;

	float GetEdgeLength();
	std::pair<glm::vec3, glm::vec3> Collide(
		glm::vec3 P,
		glm::vec3 prevP,
		glm::vec3 V, 
		float mu,
		unsigned int maxInter);

private: 

	std::pair<glm::vec3, glm::vec3> CollideInternal(
		glm::vec3 P,
		glm::vec3 prevP,
		glm::vec3 V, 
		float mu,
		unsigned int iter,
		const unsigned int maxInter);

	std::optional<float> PlaneSegmentIntersection(
		const glm::vec3& A,
		const glm::vec3& B,
		const glm::vec3& P0,
		const glm::vec3& N
	);

	bool IsInsideFrame(const glm::vec3& P);

	void Reset(float a);

private:

	float m_a; // edge size;
	float m_a2; // half of edge size;

};