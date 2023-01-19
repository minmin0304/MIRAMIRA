#include "stdafx.h"
#include "Background.h"

Background::Background()
{

}

Background::~Background()
{

}

bool Background::Start()
{
	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/map.tkm");
	//���W��ݒ肷��B
	m_modelRender.SetPosition(m_position);
	//�傫����ݒ肷��B
	m_modelRender.SetScale(m_scale);
	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);
	//���f�����X�V����B
	m_modelRender.Update();


	//�����蔻����쐬����B
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	return true;

}

void Background::Update()
{
	//���f���̍X�V�����B
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
}

void Background::Render(RenderContext& rc)
{
	//���f����`�悷��B
	m_modelRender.Draw(rc);
}