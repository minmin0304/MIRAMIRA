#pragma once
#include "Level3DRender/LevelRender.h"
#include "sound/SoundSource.h"
//エフェクトの配列
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
/// ゲーム。
/// </summary>
class Game : public IGameObject
{
public:
	
	//////////////////////////////////////
	// メンバ関数。
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ゲームクリアーを通知する。
	/// </summary>
	void NotifyGameClear();
	/// <summary>
	/// ゲームオーバーを通知する。
	/// </summary>
	void NotifyGameOver();
	/// <summary>
	/// エネミーを全滅させた？
	/// </summary>
	/// <returns>エネミーが全滅していたらtrue。</returns>
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
	/// 倒された敵の数を+1する。
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedBoss++;
	}
	//////////////////////////////////////
	// メンバ変数。
	//////////////////////////////////////
	int						m_numDefeatedBoss = 0;
	int						m_numBoss = 0;
	int                     m_bossdie = false;
private:
	LevelRender				m_levelRender;					//レベル。
	Player*                 m_player = nullptr;				//プレイヤー。
	GameCamera*             m_gameCamera = nullptr;			//カメラ。
	SkyCube*                m_skyCube = nullptr;			//空。
	Background*             m_background = nullptr;			//背景。
	SoundSource*            m_bgm = nullptr;				//ゲーム中のBGM。
	Fade*                   m_fade = nullptr;				//フェード。
	SpriteRender            m_gameclear;                    //ゲームクリア
	bool					m_isWaitFadeout = false;
	

};