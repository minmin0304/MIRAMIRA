#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "Mahou.h"
// CollisionObject���g�p���邽�߂ɁA�t�@�C�����C���N���[�h����B
#include "collision/CollisionObject.h"

namespace
{
	const int MAXIMUM_HP = 200;
}
Player::Player()
{
	//�A�j���[�V�����N���b�v�����[�h����B
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/player/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/player/jump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/player/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Death].Load("Assets/animData/player/death.tka");
	m_animationClips[enAnimationClip_Death].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/player/receiveDamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Mahou].Load("Assets/animData/player/mahou.tka");
	m_animationClips[enAnimationClip_Mahou].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/player/attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Winner].Load("Assets/animData/player/winner.tka");
	m_animationClips[enAnimationClip_Winner].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Fall].Load("Assets/animData/player/fall.tka");
	m_animationClips[enAnimationClip_Fall].SetLoopFlag(false);
	//���j�e�B�����̃��f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/player/Player.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisZ);

	//�L�����R��������������B
	m_charaCon.SetPosition(m_position);

	//�L�����N�^�[�R���g���[���[���������B
	m_charaCon.Init(
		20.0f,			//���a�B
		50.0f,			//�����B
		m_position		//���W�B
	);

	//���ɐݒ肵���{�[����ID���擾����B
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//�A�j���[�V�����C�x���g�p�̊֐���ݒ肷��B
	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});
	

	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/magic.wav");
	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/slash.wav");
	g_soundEngine->ResistWaveFileBank(7, "Assets/sound/hit_pitch.wav");

	/*m_game = FindGO<Game>("game");*/
	/*return true;*/
}

bool Player::Start()
{
	m_hpBar.Init("Assets/sprite/hpbar.dds", 1024, 60);
	m_hpbapa.Init("Assets/sprite/hpbapa2.dds", 1050, 76);
	m_hpbahaikei.Init("Assets/sprite/hpbahaikei.dds", 1050, 76);
	//�s�{�b�g��ݒ肷��B
	//�s�{�b�g�͉摜�̒��S�ł���B
	//�s�{�b�g�𒆐S�ɁA�摜���L�яk�݂���̂ŁB
	//�s�{�b�g�̒l��ύX���邱�Ƃɂ��B
	//�摜�̑傫����ύX���Ă��A�摜���ړ����Ȃ��悤�ɂ���B
	m_hpBar.SetPivot(Vector2(0.0f, 0.5f));
	m_hpbapa.SetPosition(Vector3(-394.0f, -511.0f, 0.0f));
	m_hpbahaikei.SetPosition(Vector3(-394.0f, -511.0f, 0.0f));
	m_hpBar.SetPosition(Vector3(-444.0f, -511.0f, 0.0f));
	
	// �X�|�b�g���C�g���������B
	m_spotLight.Init();
	//���C�g�̉e��������ݒ�B
	m_spotLight.SetRange(1500.0f);
	//���C�g�̐F��ݒ�B
	m_spotLight.SetColor(50.0f, 0.0f, 0.0f);
	//���C�g�̉e���p�x��ݒ�B
	m_spotLight.SetAngle(Math::DegToRad(50.0f));
	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(m_forward2);
	//���C�g�̍��W��ݒ�B
	m_spotLight.SetPosition(m_position);

	return true;
	}
void Player::Update()
{
	if (m_CD)
	{
		debug.SetText(L"�N�[���_�E��");
		m_CDtimer += g_gameTime->GetFrameDeltaTime();
		if (m_CDtimer >= 3.0f) {
			m_CD = false; 
			m_CDtimer = 3.0f;
			
		}
	}
	else
	{
		debug.SetText(L"��������");
		debug.SetPosition(Vector3(670.0f, -400.0f, 0.0f));
	}
	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"%.2f", float(m_CDtimer));

	//�\������e�L�X�g��ݒ�B
	debug2.SetText(wcsbuf);
	debug2.SetPosition(Vector3(670.0f, -440.0f, 0.0f));
	debug2.SetScale(2.0f);
	debug2.SetColor({ 1.0f,0.0f,0.0f,1.0f });

	//�ړ������B
	Move();
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
	Vector3 scale = Vector3::One;
	//��HP/�ő�HP��HP�o�[�̃X�P�[���ɂ���B
	//int�^���m�̌v�Z���ƁA�����_�ȉ��؂�̂ĂɂȂ�̂ŁB
	//float�^�ɕϊ����Čv�Z���s���B
	scale.x = float(m_hp) / float(MAXIMUM_HP);
	//�X�P�[����ݒ�B
	
	m_hpbapa.SetPosition(Vector3(-442.0f, -511.0f, 0.0f));
	m_hpbapa.Update();
	m_hpbahaikei.SetPosition(Vector3(-442.0f, -511.0f, 0.0f));
	m_hpbahaikei.Update();
	m_hpBar.SetScale(scale);
	m_hpBar.SetPosition(Vector3(-944.0f, -510.0f, 0.0f));
	m_hpBar.Update();

	//���C�g�̍��W��ݒ�B
	m_spotLight.SetPosition(m_position/* + Vector3(0.0f, 50.0f, 0.0f)*/);
	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(m_forward2);
	//���C�g���X�V�B3q
	m_spotLight.Update();
}

void Player::Move()
{
	//�ړ��ł��Ȃ���Ԃł���΁A�ړ������͂��Ȃ��B
	if (IsEnableMove() == false)
	{
		return;
	}
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	//���̃t���[���̈ړ��ʂ����߂�B
	//���X�e�B�b�N�̓��͗ʂ��󂯎��B
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();
	//�J�����̑O�������ƉE�������擾�B
	Vector3 cameraForward = g_camera3D->GetForward();
	Vector3 cameraRight = g_camera3D->GetRight();
	//XZ���ʂł̑O�������A�E�����ɕϊ�����B
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();
	//XZ�����̈ړ����x���N���A�B
	m_moveSpeed += cameraForward * lStick_y * 250.0f;	//�������ւ̈ړ����x�����Z�B
	m_moveSpeed += cameraRight * lStick_x * 250.0f;		//�E�����ւ̈ړ����x�����Z�B
	//����y���ړ��͖����B
	/*if (g_pad[0]->IsTrigger(enButtonA) //A�{�^���������ꂽ��
		&& m_charaCon.IsOnGround()  //���A�n�ʂɋ�����
		) {
		//�W�����v����B
		m_moveSpeed.y = 400.0f;	//������ɑ��x��ݒ肵�āA
	}

	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();
	*/


	//�L�����N�^�[�R���g���[���[���g�p���āA���W���X�V�B
	m_position = m_charaCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	if (m_charaCon.IsOnGround()) {
		//�n�ʂɂ����B
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		//�d�͂𔭐�������B
		m_moveSpeed.y -= 2.5f;
	}
	if (m_position.y <=-400.0f)
	{
	
		m_hp -= 50;
		m_position = { 0.0f,0.0f,0.0f };
		m_charaCon.SetPosition(m_position);
		m_playerState= enPlayerState_Idle;
		if (m_hp <= 0)
		{
			//�_�E��������B
			m_playerState = enPlayerState_Death;
		}

	}

	Vector3 modelPosition = m_position;
	//������Ƃ������f���̍��W��������B
	//modelPosition.y += 2.5f;
	m_modelRender.SetPosition(modelPosition);
}

void Player::Rotation()
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

	//�v���C���[�̐��ʃx�N�g�����v�Z����B
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Player::Collision()
{
	//��_���[�W�A�_�E�����A�N���A���̓_���[�W��������Ȃ��B
	if (m_playerState == enPlayerState_ReceiveDamage ||
		m_playerState == enPlayerState_Death ||
		m_playerState == enPlayerState_Clear)
	{
		return;
	}

	{
		//�G�̍U���p�̃R���W�����̔z����擾����B
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_attack");
		//�z���for���ŉ񂷁B
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����B
			if (collision->IsHit(m_charaCon))
			{
				//HP�����炷�B
				m_hp -= 50;
				//HP��0�ɂȂ�����B
				if (m_hp <= 0)
				{
					//�_�E��������B
					m_playerState = enPlayerState_Death;
				}
				//HP��0�ł͂Ȃ�������B
				else {
					//��_���[�W���A�N�V������������B
					m_playerState = enPlayerState_ReceiveDamage;
				}
				//���ʉ����Đ�����B
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(7);
				se->Play(false);
				se->SetVolume(0.7f);
				return;
			}
		}
	}

	{
		//�G�̌������t�@�C���[�{�[���̃R���W�����̔z����擾����B
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_fireball");
		//�z���for���ŉ񂷁B
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����B
			if (collision->IsHit(m_charaCon))
			{
			

				//HP�����炷�B
				m_hp -= 30;
				//�m�b�N�o�b�N                                             //���[���h�̏��  //���W���擾
				btVector3 collisionpos=collision->GetbtCollisionObject().getWorldTransform().getOrigin();
				Vector3 Mahoupos;
				//btVector3��Vector�ɕϊ�
				Mahoupos = { collisionpos.getX(),collisionpos.getY(),collisionpos.getZ() };
				//�m�b�N�o�b�N�̃x�N�g�������߂�
				m_nokubaku = Mahoupos - m_position;
				//Y�����K�v���Ȃ��B
				m_nokubaku.y = 0.0f;
				//�x�N�g���𐳋K��
				m_nokubaku.Normalize();
				//���݂̍��W����m�b�N�o�b�N�����Ɍ��Z�A�m�b�N�o�b�N�͐��l��Z�B
				m_position -= m_nokubaku * 300;
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
				if (m_hp <= 0)
				{
					//�_�E��������B
					m_playerState = enPlayerState_Death;
				}
				//HP��0�ł͂Ȃ�������B
				else {
					//��_���[�W���A�N�V������������B
					m_playerState = enPlayerState_ReceiveDamage;
				}
				//���ʉ����Đ�����B
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(7);
				se->Play(false);
				se->SetVolume(0.7f);
				return;
			}
		}
	}
}

void Player::Attack()
{
	//�U�����łȂ��Ȃ�A���������Ȃ��B
	if (m_playerState != enPlayerState_Attack)
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

void Player::MakeAttackCollision()
{
	//�R���W�����I�u�W�F�N�g���쐬����B
	auto collisionObject = NewGO<CollisionObject>(0);
	//���̃{�[���̃��[���h�s����擾����B
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	//�{�b�N�X��̃R���W�������쐬����B
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(110.0f, 15.0f, 15.0f));
	//���̃{�[���̃��[���h�s����R���W�����ɓK�p������B
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("player_attack");
}

void Player::MakeMahou()
{
	//���@���쐬����B
	Mahou* mahou = NewGO<Mahou>(0, "Mahou");
	Vector3 mahouPosition = m_position;
	//���W��������ɂ���B
	mahouPosition.y += 70.0f;
	mahou->SetPosition(mahouPosition);
	mahou->SetRotation(m_rotation);
	//�p�҂̓v���C���[�ɂ���B
	mahou->SetEnMagician(Mahou::enMagician_Player);

	mahou->SetZokusei(m_zokusei);

	switch (m_zokusei)
	{
	case enZokusei_hi:
		m_zokusei = enZokusei_mizu;
		m_spotLight.SetColor(0.0f, 0.0f, 50.0f);
		break;
	case enZokusei_mizu:
		m_zokusei = enZokusei_kaze;
		m_spotLight.SetColor(0.0f,50.0f, 0.0f);
		break;
	case enZokusei_kaze:
		m_zokusei = enZokusei_hi;
		m_spotLight.SetColor(50.0f, 0.0f, 0.0f);
		break;
	}

}

void Player::MakeSlashingEffect()
{
	//�U���G�t�F�N�g�𔭐�������B
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(enSlash);
	effectEmitter->SetScale(Vector3::One * 11.0f);
	Vector3 effectPosition = m_position;
	//���W��������ɂ���B
	effectPosition.y += 70.0f;
	effectEmitter->SetPosition(effectPosition);
	Quaternion rotation;
	rotation = m_rotation;
	//��]������B
	rotation.AddRotationDegY(360.0f);
	rotation.AddRotationDegZ(180.0f);
	effectEmitter->SetRotation(rotation);
	//�G�t�F�N�g���Đ�����B
	effectEmitter->Play();

	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(3);
	se->Play(false);
	se->SetVolume(0.8f);
}

void Player::ProcessCommonStateTransition()
{
	//�G��S�ł�������B
	if (m_game->GetBossDie()==true)
	{
		//�N���A�X�e�[�g�Ɉڍs����B
		m_playerState = enPlayerState_Winner;
		return;
	}
	//Y�{�^���������ꂽ��B
	if (g_pad[0]->IsTrigger(enButtonY))
	{
		//�U���X�e�[�g�Ɉڍs����B
		m_playerState = enPlayerState_Attack;
		//�t���O��false�ɂ���B
		m_isUnderAttack = false;
		return;
	}
	//X�{�^���������ꂽ��B
	else if (g_pad[0]->IsTrigger(enButtonX)&&!m_CD)
	{
		m_CDtimer = 0.0f;
		//���@�U���X�e�[�g�Ɉڍs����B
		m_playerState = enPlayerState_Mahou;

		//���ʉ��𔭐�������B
		/*SoundSource* se = NewGO<SoundSource>(0);
		se->Init(0);
		se->SetVolume(0.5f);
		se->Play(false);*/
		//�G�t�F�N�g�𔭐�������B
		EffectEmitter* effect = NewGO<EffectEmitter>(0);
		effect->Init(enCast_fire);
		Vector3 effectPosition = m_position;
		effect->SetPosition(m_position);
		effect->SetScale(Vector3::One * 10.0f);
		effect->Play();

		m_CD = true;

		return;
	}
	//�����X�e�[�g�̃^�C�~���O
	if (m_position.y<=-30.0f)
	{
		m_playerState = enPlayerState_Fall;
		return;
	}

	//x��z�̈ړ����x����������(�X�e�B�b�N�̓��͂���������)�B
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//�ړ����x�����ȏゾ������B
		//if (m_moveSpeed.LengthSq() >= 200.0f * 200.0f)
		{
			//�X�e�[�g�𑖂�ɂ���B
			m_playerState = enPlayerState_Run;
			return;
		}

	}
	//x��z�̈ړ����x������������(�X�e�B�b�N�̓��͂�����������)�B
	else
	{
		//�X�e�[�g��ҋ@�ɂ���B
		m_playerState = enPlayerState_Idle;
		return;
	}
}

void Player::ProcessIdleStateTransition()
{
	//�X�e�[�g��J�ڂ���B
	ProcessCommonStateTransition();
}


void Player::ProcessRunStateTransition()
{
	//�X�e�[�g��J�ڂ���B
	ProcessCommonStateTransition();
}

void Player::ProcessAttackStateTransition()
{
	//�U���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//�X�e�[�g��J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Player::ProcessMagicAttackStateTransition()
{
	//���@�U���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_playerState=enPlayerState_Idle;
		//�X�e�[�g��J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Player::ProcessReceiveDamageStateTransition()
{
	//��_���[�W�A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		
		//�X�e�[�g��J�ڂ���B
		ProcessCommonStateTransition();
	}
}
void Player::ProcessFallStateTransition()
{    	
	if(m_modelRender.IsPlayingAnimation() == false)
	    //�X�e�[�g��J�ڂ���B
		ProcessCommonStateTransition();
	
}


void Player::ProcessDownStateTransition()
{
	//�_�E���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//�Q�[���I�[�o�[��ʒm����B
		m_game->NotifyGameOver();

	}
}

void Player::ProcessWinnerStateTransition()
{
	//�����A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{   
		//�Q�[���N���A��ʒm����B
		m_game->NotifyGameClear();
	}
}

void Player::ManageState()
{
	switch (m_playerState)
	{
		//�ҋ@�X�e�[�g�̎��B
	case enPlayerState_Idle:
		//�ҋ@�X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessIdleStateTransition();
		break;
		//����X�e�[�g�̎��B
	case enPlayerState_Run:
		//����X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessRunStateTransition();
		break;
		//�U���X�e�[�g�̎��B
	case enPlayerState_Attack:
		//�U���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessAttackStateTransition();
		break;
		//���@�U���X�e�[�g�̎��B
	case enPlayerState_Mahou:
		//���@�U���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessMagicAttackStateTransition();
		break;
		//��_���[�W���X�e�[�g�̎��B
	case enPlayerState_ReceiveDamage:
		//��_���[�W���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessReceiveDamageStateTransition();
		break;
		//�_�E���X�e�[�g�̎��B
	case enPlayerState_Death:
		//�_�E���X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessDownStateTransition();
		break;
	case enPlayerState_Fall:
		//�����X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessFallStateTransition();
		break;
		//�N���A�X�e�[�g�̎��B
	case enPlayerState_Winner:
		//�N���A�X�e�[�g�̃X�e�[�g�J�ڏ����B
		ProcessWinnerStateTransition();
		break;
	//	//�N���A�X�e�[�g�̎��B
	//case enPlayerState_Clear:
	//	//�N���A�X�e�[�g�̃X�e�[�g�J�ڏ����B
	//	ProcessClearStateTransition();
	//	break;
	}
}

void Player::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_playerState)
	{
		//�ҋ@�X�e�[�g�̎��B
	case enPlayerState_Idle:
		//�ҋ@�A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
		//����X�e�[�g�̎��B
	case enPlayerState_Run:
		m_modelRender.SetAnimationSpeed(1.2f);
		//����A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//�U���X�e�[�g�̎��B
	case enPlayerState_Attack:
		m_modelRender.SetAnimationSpeed(0.9f);
		//�U���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
		//���@�U���X�e�[�g�̎��B
	case enPlayerState_Mahou:
		m_modelRender.SetAnimationSpeed(1.2f);
		//���@�U���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Mahou, 0.1f);
		break;
		//��_���[�W�X�e�[�g�̎��B
	case enPlayerState_ReceiveDamage:
		//��_���[�W�A�j���[�V�������Đ��B
		m_modelRender.SetAnimationSpeed(0.75f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
		//�_�E���X�e�[�g�̎��B
	case enPlayerState_Death:
		//�_�E���A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Death, 0.1f);
		break;
	case enPlayerState_Fall:
		//�����A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Fall, 0.5f);
		break;
		//�N���A�X�e�[�g�̎��B
	case enPlayerState_Winner:
		//�N���A�A�j���[�V�������Đ��B
		m_modelRender.PlayAnimation(enAnimationClip_Winner, 0.1f);
		break;
	default:
		break;
	}
}

void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//�L�[�̖��O���uattack_start�v�̎��B
	if (wcscmp(eventName, L"attack_start") == 0) {
		//�a���G�t�F�N�g���쐬����B
		MakeSlashingEffect();
		//�U�����ɂ���B
		m_isUnderAttack = true;
	}
	//�L�[�̖��O���uattack_end�v�̎��B
	else if (wcscmp(eventName, L"attack_end") == 0) {
		//�U�����I���B
		m_isUnderAttack = false;
	}
	//�L�[�̖��O���umagic_attack�v�̎��B
	else if (wcscmp(eventName, L"mahou_start") == 0) {
		//�t�@�C���[�{�[�����쐬����B
		MakeMahou();
	}
}

void Player::Render(RenderContext& rc)
{
	//���f����`�悷��B
	m_modelRender.Draw(rc);
	debug.Draw(rc);
	debug2.Draw(rc);
	if (m_game->m_bossdie == false)
	{
		m_hpBar.Draw(rc);
		m_hpbahaikei.Draw(rc);
		m_hpbapa.Draw(rc);
	}
}
//�e�T�E���h�����[�h����B
