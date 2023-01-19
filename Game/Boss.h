#pragma once
#include "Zokusei.h"
#include "tkFile/TknFile.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"
#include "AI/PathFinding/PathFinding.h"

//�N���X�錾�B
class Player;
class Game;
/// <summary>
/// �{�X�B
/// </summary>
class Boss : public IGameObject
{
public:
	//�{�X�X�e�[�g�B
	enum EnBossState {
		enBossState_Idle,					//�ҋ@�B
		enBossState_Chase,					//�ǐՁB
		enBossState_Attack,					//�U���B
		enBossState_MagicAttack,			//���@�U���B
		enBossState_ReceiveDamage,			//��_���[�W�B
		enBossState_Death,					//�_�E���B
	};
	
public:
	Boss();
	~Boss();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ���W��ݒ肷��B
	/// </summary>
	/// <param name="position">���W�B</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// ���W���擾����B
	/// </summary>
	/// <returns>���W�B</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// HP��ݒ肷��B
	/// </summary>
	/// <param name="hp">HP�B</param>
	void SetHP(const int hp)
	{
		m_hp = hp;
	}
	/// <summary>
	/// ��]��ݒ肷��B
	/// </summary>
	/// <param name="rotation">��]�B</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// �傫����ݒ肷��B
	/// </summary>
	/// <param name="scale">�傫���B</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
private:
	/// <summary>
	/// �ǐՏ����B
	/// </summary>
	void Chase();
	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();
	/// <summary>
	/// �U�������B
	/// </summary>
	void Attack();
	/// <summary>
	/// �v���C���[�̍U���Ƃ̓����蔻�菈���B
	/// </summary>
	void Collision();
	/// <summary>
	/// �v���C���[��T������B
	/// </summary>
	/// <returns>�v���C���[������������true�B</returns>
	const bool SearchPlayer() const;
	/// <summary>
	/// �U���p�̓����蔻��R���W�������쐬����B
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// �t�@�C���[�{�[�������쐬����B
	/// </summary>
	void MakeMahou();
	/// <summary>
	/// �A�j���[�V�����̍Đ��B
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �A�j���[�V�����C�x���g�p�̊֐��B
	/// </summary>
	/// <param name="clipName">�A�j���[�V�����̖��O�B</param>
	/// <param name="eventName">�A�j���[�V�����C�x���g�̃L�[�̖��O�B</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// �e�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ManageState();
	/// <summary>
	/// ���ʂ̃X�e�[�g�J�ڏ����B
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// �ҋ@�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessWalkStateTransition();
	/// <summary>
	/// ����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// �ǐՃX�e�[�g�̔w�J�ڏ����B
	/// </summary>
	void ProcessChaseStateTransition();
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
	/// �_�E���X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessDeathStateTransition();
	/// <summary>
	/// ������ݒ肷��B
	/// </summary>
	void SetZokusei(EnZokusei zokusei) {
		m_zokusei = zokusei;
	}
	
	/// <summary>
	/// �U���ł��鋗�����ǂ������ׂ�B
	/// </summary>
	/// <returns>�U���ł���Ȃ�true�B</returns>
	const bool IsCanAttack() const;

	enum EnAnimationClip {						//�A�j���[�V�����B
		enAnimationClip_Idle,					//�ҋ@�A�j���[�V�����B
		enAnimationClip_Run,					//����A�j���[�V�����B
		enAnimationClip_Attack,					//�U���A�j���[�V�����B
		enAnimationClip_Mahou,			        //���@�U���A�j���[�V�����B
		enAnimationClip_Damage,					//��_���[�W�A�j���[�V�����B
		enAnimationClip_Death,					//�_�E���A�j���[�V�����B
		enAnimationClip_Num,					//�A�j���[�V�����̐��B
	};
	
	AnimationClip			m_animationClips[enAnimationClip_Num];		//�A�j���[�V�����N���b�v�B
	ModelRender				m_modelRender;								//���f�������_�[�B
	Vector3					m_position;									//���W�B			
	Vector3					m_moveSpeed;								//�ړ����x�B
	Vector3					m_forward = Vector3::AxisZ;					//�{�X�̐��ʃx�N�g���B
	Quaternion				m_rotation;									//�N�H�[�^�j�I���B
	Vector3					m_scale = Vector3::One;						//�傫���B
	CharacterController		m_charaCon;									//�L�����R���B
	EnBossState				m_bossState = enBossState_Idle;				//�{�X�X�e�[�g�B
	bool					m_isUnderAttack = false;					//�U�����蒆���H
	int						m_swordBoneId = -1;							//���̃{�[����ID�B
	int						m_hp = 0;									//HP�B
	SpriteRender    	    m_bosshpbar;                                //HP�o�[
	SpriteRender            m_bosshpbapa;                               //HP�o�[�g
	SpriteRender            m_bosshpbahaikei;                           //HP�o�[�w�i
	Player*                 m_player = nullptr;							//�v���C���[�B
	float					m_chaseTimer = 0.0f;						//�ǐՃ^�C�}�[�B
	float					m_idleTimer = 0.0f;		                    //�ҋ@�^�C�}�[�B
	EnZokusei				m_zokusei = enZokusei_Num;                  //boss�̑���
	Vector3                 m_bossnokubaku;                             //�{�X�̍U���Ƀm�b�N�o�b�N
	
	FontRender			    m_fontRender;
	SpriteRender			m_spriteRenderfire;                         //boss�����ϊ��̉摜  ��
	SpriteRender            m_spriteRendermizu;                         //��
	SpriteRender            m_spriteRenderkaze;                         //��
	SpriteRender            m_spriteRendersankakukankei;                //���������̉摜
	Game*                   m_game = nullptr;


	TknFile m_tknFile;
	PhysicsStaticObject m_bgObject;
	nsAI::NaviMesh m_nvmMesh;
	nsAI::Path m_path;
	nsAI::PathFinding m_pathFiding;
	Vector3 m_targetPointPosition;
};



