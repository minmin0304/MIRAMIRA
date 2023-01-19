#pragma once
#include<Zokusei.h>
class Game;
class Collision;

//プレイヤークラス。
class Player : public IGameObject
{
	//プレイヤーのステート。
	enum EnPlayerState {
		enPlayerState_Idle,					//待機。
		enPlayerState_Run,					//走る。
		enPlayerState_Attack,				//攻撃。
		enPlayerState_Mahou,			    //魔法攻撃。
		enPlayerState_ReceiveDamage,		//ダメ―ジ受けた。
		enPlayerState_Death,			    //HPが0。
		enPlayerState_Clear,			    //クリアー。
		enPlayerState_Fall,                 //落下
		enPlayerState_Winner,               //勝利
		

	};

public:
	Player();
	~Player(){};
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="position">座標。</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns>座標。</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 動ける状態(ステート)かどうかを取得。
	/// </summary>
	/// <returns>動ける状態(ステート)ならtrue。</returns>
	bool IsEnableMove() const
	{
		return m_playerState != enPlayerState_Attack &&
			m_playerState != enPlayerState_Mahou &&
			m_playerState != enPlayerState_ReceiveDamage &&
			m_playerState != enPlayerState_Death &&
			m_playerState != enPlayerState_Winner;
	}

	Game* m_game = nullptr;                           //ゲーム

private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();
	/// <summary>
	/// 攻撃中の処理。
	/// </summary>
	void Attack();
	/// <summary>
	/// 敵の攻撃との当たり判定処理。
	/// </summary>
	void Collision();
	/// <summary>
	/// 攻撃の際の当たり判定用のコリジョンを作成する。
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// 魔法攻撃のファイヤーボールを作成する。
	/// </summary>
	void MakeMahou();
	/// <summary>
	/// 攻撃した際のエフェクトを作成する。
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// アニメーションの再生。
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// 各ステートの遷移処理。
	/// </summary>
	void ManageState();
	/// <summary>
	/// アニメーションイベント用の関数。
	/// </summary>
	/// <param name="clipName">アニメーションの名前。</param>
	/// <param name="eventName">アニメーションイベントのキーの名前。</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// 共通のステート遷移処理。
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// 待機ステートの遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 走りステートの遷移処理。
	/// </summary>
	void ProcessRunStateTransition();
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
	/// 落下ステートの遷移処理。
	/// </summary>
	void ProcessFallStateTransition();
	/// <summary>
	/// ダウンステートの遷移処理。
	/// </summary>
	void ProcessDownStateTransition();
	/// 勝利ステートの遷移処理。
	/// </summary>
	void ProcessWinnerStateTransition();
	/// </summary>
	void Setstate()
	{
		EnPlayerState{ enPlayerState_Winner };
	}
	

	//アニメーション。
	enum EnAnimationClip {		
		enAnimationClip_Idle,   //待機アニメーション。
		enAnimationClip_Run,    //走りアニメーション。
		enAnimationClip_Jump,   //ジャンプアニメーション。
		enAnimationClip_Mahou,  //魔法アニメーション。
		enAnimationClip_Attack, //攻撃アニメーション。
		enAnimationClip_Damage, //被ダメージアニメーション。
		enAnimationClip_Death,  //敗北アニメーション。
		enAnimationClip_Winner, //勝利アニメーション。
		enAnimationClip_Fall,   //落下アニメーション。
		enAnimationClip_Num,    //アニメーションの数。
	};
	AnimationClip                       m_animationClips[enAnimationClip_Num];		//アニメーションクリップ。
	ModelRender				            m_modelRender;                              //モデルレンダー
	CharacterController                 m_charaCon;                                 //キャラクターコントローラー。
	Vector3                             m_moveSpeed;                                //移動速度
	Vector3					            m_forward  = Vector3::AxisZ;                //プレイヤーの正面ベクトル。
	Vector3					            m_forward2 = Vector3::AxisY;                //スポットライトの正面ベクトル
	Vector3					            m_position;	                                //座標。
	SpriteRender					    m_hpBar;                                    //HPバー
	SpriteRender                        m_hpbapa;                                   //HPバー枠
	SpriteRender                        m_hpbahaikei;                               //HPバー背景
	EnPlayerState			            m_playerState = enPlayerState_Idle;         //プレイヤーのステートを待機
	int						            m_swordBoneId = -1;                         //ソードの骨
	Quaternion                          m_rotation;                                 //クォータニオン。
	PointLight                          m_pointLight;                               //ポイントライト
	bool					            m_isUnderAttack = false;                    //攻撃判定
	int						            m_hp = 200;                                 //プレイヤーのHP
	EnZokusei							m_zokusei = enZokusei_hi;                   //魔法の属性
	bool                                m_CD;                                       //魔法クールタイム
	float                               m_CDtimer = 0.0f;                           //魔法クールタイム
	FontRender							debug;                                      //準備完了のテキスト
	FontRender							debug2;                                     //魔法クールタイムテキスト
	Vector3                             m_nokubaku;                                 //ノックバック
	SpotLight					        m_spotLight;                                //スポットライト
};
