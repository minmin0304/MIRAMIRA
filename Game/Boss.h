#pragma once
#include "Zokusei.h"
#include "tkFile/TknFile.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"
#include "AI/PathFinding/PathFinding.h"

//クラス宣言。
class Player;
class Game;
/// <summary>
/// ボス。
/// </summary>
class Boss : public IGameObject
{
public:
	//ボスステート。
	enum EnBossState {
		enBossState_Idle,					//待機。
		enBossState_Chase,					//追跡。
		enBossState_Attack,					//攻撃。
		enBossState_MagicAttack,			//魔法攻撃。
		enBossState_ReceiveDamage,			//被ダメージ。
		enBossState_Death,					//ダウン。
	};
	
public:
	Boss();
	~Boss();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 座標を設定する。
	/// </summary>
	/// <param name="position">座標。</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 座標を取得する。
	/// </summary>
	/// <returns>座標。</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// HPを設定する。
	/// </summary>
	/// <param name="hp">HP。</param>
	void SetHP(const int hp)
	{
		m_hp = hp;
	}
	/// <summary>
	/// 回転を設定する。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 大きさを設定する。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
private:
	/// <summary>
	/// 追跡処理。
	/// </summary>
	void Chase();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();
	/// <summary>
	/// 攻撃処理。
	/// </summary>
	void Attack();
	/// <summary>
	/// プレイヤーの攻撃との当たり判定処理。
	/// </summary>
	void Collision();
	/// <summary>
	/// プレイヤーを探索する。
	/// </summary>
	/// <returns>プレイヤーが見つかったらtrue。</returns>
	const bool SearchPlayer() const;
	/// <summary>
	/// 攻撃用の当たり判定コリジョンを作成する。
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// ファイヤーボールをを作成する。
	/// </summary>
	void MakeMahou();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// アニメーションイベント用の関数。
	/// </summary>
	/// <param name="clipName">アニメーションの名前。</param>
	/// <param name="eventName">アニメーションイベントのキーの名前。</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// 各ステートの遷移処理。
	/// </summary>
	void ManageState();
	/// <summary>
	/// 共通のステート遷移処理。
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// 待機ステートの遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 歩きステートの遷移処理。
	/// </summary>
	void ProcessWalkStateTransition();
	/// <summary>
	/// 走りステートの遷移処理。
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// 追跡ステートの背遷移処理。
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// 攻撃ステートの遷移処理。
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理。
	/// </summary>
	void ProcessMagicAttackStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理。
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// ダウンステートの遷移処理。
	/// </summary>
	void ProcessDeathStateTransition();
	/// <summary>
	/// 属性を設定する。
	/// </summary>
	void SetZokusei(EnZokusei zokusei) {
		m_zokusei = zokusei;
	}
	
	/// <summary>
	/// 攻撃できる距離かどうか調べる。
	/// </summary>
	/// <returns>攻撃できるならtrue。</returns>
	const bool IsCanAttack() const;

	enum EnAnimationClip {						//アニメーション。
		enAnimationClip_Idle,					//待機アニメーション。
		enAnimationClip_Run,					//走りアニメーション。
		enAnimationClip_Attack,					//攻撃アニメーション。
		enAnimationClip_Mahou,			        //魔法攻撃アニメーション。
		enAnimationClip_Damage,					//被ダメージアニメーション。
		enAnimationClip_Death,					//ダウンアニメーション。
		enAnimationClip_Num,					//アニメーションの数。
	};
	
	AnimationClip			m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				m_modelRender;								//モデルレンダー。
	Vector3					m_position;									//座標。			
	Vector3					m_moveSpeed;								//移動速度。
	Vector3					m_forward = Vector3::AxisZ;					//ボスの正面ベクトル。
	Quaternion				m_rotation;									//クォータニオン。
	Vector3					m_scale = Vector3::One;						//大きさ。
	CharacterController		m_charaCon;									//キャラコン。
	EnBossState				m_bossState = enBossState_Idle;				//ボスステート。
	bool					m_isUnderAttack = false;					//攻撃判定中か？
	int						m_swordBoneId = -1;							//剣のボーンのID。
	int						m_hp = 0;									//HP。
	SpriteRender    	    m_bosshpbar;                                //HPバー
	SpriteRender            m_bosshpbapa;                               //HPバー枠
	SpriteRender            m_bosshpbahaikei;                           //HPバー背景
	Player*                 m_player = nullptr;							//プレイヤー。
	float					m_chaseTimer = 0.0f;						//追跡タイマー。
	float					m_idleTimer = 0.0f;		                    //待機タイマー。
	EnZokusei				m_zokusei = enZokusei_Num;                  //bossの属性
	Vector3                 m_bossnokubaku;                             //ボスの攻撃にノックバック
	
	FontRender			    m_fontRender;
	SpriteRender			m_spriteRenderfire;                         //boss属性変換の画像  炎
	SpriteRender            m_spriteRendermizu;                         //水
	SpriteRender            m_spriteRenderkaze;                         //風
	SpriteRender            m_spriteRendersankakukankei;                //属性相性の画像
	Game*                   m_game = nullptr;


	TknFile m_tknFile;
	PhysicsStaticObject m_bgObject;
	nsAI::NaviMesh m_nvmMesh;
	nsAI::Path m_path;
	nsAI::PathFinding m_pathFiding;
	Vector3 m_targetPointPosition;
};



