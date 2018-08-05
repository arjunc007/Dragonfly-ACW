#include "ModelCollection.h"
#include "Model.h"
#include "Configuration.h"

using namespace DirectX;

ModelCollection::ModelCollection()
{
}


ModelCollection::~ModelCollection()
{
}

void ModelCollection::Scale(const float x, const float y, const float z)
{
	for (const auto& part : m_parts)
	{
		part->Scale(x, y, z);
	}
}

void ModelCollection::Rotate(const float x, const float y, const float z)
{
	const XMVECTOR mPos = XMLoadFloat3(&GetPosition());
	for (const auto& part : m_parts)
	{
		part->Rotate(x, y, z);
		XMVECTOR pPos = XMLoadFloat3(&part->GetPosition());
		//Vector pointing to part from center
		const XMVECTOR diff = pPos - mPos;
		//Rotate vector by same amount
		pPos = XMVector3Transform(diff, XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
		//Move position by distance bween new and old
		pPos = pPos - diff;
		part->Move(XMVectorGetX(pPos), XMVectorGetY(pPos), XMVectorGetZ(pPos));
	}

}

void ModelCollection::Move(const float x, const float y, const float z)
{
	const XMFLOAT3 newPos(x, y, z);
	const XMVECTOR diff = XMLoadFloat3(&newPos) - XMLoadFloat3(&GetPosition());

	//change component position
	for (const auto& part : m_parts)
	{
		part->Move(XMVectorGetX(diff), XMVectorGetY(diff), XMVectorGetZ(diff));
	}
	
	//Change own position
	SetPosition(newPos);
}

void ModelCollection::StartAnimation()
{
	SetAnimating(true);
	SetAnimationTime(0.0f);
}

void ModelCollection::Update(const float dt)
{
	if (IsAnimating())
	{
		SetAnimationTime(GetAnimationTime() + dt);
		XMFLOAT3 pos = GetPosition();
		XMFLOAT4 rot = GetRotation();
		SetAnimating( GetAnimation()->Run(pos, rot, GetAnimationTime()) );

		//XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&rot), XMQuaternionInverse(XMLoadFloat4(&m_rotation))));

		Move(pos.x, pos.y, pos.z);

		//Convert Quaternion to rollpitchyaw before 
		//Rotate(rot.x, rot.y, rot.z, rot.w);
		//XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation));
		//XMStoreFloat3(&m_pos, XMLoadFloat3(&m_pos) + XMLoadFloat3(&newpos));
	}

}


void ModelCollection::Render(ID3D11DeviceContext* context)
{
	for(const auto& part : m_parts)
	{
		part->Render(context);
	}
}

void ModelCollection::CreateDragonfly(ID3D11Device* device, const float x, const float y, const float z)
{
	SetRadius(3.5f);
	SetPosition(XMFLOAT3(x, y, z));
	SetRotation(XMFLOAT4(dragonfly_initialRot.x, dragonfly_initialRot.y, dragonfly_initialRot.z, dragonfly_initialRot.w));

	//torso
	m_parts.push_back(std::make_unique<Model>());

	m_parts.back()->Initialize(device, cubeFile, dragonflySkin);
	m_parts.back()->Scale(1.0f, 0.7f, 0.7f);
	m_parts.back()->Move(x, y, z);

	//head
	m_parts.push_back(std::make_unique<Model>());

	m_parts.back()->Initialize(device, cubeFile, dragonflySkin);
	m_parts.back()->Scale(.9f, 0.9f, 0.9f);
	m_parts.back()->Move(x -1.8f, y, z);

	//eyes
	m_parts.push_back(std::make_unique<Model>());

	m_parts.back()->Initialize(device, cubeFile, dragonflyEye);
	m_parts.back()->Scale(.4f, .8f, .43f);
	m_parts.back()->Move(x - 2.4f, y, z - 0.5f);

	m_parts.push_back(std::make_unique<Model>());

	m_parts.back()->Initialize(device, cubeFile, dragonflyEye);
	m_parts.back()->Scale(.4f, .8f, .43f);
	m_parts.back()->Move(x - 2.4f, y, z + 0.5f);

	//tail
	m_parts.push_back(std::make_unique<Model>());

	m_parts.back()->Initialize(device, cubeFile, dragonflySkin);
	m_parts.back()->Scale(2.5f, 0.2f, 0.2f);
	m_parts.back()->Move(x + 2.0f, y, z);

	//legs
	for (int i = 0; i < 6; i++)
	{
		//upper
		m_parts.push_back(std::make_unique<Model>());

		m_parts.back()->Initialize(device, cubeFile, dragonflySkin);
		m_parts.back()->Scale(0.02f, 0.02f, 1.0f);
		m_parts.back()->Move(x + (i % 3 - 1)*0.2f, y, z + (i < 3 ? -1.0f : 1.0f));

		//lower
		m_parts.push_back(std::make_unique<Model>());

		m_parts.back()->Initialize(device, cubeFile, dragonflySkin);
		m_parts.back()->Scale(0.02f, 0.45f, 0.02f);
		m_parts.back()->Move(x + (i % 3 - 1)*0.2f, y - 0.45f, z + (i < 3 ? -1.98f : 1.98f));
	}

	//wings
	for (int i = 0; i < 4; i++)
	{
		m_parts.push_back(std::unique_ptr<Model>(new Model));

		m_parts.back()->Initialize(device, cubeFile, dragonflyWings);
		m_parts.back()->Scale(0.3f, 0.001f, 2.0f);
		m_parts.back()->Rotate(i < 2 ? 15.0f : -15.0f, 0.0f, 0.0f);
		m_parts.back()->Move(x + i % 2 - 0.5f, y + 1.1f, z + (i < 2 ? -1.8f : 1.8f));
		/*animation = std::make_unique<Animator>();
		if (i > 1)
			animation->LoadPath("LeftWings");
		else
			animation->LoadPath("RightWings");*/
	}

	//Add animations
	//Flight
	SetAnimation(new Animator);
	GetAnimation()->LoadPath("Flight");
}

void ModelCollection::Reset()
{
	SetPosition(dragonfly_initialPos);
	SetRotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	
	SetAnimating(false);
	
	m_parts.clear();
	GetAnimation()->Reset();
	SetAnimationTime(0.0f);
}

void ModelCollection::Shutdown()
{
	for (const auto& part : m_parts)
		part->Shutdown();
}