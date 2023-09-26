
#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>
using namespace CK::DD;

// ���ڸ� �׸��� �Լ�
void SoftRenderer::DrawGizmo2D()
{
	auto& r = GetRenderer();
	const auto& g = Get2DGameEngine();

	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���
	Vector2 viewPos = g.GetMainCamera().GetTransform().GetPosition();
	Vector2 extent = Vector2(_ScreenSize.X * 0.5f, _ScreenSize.Y * 0.5f);

	// ���� �ϴܿ������� ���� �׸���
	int xGridCount = _ScreenSize.X / _Grid2DUnit;
	int yGridCount = _ScreenSize.Y / _Grid2DUnit;

	// �׸��尡 ���۵Ǵ� ���ϴ� ��ǥ �� ���
	Vector2 minPos = viewPos - extent;
	Vector2 minGridPos = Vector2(ceilf(minPos.X / (float)_Grid2DUnit), ceilf(minPos.Y / (float)_Grid2DUnit)) * (float)_Grid2DUnit;
	ScreenPoint gridBottomLeft = ScreenPoint::ToScreenCoordinate(_ScreenSize, minGridPos - viewPos);

	for (int ix = 0; ix < xGridCount; ++ix)
	{
		r.DrawFullVerticalLine(gridBottomLeft.X + ix * _Grid2DUnit, gridColor);
	}

	for (int iy = 0; iy < yGridCount; ++iy)
	{
		r.DrawFullHorizontalLine(gridBottomLeft.Y - iy * _Grid2DUnit, gridColor);
	}

	ScreenPoint worldOrigin = ScreenPoint::ToScreenCoordinate(_ScreenSize, -viewPos);
	r.DrawFullHorizontalLine(worldOrigin.Y, LinearColor::Red);
	r.DrawFullVerticalLine(worldOrigin.X, LinearColor::Green);
}

// ���� ������Ʈ ���


// ���� �� �ε��� ����ϴ� �Լ�
void SoftRenderer::LoadScene2D()
{
	// ���� �� �ε����� ����ϴ� ��� �� �ֿ� ���۷���
	auto& g = Get2DGameEngine();

}

// [�Լ��� ���� ������ ���� ������ Ȱ��]
// 1. Update2D -> ���� ����: �������� �����ϴ� ��ü�� ���� ��ġ�� ����ϰ� �̸� ������ ����
// 2. ���� ���� -> Render2D: ������ ����� ��ü�� ��ġ�� �а�, �̸� �߽����� ��ü�� ȭ�鿡 �׷���

// ���� ������ ������ ������ �����ϴ� ����
// ���� ����: Update2D�� Render2D �Լ����� �Բ� ������ ������ �����Ѵ�.
// ������ ��ǥ�� ������ ������ currentPosition���� �����Ѵ�.
// ��ü�� ��ġ�� �����ϴ� ���� currentPosition�� �����ϰ� �⺻ ����(100, 100)���� �����Ѵ�.
Vector2 currentPosition;
// ���� ũ�� ���� �����ϴ� ���� currentScale�� �����ϰ� �⺻ ���� 10���� �����Ѵ�.
float currentScale = 10.f;

// ���� ������ ����ϴ� �Լ�
// Update2D �Լ� ����: �Է¿� ���� currentPosition ���� ���� ��ȭ��Ų��.
// �� ������ ����� �Է¿� ���� ��ü�� �̵���Ű�� ���� ������ �ش��Ѵ�.
// ���� ������ ����ϴ� Update2D �Լ��� �����Ѵ�.
// ���� ������ �ַ� ����� �Է¿� ���� ó���� �ٷ��.
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ���� �������� ����ϴ� ��� �� �ֿ� ���۷���
	// ���� ���� ��⿡ �����Ѵ�.
	auto& g = Get2DGameEngine();
	// ���� ���� ��⿡�� ����� �Է°��� �о���� �� �ֵ��� �Է� ������ InputManager�� ���۷����� �б� �������� �����´�.
	const InputManager& input = g.GetInputManager();

	// ���� ������ ���� ����
	// �����Ӵ� �̵� �ӵ��� �����Ѵ�.
	static float moveSpeed = 100.f;
	// �ּ� ũ�� ���� �����ϴ� ���� scaleMin�� �����Ѵ�.
	static float scaleMin = 5.f;
	// �ִ� ũ�� ���� �����ϴ� ���� scaleMax�� �����Ѵ�.
	static float scaleMax = 20.f;
	// �Է¿� ���� ũ�� ��ȭ �ӵ��� �����ϴ� ���� scaleSpeed�� �����Ѵ�.
	static float scaleSpeed = 20.f;

	// X��� Y�� �Է��� ������ �Է� ���͸� �����Ѵ�.
	// GetNormalize() �Լ��� ����� �Է� ������ ũ�⸦ �׻� 1�� ����ȭ�Ѵ�.
	Vector2 inputVector = Vector2(input.GetAxis(InputAxis::XAxis), input.GetAxis(InputAxis::YAxis)).GetNormalize();
	// �̵� �ӵ��� �������� ��� �ð� InDeltaTime�� ���Ͽ� �ش� �����ӿ��� �̵��� ���̸� ����� �� �̸� �Է� ���Ϳ� ���ϰ� �� ����� ���� deltaPosition�� �����Ѵ�.
	Vector2 deltaPosition = inputVector * moveSpeed * InDeltaSeconds;
	// ���� �����ӿ��� Z�� �Է¿� ���� ũ���� ��ȭ���� ����Ѵ�.
	float deltaScale = input.GetAxis(InputAxis::ZAxis) * scaleSpeed * InDeltaSeconds;

	// ��ü�� ���� ���� ����
	// deltaPosition ���� �ݿ��� ���� �������� currentPosition ���� Ȯ���Ѵ�.
	currentPosition += deltaPosition;
	// ���� ũ�� ���� ��ȭ���� �ݿ��ϵ� Clamp �Լ��� ����� ������ �ִ񰪰� �ּڰ� ���̸� ���� ���ϵ��� �̴�.
	currentScale = Math::Clamp(currentScale + deltaScale, scaleMin, scaleMax);
}

// ������ ������ ����ϴ� �Լ�
// Render2D �Լ� ����: �Է��� �ݿ��� ���� ��ǥ�� ������ currentPosition ���� ���� �о� �ش� ��ǥ�� ������ �������� ���� ��� ���� �׸���.
// �� ������ ȭ�鿡 �׸��� ����� �����ϴ� ������ ������ �ش��Ѵ�.
// ������ ������ ����ϴ� Render2D �Լ��� �����Ѵ�.
// ������ ������ ���� �������� ����� �����͸� �ݿ��� ���� ����� ����� ȭ�鿡 �����ִ� ����� �����Ѵ�.
void SoftRenderer::Render2D()
{
	// ������ �������� ����ϴ� ��� �� �ֿ� ���۷���
	// ȭ���� �ð��� ǥ���� ���� ������ ����� ���۷����� �����´�.
	auto& r = GetRenderer();
	// ���� ���� ����� ���۷����� �б� �������� �����´�.
	const auto& g = Get2DGameEngine();

	// ��濡 ���� �׸���
	DrawGizmo2D();

	// ������ ������ ���� ����
	// ���� ���� �����ϴ� ���� rad�� �����Ѵ�.
	float rad = 0.f;
	// ���� ������ ������Ű���� ���а� increment�� �����Ѵ�.
	static float increment = 0.001f;
	// ��Ʈ�� �����ϴ� ���� ��Ƶ� �����̳� hearts�� �����Ѵ�.
	static std::vector<Vector2> hearts;

	// ��Ʈ�� �����ϴ� �� ����
	if (hearts.empty())
	{
		// ���� 0���� 2pi���� ������ ������Ű�� ������ �����Ѵ�.
		for (rad = 0.f; rad < Math::TwoPI; rad += increment)
		{
			// ��Ʈ �������� ����� �ش� ���� x�� y���� ����Ѵ�.
			float sin = sinf(rad);
			float cos = cosf(rad);
			float cos2 = cosf(2 * rad);
			float cos3 = cosf(3 * rad);
			float cos4 = cosf(4 * rad);
			float x = 16.f * sin * sin * sin;
			float y = 13 * cos - 5 * cos2 - 2 * cos3 - cos4;
			// x�� y������ ���͸� ���� �� �̸� hearts�� �߰��Ѵ�.
			hearts.push_back(Vector2(x, y));
		}
	}

	for (auto const& v : hearts)
	{
		// hearts�� ���� ��� ���� ���� ��ġ�� ũ�� ���� �ݿ��� �� �Ķ������� ���� ��´�.
		r.DrawPoint(v * currentScale + currentPosition, LinearColor::Blue);
	}

	// ���� ��ġ�� �������� ȭ�鿡 ���
	r.PushStatisticText(std::string("Position : ") + currentPosition.ToString());
	r.PushStatisticText(std::string("Scale : ") + std::to_string(currentScale));

	// 3-2
	//// �׸� ���� �������� 50���� �����Ѵ�.
	//static float radius = 50.f;
	//// ���� �����ϴ� ���� �����ϱ� ���� �ڷᱸ���� vector�� �����ϰ� �̸� circles�� �����Ѵ�.
	//// �̶� �ѹ� ������ ������ ��� ������ �� �ֵ��� ������ static���� �����Ѵ�.
	//static std::vector<Vector2> circles;

	//// ���ʿ� �ѹ� ���������� �� ���͸� ��� �����̳ʿ� ��´�.
	//// circles �ڷᱸ���� ��� �ִ� ��쿡�� ���� �����ϴ� ���� ä�� �ֵ��� ������ �����Ѵ�.
	//if (circles.empty()) {
	//	// ������ ������ Ȱ���� ���� ���Ե� �簢�� ������ ����ϰ� �� �ȿ� ���� ���͸� �����ϱ� ���� ũ�� 1������ ������ ����.
	//	for (float x = -radius; x <= radius; ++x) {
	//		for (float y = -radius; y <= radius; ++y)
	//		{
	//			// x, y���� ������ ���͸� �����Ѵ�.
	//			Vector2 pointToTest = Vector2(x, y);
	//			// ������ ũ�⸦ ���� �� �������� ������ ���� �� x^2 + y^2���� ����Ѵ�.
	//			float squardLength = pointToTest.SizeSquared();
	//			// squardLength ���� ũ�⸦ ���ϱ� ���� ������ ���� �� �� ���Ѵ�.
	//			// �̷��� ũ���� �������� �������� �������� ���ϴ� ����� �������� ������� �ʱ� ������ �� ������ ����� ����.
	//			if (squardLength <= radius * radius)
	//			{
	//				// �� ���ǹ��� ����� ���̸� ���� �����ϴ� �ڷᱸ�� circles�� �ش� ���͸� �߰��Ѵ�.
	//				circles.push_back(Vector2(x, y));
	//			}
	//		}
	//	}
	//}

	//// ���� �����ϴ� ���͸� ��� ���������� ǥ���Ѵ�.
	//// ���� �����ϴ� �ڷᱸ�� circles�� ���� ��� ���� ��ȸ�Ѵ�.
	//for (auto const& v : circles) {
	//	// �� ���� ��ġ�� ���� ���� ���� ���ϰ� ���������� ��´�.
	//	r.DrawPoint(v + currentPosition, LinearColor::Red);
	//}

	// 3-1
	//// ���� ȸ���� ���� ����� ������ ���͸� ǥ��
	//// ���� currentPosition�� ������ ���͸� ǥ���ϵ��� ���̸� ����� ũ�� �����Ѵ�.
	//static float lineLength = 500.f;
	//// ���� currentPosition�� ������ ���� �������� ������ ����Ѵ�.
	//Vector2 lineStart = currentPosition * lineLength;
	//Vector2 lineEnd = currentPosition * -lineLength;
	//// ���������� �����ϴ� DrawLine �Լ��� ����� ���� ȸ������ ������ ���� ǥ���Ѵ�.
	//r.DrawLine(lineStart, lineEnd, LinearColor::LightGray);

	//// ���͸� �Ķ��� �ȼ��� ǥ��
	//// �Ķ��� ������ ���� currentPosition�� ȭ�鿡 ��´�.
	//// ���� �� ���̵��� �ش� ���� �ѷ� 8�� �ȼ��� �Բ� ��´�.
	//r.DrawPoint(currentPosition, LinearColor::Blue);
	//r.DrawPoint(currentPosition + Vector2::UnitX, LinearColor::Blue);
	//r.DrawPoint(currentPosition - Vector2::UnitX, LinearColor::Blue);
	//r.DrawPoint(currentPosition + Vector2::UnitY, LinearColor::Blue);
	//r.DrawPoint(currentPosition - Vector2::UnitY, LinearColor::Blue);
	//r.DrawPoint(currentPosition + Vector2::One, LinearColor::Blue);
	//r.DrawPoint(currentPosition - Vector2::One, LinearColor::Blue);
	//r.DrawPoint(currentPosition + Vector2(1.f, -1.f), LinearColor::Blue);
	//r.DrawPoint(currentPosition - Vector2(1.f, -1.f), LinearColor::Blue);

	//// ������ ���� ��ǥ�� ���ܿ� ���
	//// ���� currentPosition�� ��ǥ�� ȭ�� ���ܿ� ����Ѵ�.
	//r.PushStatisticText("Coordinate : " + currentPosition.ToString());
}

// �޽ø� �׸��� �Լ�
void SoftRenderer::DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor)
{
}

// �ﰢ���� �׸��� �Լ�
void SoftRenderer::DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
}
