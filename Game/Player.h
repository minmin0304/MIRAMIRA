#pragma once
#include<Zokusei.h>
class Game;
class Collision;

//�v���C���[�N���X�B
class Player : public IGameObject
{
	//�v���C���[�̃X�e�[�g�B
	enum EnPlayerState {
		enPlayerState_Idle,					//�ҋ@�B
		enPlayerState_Run,					//����B
		enPlayerState_Attack,				//�U���B
		enPlayerState_Mahou,			    //���@�U���B
		enPlayerState_ReceiveDamage,		//�_���\�W�󂯂��B
		enPlayerState_Death,			    //HP��0�B
		enPlayerState_Clear,			    //�N���A�[�B
		enPlayerState_Fall,                 //����
		enPlayerState_Winner,               //����
		

	};

public:
	Player();
	~Player(){};
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ���W��ݒ�B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// ���W���擾�B
	/// </summary>
	/// <returns>���W�B</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// ��]��ݒ�B
	/// </summary>
	/// <param name="rotation">��]�B</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// ��������(�X�e�[�g)���ǂ������擾�B
	/// </summary>
	/// <returns>��������(�X�e�[�g)�Ȃ�true�B</returns>
	bool IsEnableMove() const
	{
		return m_playerState != enPlayerState_Attack &&
			m_playerState != enPlayerState_Mahou &&
			m_playerState != enPlayerState_ReceiveDamage &&
			m_playerState != enPlayerState_Death &&
			m_playerState != enPlayerState_Winner;
	}

	Game* m_game = nullptr;                           //�Q�[��

private:
	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();
	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();
	/// <summary>
	/// �U�����̏����B
	/// </summary>
	void Attack();
	/// <summary>
	/// �G�̍U���Ƃ̓����蔻�菈���B
	/// </summary>
	void Collision();
	/// <summary>
	/// �U���̍ۂ̓����蔻��p�̃R���W�������쐬����B
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// ���@�U���̃t�@�C���[�{�[�����쐬����B
	/// </summary>
	void MakeMahou();
	/// <summary>
	/// �U�������ۂ̃G�t�F�N�g���쐬����B
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// �A�j���[�V�����̍Đ��B
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �e�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ManageState();
	/// <summary>
	/// �A�j���[�V�����C�x���g�p�̊֐��B
	/// </summary>
	/// <param name="clipName">�A�j���[�V�����̖��O�B</param>
	/// <param name="eventName">�A�j���[�V�����C�x���g�̃L�[�̖��O�B</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// ���ʂ̃X�e�[�g�J�ڏ����B
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// �ҋ@�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// ����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// �U���X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// ���@�U���X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessMagicAttackStateTransition();
	/// <summary>
	/// ��_���[�W�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessFallStateTransition();
	/// <summary>
	/// �_�E���X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessDownStateTransition();
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessWinnerStateTransition();
	/// </summary>
	void Setstate()
	{
		EnPlayerState{ enPlayerState_Winner };
	}
	

	//�A�j���[�V�����B
	enum EnAnimationClip {		
		enAnimationClip_Idle,   //�ҋ@�A�j���[�V�����B
		enAnimationClip_Run,    //����A�j���[�V�����B
		enAnimationClip_Jump,   //�W�����v�A�j���[�V�����B
		enAnimationClip_Mahou,  //���@�A�j���[�V�����B
		enAnimationClip_Attack, //�U���A�j���[�V�����B
		enAnimationClip_Damage, //��_���[�W�A�j���[�V�����B
		enAnimationClip_Death,  //�s�k�A�j���[�V�����B
		enAnimationClip_Winner, //�����A�j���[�V�����B
		enAnimationClip_Fall,   //�����A�j���[�V�����B
		enAnimationClip_Num,    //�A�j���[�V�����̐��B
	};
	AnimationClip                       m_animationClips[enAnimationClip_Num];		//�A�j���[�V�����N���b�v�B
	ModelRender				            m_modelRender;                              //���f�������_�[
	CharacterController                 m_charaCon;                                 //�L�����N�^�[�R���g���[���[�B
	Vector3                             m_moveSpeed;                                //�ړ����x
	Vector3					            m_forward  = Vector3::AxisZ;                //�v���C���[�̐��ʃx�N�g���B
	Vector3					            m_forward2 = Vector3::AxisY;                //�X�|�b�g���C�g�̐��ʃx�N�g��
	Vector3					            m_position;	                                //���W�B
	SpriteRender					    m_hpBar;                                    //HP�o�[
	SpriteRender                        m_hpbapa;                                   //HP�o�[�g
	SpriteRender                        m_hpbahaikei;                               //HP�o�[�w�i
	EnPlayerState			            m_playerState = enPlayerState_Idle;         //�v���C���[�̃X�e�[�g��ҋ@
	int						            m_swordBoneId = -1;                         //�\�[�h�̍�
	Quaternion                          m_rotation;                                 //�N�H�[�^�j�I���B
	PointLight                          m_pointLight;                               //�|�C���g���C�g
	bool					            m_isUnderAttack = false;                    //�U������
	int						            m_hp = 200;                                 //�v���C���[��HP
	EnZokusei							m_zokusei = enZokusei_hi;                   //���@�̑���
	bool                                m_CD;                                       //���@�N�[���^�C��
	float                               m_CDtimer = 0.0f;                           //���@�N�[���^�C��
	FontRender							debug;                                      //���������̃e�L�X�g
	FontRender							debug2;                                     //���@�N�[���^�C���e�L�X�g
	Vector3                             m_nokubaku;                                 //�m�b�N�o�b�N
	SpotLight					        m_spotLight;                                //�X�|�b�g���C�g
};
