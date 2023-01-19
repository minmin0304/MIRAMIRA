#include "stdafx.h"
#include "Boss.h"

#include "Game.h"
#include "Mahou.h"
#include "Player.h"


#include <time.h>
#include <stdlib.h>

namespace
{
	const int MAXIMUM_HP = 1;
}

Boss::Boss()
{

}

Boss::~Boss()
{

}

bool Boss::Start()
{
	m_spriteRenderkaze.Init("Assets/sprite/kaze.dds", 345, 345);
	m_spriteRendermizu.Init("Assets/sprite/mizu.dds", 345, 345);
	m_spriteRenderfire.Init("Assets/sprite/fire.dds", 345, 345);
	m_spriteRendersankakukankei.Init("Assets/sprite/sankakukankei.dds", 256, 256);
	m_bosshpbar.Init("Assets/sprite/bosshpbar.dds", 1024, 72);
	m_bosshpbapa.Init("Assets/sprite/hpbapa2.dds", 1050, 76);
	m_bosshpbahaikei.Init("Assets/sprite/hpbahaikei.dds", 1050, 76);

	//�s�{�b�g��ݒ肷��B
	//�s�{�b�g�͉摜�̒��S�ł���B
	//�s�{�b�g�𒆐S�ɁA�摜���L�яk�݂���̂ŁB
	//�s�{�b�g�̒l��ύX���邱�Ƃɂ��B
	//�摜�̑傫����ύX���Ă��A�摜���ړ����Ȃ��悤�ɂ���B
	m_bosshpbar.SetPivot(Vector2(0.0f, 0.5f));
	m_bosshpbapa.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbahaikei.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbar.SetPosition(Vector3(-2000.0f,502.0f, 0.0f));
	m_spriteRenderkaze.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRendermizu.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRenderfire.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRendersankakukankei.SetPosition(Vector3(507.0f, 405.0f, 0.0f));

	//�A�j���[�V������ǂݍ��ށB
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/boss/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/boss/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/boss/attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Mahou].Load("Assets/animData/boss/mahou.tka");
	m_animationClips[enAnimationClip_Mahou].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/boss/receivedamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/boss/death.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/boss/boss.tkm", m_animationClips, enAnimationClip_Num);
	//���W��ݒ肷��B

	m_modelRender.SetPosition(m_position);
	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);
	//�傫����ݒ肷��B
	m_modelRender.SetScale(m_scale);

	// �i�r���b�V�����\�z�B
	m_nvmMesh.Init("Assets/nv/stage.tkn");

	//�L�����N�^�[�R���g���[���[���������B
	m_charaCon.Init(
		20.0f,			//���a�B
		50.0f,			//�����B
		m_position		//���W�B
	);

	//���̃{�[����ID���擾����B
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//�A�j���[�V�����C�x���g�p�̊֐���ݒ肷��B
	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});

	//����ǂݍ��ށB
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/magic.wav");
	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/slash.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/hit.wav");

	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	//�������������B
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	//�X�V�����B
	m_bosshpbar.Update();
	m_hp = MAXIMUM_HP;


	return true;
}

void Boss::Update()
{
	
	//�ǐՏ����B
	Chase();
	//��]�����B
	Rotation();
	//�����蔻��B
	Collision();
	//�U�������B
	Attack();
	//�A�j���[�V�����̍Đ��B
	PlayAnimation();
	//�X�e�[�g�̑J�ڏ����B
	ManageState();

	//���f���̍X�V�B
	m_modelRender.Update();
	m_spriteRenderkaze.Update();
	m_spriteRendermizu.Update();
	m_spriteRenderfire.Update();
	m_spriteRendersankakukankei.Update();

	Vector3 move = Vector3::Zero;
	m_charaCon.Execute(move, 0);
	m_modelRender.Update();

	Vector3 scale = Vector3::One;
	//��HP/�ő�HP��HP�o�[�̃X�P�[���ɂ���B
	//int�^���m�̌v�Z���ƁA�����_�ȉ��؂�̂ĂɂȂ�̂ŁB
	//float�^�ɕϊ����Čv�Z���s���B
	scale.x = float(m_hp) / float(MAXIMUM_HP);
	//�X�P�[����ݒ�B
	m_bosshpbapa.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbapa.Update();
	m_bosshpbahaikei.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbahaikei.Update();
	m_bosshpbar.SetScale(scale);
	m_bosshpbar.SetPosition(Vector3(-944.0f, 502.0f, 0.0f));
	//�X�V�����B
	m_bosshpbar.Update();
}

void Boss::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		//m_moveSpeed.x��m_moveSpeed.z�̐�Βl���Ƃ���0.001�ȉ��Ƃ������Ƃ�
		//���̃t���[���ł̓L�����͈ړ����Ă��Ȃ��̂Ő��񂷂�K�v�͂Ȃ��B
		return;
	}
	//atan2��tan�Ƃ̒l���p�x(���W�A���P��)�ɕϊ����Ă����֐��B
	//m_moveSpeed.x / m_moveSpeed.z�̌��ʂ�tan�ƂɂȂ�B
	//atan2���g�p���āA�p�x�����߂Ă���B
	//���ꂪ��]�p�x�ɂȂ�B
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	//atan���Ԃ��Ă���p�x�̓��W�A���P�ʂȂ̂�
	//SetRotationDeg�ł͂Ȃ�SetRotation���g�p����B
	m_rotation.SetRotationY(-angle);

	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);

	//�v���C���[�̑O�x�N�g�����v�Z����B
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Boss::Chase()
{
	//�ǐՃX�e�[�g�łȂ��Ȃ�A�ǐՏ����͂��Ȃ��B
	if (m_bossState != enBossState_Chase)
	{
		return;
	}

	m_targetPointPosition = m_player->GetPosition();
	bool isEnd;
	//if(){
		// �p�X����
	m_pathFiding.Execute(
		m_path,							// �\�z���ꂽ�p�X�̊i�[��
		m_nvmMesh,						// �i�r���b�V��
		m_position,						// �J�n���W
		m_targetPointPosition,			// �ړ��ڕW���W
		PhysicsWorld::GetInstance(),	// �����G���W��	
		20.0f,							// AI�G�[�W�F���g�̔��a
		50.0f							// AI�G�[�W�F���g�̍����B
	);
	//}
	// �p�X����ړ�����B
	m_position = m_path.Move(
		m_position,
		5.0f,
		isEnd
	);
	Vector3 pos = m_position;
	m_charaCon.SetPosition(pos);
	m_modelRender.SetPosition(pos);
}

void Boss::Collision()
{
	//��_���[�W�A���邢�̓_�E���X�e�[�g�̎��́B
//�����蔻�菈���͂��Ȃ��B
	if (m_bossState == enBossState_ReceiveDamage ||
		m_bossState == enBossState_Death)
	{
		return;
	}

	{
		//�v���C���[�̍U���p�̃R���W�������擾����B
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");
		//�R���W�����̔z���for���ŉ񂷁B
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����B
			if (collision->IsHit(m_charaCon))
			{

				//HP��1���炷�B
				m_hp -= 30;
				btVector3 collisionpos = collision->GetbtCollisionObject().getWorldTransform().getOrigin();
				Vector3 Mahoupos;
				//btVector3��Vector�ɕϊ�
				Mahoupos = { collisionpos.getX(),collisionpos.getY(),collisionpos.getZ() };
				//�m�b�N�o�b�N�̃x�N�g�������߂�
				m_bossnokubaku = Mahoupos - m_position;
				//Y�����K�v���Ȃ��B
				m_bossnokubaku.y = 0.0f;
				//�x�N�g���𐳋K��
				m_bossnokubaku.Normalize();
				//���݂̍��W����m�b�N�o�b�N�����Ɍ��Z�A�m�b�N�o�b�N�͐��l��Z�B
				m_position -= m_bossnokubaku * 50;
				//�L�����R��������������B
				m_charaCon.SetPosition(m_position);
				//�L�����R�����Z�b�g�|�W�V���������ʒu�Ɉړ����邽��0��Execute�̏��������s
				Vector3 moveSpeed = Vector3::Zero;
				m_charaCon.Execute(moveSpeed, 0);
				//���f�������_�[�̍��W���X�V�B
				Vector3 modelPosition = m_position;
				//������Ƃ������f���̍��W��������B
				modelPosition.y += 2.5f;
				m_modelRender.SetPosition(modelPosition);
				m_modelRender.Update();
				//HP��0�ɂȂ�����B
				if (m_hp <=0)
				{
					//�_�E���X�e�[�g�ɑJ�ڂ���B
					m_bossState = enBossState_Death;
				}
				else {
					//��_���[�W�X�e�[�g�ɑJ�ڂ���B
					m_bossState = enBossState_ReceiveDamage;
				}
				//���ʉ����Đ�����B
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.4f);
				return;

			}
		}
	}

	{
		//�v���C���[�̃t�@�C���[�{�[���p�̃R���W�������擾����B
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_fireball");
		//for���Ŕz����񂷁B
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����B
			if (collision->IsHit(m_charaCon))
			{
				//boss���������̎����_���[�W�H������猸���
				if(m_zokusei==enZokusei_kaze){
					m_hp -= 30;
				}
				//boss���������̎����_���[�W�H������猸���
				else if(m_zokusei==enZokusei_mizu) {
					m_hp -= 5;
				}

				m_zokusei = enZokusei_hi;
				//HP��0�ɂȂ�����B
				if (m_hp <= 0)
				{
					//�_�E���X�e�[�g�ɑJ�ڂ���B
					m_bossState = enBossState_Death;
				}
				else {
					//��_���[�W�X�e�[�g�ɑJ�ڂ���B
					m_bossState = enBossState_ReceiveDamage;
				}
				//���ʉ����Đ�����B
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.6f);
				return;
			}
		}
	}
		{
			//�v���C���[�̂����p�̃R���W�������擾����B
			const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_kaze");
			//for���Ŕz����񂷁B
			for (auto collision : collisions)
			{
				//�R���W�����ƃL�����R�����Փ˂�����B
				if (collision->IsHit(m_charaCon))
				{
					//boss���������̎����_���[�W�H������猸���
					if (m_zokusei == enZokusei_hi) {
						if (m_hp < 600) {
							m_hp += 100;
						}
					}
					//boss���������̎����_���[�W�H������猸���
					else if (m_zokusei == enZokusei_mizu) {
						m_hp -= 50;
					}
					m_zokusei = enZokusei_kaze;
					//HP��0�ɂȂ�����B
					if (m_hp <= 0)
					{
						//�_�E���X�e�[�g�ɑJ�ڂ���B
						m_bossState = enBossState_Death;
					}
					else {
						//��_���[�W�X�e�[�g�ɑJ�ڂ���B
						m_bossState = enBossState_ReceiveDamage;
					}
					//���ʉ����Đ�����B
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(4);
					se->Play(false);
					se->SetVolume(0.6f);
					return;

				}
			}
		}
		{
			//�v���C���[�݂̂��p�̃R���W�������擾����B
			const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_mizu");
			//for���Ŕz����񂷁B
			for (auto collision : collisions)
			{
				//�R���W�����ƃL�����R�����Փ˂�����B
				if (collision->IsHit(m_charaCon))
				{
					//boss���������̎����_���[�W�H������猸���
					if (m_zokusei == enZokusei_hi) {
						m_hp -= 50;
					}
					//boss���������̎����_���[�W�H������猸���
					else if (m_zokusei == enZokusei_kaze) {
						m_hp -= 0;
					}
					//�������ɕϊ�����B
					m_zokusei = enZokusei_mizu;
					//HP��0�ɂȂ�����B
					if (m_hp <= 0)
					{
						//�_�E���X�e�[�g�ɑJ�ڂ���B
						m_bossState = enBossState_Death;
					}
					else {
						//��_���[�W�X�e�[�g�ɑJ�ڂ���B
						m_bossState = enBossState_ReceiveDamage;
					}
					//���ʉ����Đ�����B
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(4);
					se->Play(false);
					se->SetVolume(0.6f);
					return;
				}
			}
		}

	}



void Boss::Attack()
{
	//�U���X�e�[�g����Ȃ�������B
	//�������Ȃ��B
	if (m_bossState != enBossState_Attack)
	{
		return;
	}

	//�U�����蒆�ł���΁B
	if (m_isUnderAttack == true)
	{
		//�U���p�̃R���W�������쐬����B
		MakeAttackCollision();
	}
}

const bool Boss::SearchPlayer() const
{
	//�{�X����G�l�~�[�Ɍ������x�N�g�����v�Z����B
	Vector3 diff = m_player->GetPosition() - m_position;

	//�{�X�ƃv���C���[�̋�����������x�߂�������B
	if (diff.LengthSq() <= 700.0 * 700.0f)
	{
		//�G�l�~�[����v���C���[�Ɍ������x�N�g���𐳋K������B
		diff.Normalize();
		//�G�l�~�[�̐��ʂ̃x�N�g���ƁA�G�l�~�[����v���C���[�Ɍ������x�N�g���́B
		//����(cos��)�����߂�B
		float cos = m_forward.Dot(diff);
		//����(cos��)����p�x(��)�����߂�B
		float angle = acosf(cos);
		//�p�x(��)��180����菬������΁B
		if (angle <= (Math::PI / 180.0f) * 180.0f)
		{
			//�v���C���[���������I
			return true;
		}
	}

	return false;
}

void Boss::MakeAttackCollision()
{
	//�U������p�̃R���W�����I�u�W�F�N�g���쐬����B
	auto collisionObject = NewGO<CollisionObject>(0);
	//���̃{�[���̃��[���h�s����擾����B
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	//�{�b�N�X��̃R���W�������쐬����B
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(100.0f, 10.0f, 10.0f));
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("enemy_attack");
}

void Boss::MakeMahou()
{
	//�t�@�C���[�{�[���̃I�u�W�F�N�g���쐬����B
	Mahou* mahou = NewGO<Mahou>(0);
	Vector3 mahouPosition = m_position;
	//���W��������ɂ���B
	mahouPosition.y += 70.0f;
	//���W��ݒ肷��B
	mahou->SetPosition(mahouPosition);
	//��]��ݒ肷��B
	mahou->SetRotation(m_rotation);
	//�p�҂�ݒ肷��B
	mahou->SetEnMagician(Mahou::enMagician_Boss);
}

void Boss::ProcessCommonStateTransition()
{
	//�e�^�C�}�[���������B
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	//�v���C���[����������B
	if (SearchPlayer() == true)
	{
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		//�ړ����x��ݒ肷��B
		m_moveSpeed = diff * 320.0f;
		//�U���ł��鋗���Ȃ�B
		if (IsCanAttack() == true)
		{
			//�����ɂ���āA�U�����邩�ҋ@�����邩�����肷��B	
			int ram = rand() % 100;
			if (ram > 30)
			{
				//�U���X�e�[�g�Ɉڍs����B
				m_bossState = enBossState_Attack;
				m_isUnderAttack = false;
				return;
			}
			else
			{
				//�ҋ@�X�e�[�g�Ɉڍs����B
				m_bossState = enBossState_Idle;
				return;
			}

		}
		//�U���ł��Ȃ������Ȃ�B
		else
		{
			//�����ɂ���āA�ǐՂ����邩���@�U�������邩���肷��B	
			int ram = rand() % 100;
			if (ram > 40)
			{
				//�ǐՃX�e�[�g�Ɉڍs����B
				m_bossState = enBossState_Chase;
				return;
			}
			else {
				//���݂����@�U���X�e�[�g�Ȃ�B
				if (m_bossState == enBossState_MagicAttack)
				{
					//�A���Ŗ��@���������Ȃ��悤�ɁB
					//�ǐՃX�e�[�g�Ɉڍs����B
					m_bossState = enBossState_Chase;
					return;
				}
				//���݂����@�U���X�e�[�g�łȂ��Ȃ�B
				else
				{
					//���@�U���X�e�[�g�Ɉڍs����B
					m_bossState = enBossState_MagicAttack;
					EffectEmitter* effect = NewGO<EffectEmitter>(0);
					effect->Init(enCast_fire);
					Vector3 effectPosition = m_position;
					effect->SetPosition(m_position);
					effect->SetScale(Vector3::One * 10.0f);
					effect->Play();
					return;
				}
			}
		}
	}
	//�v���C���[���������Ȃ���΁B
	else
	{
		//�ҋ@�X�e�[�g�Ɉڍs����B
		m_bossState = enBossState_Idle;
		return;

	}
}

void Boss::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//�ҋ@���Ԃ�������x�o�߂�����B
	if (m_idleTimer >= 0.5f)
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
	}

}

void Boss::ProcessWalkStateTransition()
{
	//���̃X�e�[�g�֑J�ڂ���B
	ProcessCommonStateTransition();
}

void Boss::ProcessRunStateTransition()
{
	//���̃X�e�[�g�֑J�ڂ���B
	ProcessCommonStateTransition();
}

void Boss::ProcessChaseStateTransition()
{
	//�U���ł��鋗���Ȃ�B
	if (IsCanAttack())
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	//�ǐՎ��Ԃ�������x�o�߂�����B
	if (m_chaseTimer >= 0.5f)
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessAttackStateTransition()
{
	//�U���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessMagicAttackStateTransition()
{
	//���@�U���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessReceiveDamageStateTransition()
{
	//��_���[�W�A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//�U�����ꂽ�狗���֌W�����ɁA��芸�����ǐՂ�����B
		m_bossState = enBossState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		//�ړ����x��ݒ肷��B
		m_moveSpeed = diff * 100.0f;
	}
}

void Boss::ProcessDeathStateTransition()
{
	//�_�E���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_game->SetBossDie();
		//���g���폜����B
		//DeleteGO(this);
	}
}

void Boss::ManageState()
{
	switch (m_bossState)
	{
		//�ҋ@�X�e�[�g�̎��B
	case enBossState_Idle:
		//�ҋ@�X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessIdleStateTransition();
		break;
		//�ǐՃX�e�[�g�̎��B
	case enBossState_Chase:
		//�ǐՃX�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessChaseStateTransition();
		break;
		//�U���X�e�[�g�̎��B
	case enBossState_Attack:
		//�U���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessAttackStateTransition();
		break;
		//���@�U���X�e�[�g�̎��B
	case enBossState_MagicAttack:
		//���@�U���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessMagicAttackStateTransition();
		break;
		//��_���[�W�X�e�[�g�̎��B
	case enBossState_ReceiveDamage:
		//��_���[�W�X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessReceiveDamageStateTransition();
		break;
		//�_�E���X�e�[�g�̎��B
	case enBossState_Death:
		//�_�E���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessDeathStateTransition();
		break;
	}
}

void Boss::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.5f);
	switch (m_bossState)
	{
		//�ҋ@�X�e�[�g�̎��B
	case enBossState_Idle:
		//�ҋ@�A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.5f);
		break;
		//�ǐՃX�e�[�g�̎��B
	case enBossState_Chase:
		m_modelRender.SetAnimationSpeed(0.5f);
		//����A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//�U���X�e�[�g�̎��B
	case enBossState_Attack:
		m_modelRender.SetAnimationSpeed(0.5f);
		//�U���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
		//���@�U���X�e�[�g�̎��B
	case enBossState_MagicAttack:
		m_modelRender.SetAnimationSpeed(0.5f);
		//���@�U���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Mahou, 0.1f);
		break;
		//��_���[�W�X�e�[�g�̎��B
	case enBossState_ReceiveDamage:
		m_modelRender.SetAnimationSpeed(0.5f);
		//��_���[�W�A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
		//�_�E���X�e�[�g�̎��B
	case enBossState_Death:
		//�_�E���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Death, 0.1f);
		break;
	default:
		break;
	}
}

void Boss::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//�L�[�̖��O���uattack_start�v�̎��B
	if (wcscmp(eventName, L"attack_start") == 0) {
		//�U���������true�ɂ���B
		m_isUnderAttack = true;
		//�U���G�t�F�N�g�𔭐�������B
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(enSlash);
		//�傫����ݒ肷��B
		effectEmitter->SetScale(Vector3::One * 12.0f);
		Vector3 effectPosition = m_position;
		//���W��������ɂ���B
		effectPosition.y += 70.0f;
		//���W��ݒ肷��B
		effectEmitter->SetPosition(effectPosition);
		Quaternion rotation;
		rotation = m_rotation;
		rotation.AddRotationDegY(360.0f);
		//��]��ݒ肷��B
		effectEmitter->SetRotation(rotation);
		//�G�t�F�N�g���Đ�����B
		effectEmitter->Play();
		//���ʉ����Đ�����B
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(3);
		se->Play(false);
		se->SetVolume(0.8f);
	}
	//�L�[�̖��O���uattack_end�v�̎��B
	else if (wcscmp(eventName, L"attack_end") == 0) {
		//�U���������false�ɂ���B
		m_isUnderAttack = false;
	}
	//�L�[�̖��O���umagic_attack�v�̎��B
	else if (wcscmp(eventName, L"mahou_start") == 0) {
		//�t�@�C���[�{�[���̃I�u�W�F�N�g���쐬����B
		MakeMahou();
	}
}

const bool Boss::IsCanAttack() const
{
	//�{�X����v���C���[�Ɍ������x�N�g�����v�Z����B
	Vector3 diff = m_player->GetPosition() - m_position;
	//�{�X�ƃv���C���[�̋������߂�������B
	if (diff.LengthSq() <= 100.0f * 100.0f)
	{
		//�U���ł���I
		return true;
	}
	//�U���ł��Ȃ��B
	return false;
}

void Boss::Render(RenderContext& rc)
{
	//���f����`�悷��B
	m_modelRender.Draw(rc);
	//HP�o�[�`��B
	m_bosshpbahaikei.Draw(rc);
	m_bosshpbar.Draw(rc);
	m_bosshpbapa.Draw(rc);
	m_spriteRendersankakukankei.Draw(rc);
	//�摜�̕`��B
	if (m_zokusei == enZokusei_hi)
	{
		m_spriteRenderfire.Draw(rc);
	}
	else if (m_zokusei == enZokusei_mizu)
	{
		m_spriteRendermizu.Draw(rc);
	}
	else if(m_zokusei == enZokusei_kaze)
	{
		m_spriteRenderkaze.Draw(rc);
	}
}