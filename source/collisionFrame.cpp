#include "collisionFrame.hpp"

#include <glm/geometric.hpp>
#include <algorithm>

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

std::pair<glm::vec3, glm::vec3> collisionFrame::Collide(	
	glm::vec3 P, glm::vec3 prevP, glm::vec3 V, float mu)
{	
	// DODAĆ SPRAWDZENIE GŁĘBOKOŚCI REKURENCJI
	if (IsInsideFrame(P)) {
		return {P, V};
	}

	std::optional<float> t_xp = PlaneSegmentIntersection(prevP, P, glm::vec3(+m_a2, 0.0f, 0.0f), glm::vec3(+1.0f, 0.0f, 0.0f));
	std::optional<float> t_xn = PlaneSegmentIntersection(prevP, P, glm::vec3(-m_a2, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	std::optional<float> t_yp = PlaneSegmentIntersection(prevP, P, glm::vec3(0.0f, +m_a2, 0.0f), glm::vec3(0.0f, +1.0f, 0.0f));
	std::optional<float> t_yn = PlaneSegmentIntersection(prevP, P, glm::vec3(0.0f, -m_a2, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	std::optional<float> t_zp = PlaneSegmentIntersection(prevP, P, glm::vec3(0.0f, 0.0f, +m_a2), glm::vec3(0.0f, 0.0f, +1.0f));
	std::optional<float> t_zn = PlaneSegmentIntersection(prevP, P, glm::vec3(0.0f, 0.0f, -m_a2), glm::vec3(0.0f, 0.0f, -1.0f));

	std::array<float, 6> T = {
		t_xp.value_or(std::numeric_limits<float>::max()), 
		t_xn.value_or(std::numeric_limits<float>::max()), 
		t_yp.value_or(std::numeric_limits<float>::max()), 
		t_yn.value_or(std::numeric_limits<float>::max()), 
		t_zp.value_or(std::numeric_limits<float>::max()), 
		t_zn.value_or(std::numeric_limits<float>::max())
	};

	auto it = std::min_element(T.begin(), T.end(), 
		[](const float& t1, const float& t2) { return t1 > 0 && (t2 <= 0 || t1 < t2); });

	int index = (it - T.begin());
	float t = *it;
	glm::vec3 X = prevP + t * (P - prevP);
	glm::vec3 PX = P - X;

	switch (index)
	{
	case 0:	/* t_xp */
	case 1: /* t_xn */
	{
		PX.x = -PX.x;
		V.x = -V.x;
		V *= mu;
	} break;
	
	case 2: /* t_yp */
	case 3: /* t_yn */
	{
		PX.y = -PX.y;
		V.y = -V.y;
		V *= mu;
	} break;

	case 4: /* t_zp */
	case 5: /* t_zn */
	{
		PX.z = -PX.z;
		V.z = -V.z;
		V *= mu;
	} break;

	}
	
	glm::vec3 newP = X + mu * PX;
	glm::vec3 newPrevP = X; // DO POPRAWY !!!!!!!!!!!!!!!

	return Collide(newP, newPrevP, V, mu);
}

std::optional<float> collisionFrame::PlaneSegmentIntersection(
	const glm::vec3& A,
	const glm::vec3& B,
	const glm::vec3& P0,
	const glm::vec3& N
)
{
	glm::vec3 L = (B - A);
	float NL = glm::dot(N, L);

	if (glm::abs(NL) < 1e-8f) { 
		return std::nullopt; 
	}

	return glm::dot(P0 - A, N) / NL;	
}

bool collisionFrame::IsInsideFrame(const glm::vec3& P)
{
	return (P.x >= -m_a2 && P.x <= m_a2) && (P.y >= -m_a2 && P.y <= m_a2) && (P.z >= -m_a2 && P.z <= m_a2); 
}