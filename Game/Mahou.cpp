#include "stdafx.h"
#include "Mahou.h"
#include "Game.h"
#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Mahou::Mahou()
{

}

Mahou::~Mahou()
{
	//�G�t�F�N�g�̍Đ����~����B
	m_effectEmitter->Stop();
	//�F�X�폜�B
	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool Mahou::Start()
{

	switch (m_zokusei) {

	case enZokusei_hi:
		//�G�t�F�N�g�̃I�u�W�F�N�g���쐬����B
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enMagic_fire);
		break;
	case enZokusei_kaze:
		//�G�t�F�N�g�̃I�u�W�F�N�g���쐬����B
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enDaburuinnpakuto);
		break;
	case enZokusei_mizu:
		//�G�t�F�N�g�̃I�u�W�F�N�g���쐬����B
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enWater);
		break;
	}

	//�G�t�F�N�g�̑傫����ݒ肷��B
	m_effectEmitter->SetScale(m_scale * 15.0f);
	//�ړ����x���v�Z�B
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_position += m_moveSpeed * 50.0f;
	m_moveSpeed *= 600.0f;
	m_rotation.AddRotationDegY(360.0f);
	//��]��ݒ肷��B
	m_effectEmitter->SetRotation(m_rotation);
	m_effectEmitter->Play();

	//�R���W�����I�u�W�F�N�g���쐬����B
	m_collisionObject = NewGO<CollisionObject>(0);
	//����̃R���W�������쐬����B
	m_collisionObject->CreateSphere(m_position, Quaternion::Identity, 35.0f * m_scale.z);
	//�p�҂��v���C���[��������B
	if (m_enMagician == enMagician_Player)
	{
		//���O��player_fireball�ɂ���B
		switch (m_zokusei) {
		case enZokusei_hi:
			m_collisionObject->SetName("player_fireball");
			break;
		case enZokusei_kaze:
			m_collisionObject->SetName("player_kaze");
			break;
		case enZokusei_mizu:
			m_collisionObject->SetName("player_mizu");
			break;
		}
		
	}
	//�p�҂�Boss��������B
	else if (m_enMagician == enMagician_Boss)
	{
		//���O��enemy_fireball�ɂ���B
		m_collisionObject->SetName("enemy_fireball");
	}

	//���菊�I�u�W�F�N�g�������ō폜����Ȃ��悤�ɂ���B
	//m_collisionObject->SetIsEnableAutoDelete(false);
	m_collisionObject->SetTimeLimit(1.0f);
	//����ǂݍ��ށB
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/fire_cut_cut.wav");
	//���ʉ����Đ�����B
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(1);
	se->Play(false);
	se->SetVolume(1.5f);
	return true;
}

void Mahou::Update()
{

	switch (m_zokusei) {

	case enZokusei_hi:
		//���W���ړ�������B
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//�G�t�F�N�g�̍��W��ݒ肷��B
		m_effectEmitter->SetPosition(m_position);
		//�R���W�����I�u�W�F�N�g�ɍ��W��ݒ肷��B
		m_collisionObject->SetPosition(m_position);

		//�^�C�}�[�����Z����B
		m_timer += g_gameTime->GetFrameDeltaTime();

		//�^�C�}�[��0.7f�ȏゾ������B
		if (m_timer >= 0.7f)
		{
			//���g���폜����B
			DeleteGO(this);
		}
		break;
	case enZokusei_kaze:
		//���W���ړ�������B
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//�G�t�F�N�g�̍��W��ݒ肷��B
		m_effectEmitter->SetPosition(m_position);
		//�R���W�����I�u�W�F�N�g�ɍ��W��ݒ肷��B
		m_collisionObject->SetPosition(m_position);

		//�^�C�}�[�����Z����B
		m_timer += g_gameTime->GetFrameDeltaTime();

		//�^�C�}�[��0.7f�ȏゾ������B
		if (m_timer >= 2.5f)
		{
			//���g���폜����B
			DeleteGO(this);
		}
		break;
	case enZokusei_mizu:
		//���W���ړ�������B
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//�G�t�F�N�g�̍��W��ݒ肷��B
		m_effectEmitter->SetPosition(m_position);
		//�R���W�����I�u�W�F�N�g�ɍ��W��ݒ肷��B
		m_collisionObject->SetPosition(m_position);

		//�^�C�}�[�����Z����B
		m_timer += g_gameTime->GetFrameDeltaTime();

		//�^�C�}�[��0.7f�ȏゾ������B
		if (m_timer >=1.5f)
		{
			//���g���폜����B
			DeleteGO(this);
		}
		break;
	}

	
}