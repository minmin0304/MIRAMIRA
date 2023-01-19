#pragma once
#include "Level3DRender/LevelRender.h"
#include "sound/SoundSource.h"
//�G�t�F�N�g�̔z��
enum EnMahoustate {
	enMagic_fire,
	enHoge,
	enSlash,
	enInazuma,
	enDaburuinnpakuto,
	enCast_fire,
	enWater,
};
class Player;
class GameCamera;
class Background;
class Fade;
/// <summary>
/// �Q�[���B
/// </summary>
class Game : public IGameObject
{
public:
	
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// �Q�[���N���A�[��ʒm����B
	/// </summary>
	void NotifyGameClear();
	/// <summary>
	/// �Q�[���I�[�o�[��ʒm����B
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// �G�l�~�[��S�ł������H
	/// </summary>
	/// <returns>�G�l�~�[���S�ł��Ă�����true�B</returns>
	const bool IsWannihilationBoss() const
	{
		return false;//m_numBoss == m_numDefeatedBoss;
	}
	const int& GetBossDie()
	{
		return m_bossdie;
	}
	void SetBossDie()
	{
		m_bossdie = true;
	}
	/// <summary>
	/// �|���ꂽ�G�̐���+1����B
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedBoss++;
	}
	//////////////////////////////////////
	// �����o�ϐ��B
	//////////////////////////////////////
	int						m_numDefeatedBoss = 0;
	int						m_numBoss = 0;
	int                     m_bossdie = false;
private:
	LevelRender				m_levelRender;					//���x���B
	Player*                 m_player = nullptr;				//�v���C���[�B
	GameCamera*             m_gameCamera = nullptr;			//�J�����B
	SkyCube*                m_skyCube = nullptr;			//��B
	Background*             m_background = nullptr;			//�w�i�B
	SoundSource*            m_bgm = nullptr;				//�Q�[������BGM�B
	Fade*                   m_fade = nullptr;				//�t�F�[�h�B
	SpriteRender            m_gameclear;                    //�Q�[���N���A
	bool					m_isWaitFadeout = false;
	

};