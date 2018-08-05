#include "stdafx.h"
#include "Dragonfly.h"

using namespace DirectX;

Dragonfly::Dragonfly() : m_head(nullptr), m_thorax(nullptr), m_abdomen(nullptr), m_tail(nullptr), m_legs(nullptr), m_wings(nullptr), m_posX(0.0f), m_posY(1.0f), m_posZ(0.0f)
{
}

Dragonfly::Dragonfly(ID3D11DeviceContext* context)
{
	m_head = GeometricPrimitive::CreateSphere(context, 0.8f);
	
	m_thorax = GeometricPrimitive::CreateSphere(context, 1.f);
	
	m_abdomen = GeometricPrimitive::CreateCone(context, 0.8f);
	
	m_tail = GeometricPrimitive::CreateCylinder(context, 2.f, 0.2f);
	
	m_legs = GeometricPrimitive::CreateCylinder(context, 1.f, 0.05f);
	
	m_wings = GeometricPrimitive::CreateBox(context, XMFLOAT3(0.3f, 0.01f, 2.5f));

	Update(XMMatrixTranslation(m_posX, m_posY, m_posZ));
}


Dragonfly::~Dragonfly()
{
}

XMFLOAT3 Dragonfly::GetPos() const
{
	return XMFLOAT3(m_posX, m_posY, m_posZ);
}

void Dragonfly::Update(const XMMATRIX& world)
{
	m_headMatrix = XMMatrixTranslation(-0.8f, 0.f, 0.f) *world;

	m_thoraxMatrix = world;

	m_abdomenMatrix = XMMatrixRotationZ(-XM_PIDIV2)*XMMatrixTranslation(0.8f, 0.f, 0.f) * world;

	m_tailMatrix = XMMatrixRotationZ(-XM_PIDIV2)*XMMatrixTranslation(1.8f, 0.f, 0.f) * world;

	m_legsMatrix.clear();
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.2f, 0.f, 0.9f) * world);
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.1f, 0.f, 0.9f) * world);
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.0f, 0.f, 0.9f) * world);
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.2f, 0.f, -0.9f) * world);
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.1f, 0.f, -0.9f) * world);
	m_legsMatrix.push_back(XMMatrixRotationX(XM_PIDIV2)*XMMatrixTranslation(0.0f, 0.f, -0.9f) * world);

	m_wingsMatrix.clear();
	m_wingsMatrix.push_back(XMMatrixRotationX(0.261799f)*XMMatrixTranslation(-0.2f, 0.3f, -1.0f) * world);
	m_wingsMatrix.push_back(XMMatrixRotationX(-0.261799f)*XMMatrixTranslation(-0.2f, 0.3f, 1.0f) * world);
	m_wingsMatrix.push_back(XMMatrixRotationX(0.349066f)*XMMatrixTranslation(0.2f, 0.3f, -1.0f) * world);
	m_wingsMatrix.push_back(XMMatrixRotationX(-0.349066f)*XMMatrixTranslation(0.2f, 0.3f, 1.0f) * world);
}

void Dragonfly::Draw(const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
{
	m_head->Draw(m_headMatrix, view, projection, Colors::CornflowerBlue);
	m_thorax->Draw(m_thoraxMatrix, view, projection);
	m_abdomen->Draw(m_abdomenMatrix, view, projection);
	m_tail->Draw(m_tailMatrix, view, projection);
	m_legs->Draw(m_legsMatrix[0], view, projection);
	m_legs->Draw(m_legsMatrix[1], view, projection);
	m_legs->Draw(m_legsMatrix[2], view, projection);
	m_legs->Draw(m_legsMatrix[3], view, projection);
	m_legs->Draw(m_legsMatrix[4], view, projection);
	m_legs->Draw(m_legsMatrix[5], view, projection);
	m_wings->Draw(m_wingsMatrix[0], view, projection);
	m_wings->Draw(m_wingsMatrix[1], view, projection);
	m_wings->Draw(m_wingsMatrix[2], view, projection);
	m_wings->Draw(m_wingsMatrix[3], view, projection);
}