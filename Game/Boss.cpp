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

	//ピボットを設定する。
	//ピボットは画像の中心である。
	//ピボットを中心に、画像が伸び縮みするので。
	//ピボットの値を変更することにより。
	//画像の大きさを変更しても、画像が移動しないようにする。
	m_bosshpbar.SetPivot(Vector2(0.0f, 0.5f));
	m_bosshpbapa.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbahaikei.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbar.SetPosition(Vector3(-2000.0f,502.0f, 0.0f));
	m_spriteRenderkaze.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRendermizu.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRenderfire.SetPosition(Vector3(846.0f, 430.0f, 0.0f));
	m_spriteRendersankakukankei.SetPosition(Vector3(507.0f, 405.0f, 0.0f));

	//アニメーションを読み込む。
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
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/boss/boss.tkm", m_animationClips, enAnimationClip_Num);
	//座標を設定する。

	m_modelRender.SetPosition(m_position);
	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
	//大きさを設定する。
	m_modelRender.SetScale(m_scale);

	// ナビメッシュを構築。
	m_nvmMesh.Init("Assets/nv/stage.tkn");

	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		50.0f,			//高さ。
		m_position		//座標。
	);

	//剣のボーンのIDを取得する。
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//アニメーションイベント用の関数を設定する。
	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});

	//音を読み込む。
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/magic.wav");
	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/slash.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/hit.wav");

	m_player = FindGO<Player>("player");
	m_game = FindGO<Game>("game");
	//乱数を初期化。
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	//更新処理。
	m_bosshpbar.Update();
	m_hp = MAXIMUM_HP;


	return true;
}

void Boss::Update()
{
	
	//追跡処理。
	Chase();
	//回転処理。
	Rotation();
	//当たり判定。
	Collision();
	//攻撃処理。
	Attack();
	//アニメーションの再生。
	PlayAnimation();
	//ステートの遷移処理。
	ManageState();

	//モデルの更新。
	m_modelRender.Update();
	m_spriteRenderkaze.Update();
	m_spriteRendermizu.Update();
	m_spriteRenderfire.Update();
	m_spriteRendersankakukankei.Update();

	Vector3 move = Vector3::Zero;
	m_charaCon.Execute(move, 0);
	m_modelRender.Update();

	Vector3 scale = Vector3::One;
	//現HP/最大HPをHPバーのスケールにする。
	//int型同士の計算だと、小数点以下切り捨てになるので。
	//float型に変換して計算を行う。
	scale.x = float(m_hp) / float(MAXIMUM_HP);
	//スケールを設定。
	m_bosshpbapa.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbapa.Update();
	m_bosshpbahaikei.SetPosition(Vector3(-435.0f, 502.0f, 0.0f));
	m_bosshpbahaikei.Update();
	m_bosshpbar.SetScale(scale);
	m_bosshpbar.SetPosition(Vector3(-944.0f, 502.0f, 0.0f));
	//更新処理。
	m_bosshpbar.Update();
}

void Boss::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		//m_moveSpeed.xとm_moveSpeed.zの絶対値がともに0.001以下ということは
		//このフレームではキャラは移動していないので旋回する必要はない。
		return;
	}
	//atan2はtanθの値を角度(ラジアン単位)に変換してくれる関数。
	//m_moveSpeed.x / m_moveSpeed.zの結果はtanθになる。
	//atan2を使用して、角度を求めている。
	//これが回転角度になる。
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	//atanが返してくる角度はラジアン単位なので
	//SetRotationDegではなくSetRotationを使用する。
	m_rotation.SetRotationY(-angle);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);

	//プレイヤーの前ベクトルを計算する。
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Boss::Chase()
{
	//追跡ステートでないなら、追跡処理はしない。
	if (m_bossState != enBossState_Chase)
	{
		return;
	}

	m_targetPointPosition = m_player->GetPosition();
	bool isEnd;
	//if(){
		// パス検索
	m_pathFiding.Execute(
		m_path,							// 構築されたパスの格納先
		m_nvmMesh,						// ナビメッシュ
		m_position,						// 開始座標
		m_targetPointPosition,			// 移動目標座標
		PhysicsWorld::GetInstance(),	// 物理エンジン	
		20.0f,							// AIエージェントの半径
		50.0f							// AIエージェントの高さ。
	);
	//}
	// パス上を移動する。
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
	//被ダメージ、あるいはダウンステートの時は。
//当たり判定処理はしない。
	if (m_bossState == enBossState_ReceiveDamage ||
		m_bossState == enBossState_Death)
	{
		return;
	}

	{
		//プレイヤーの攻撃用のコリジョンを取得する。
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");
		//コリジョンの配列をfor文で回す。
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら。
			if (collision->IsHit(m_charaCon))
			{

				//HPを1減らす。
				m_hp -= 30;
				btVector3 collisionpos = collision->GetbtCollisionObject().getWorldTransform().getOrigin();
				Vector3 Mahoupos;
				//btVector3をVectorに変換
				Mahoupos = { collisionpos.getX(),collisionpos.getY(),collisionpos.getZ() };
				//ノックバックのベクトルを求める
				m_bossnokubaku = Mahoupos - m_position;
				//Y方向必要がない。
				m_bossnokubaku.y = 0.0f;
				//ベクトルを正規化
				m_bossnokubaku.Normalize();
				//現在の座標からノックバック方向に減算、ノックバックは数値乗算。
				m_position -= m_bossnokubaku * 50;
				//キャラコンを初期化する。
				m_charaCon.SetPosition(m_position);
				//キャラコンをセットポジションした位置に移動するため0のExecuteの処理を実行
				Vector3 moveSpeed = Vector3::Zero;
				m_charaCon.Execute(moveSpeed, 0);
				//モデルレンダーの座標を更新。
				Vector3 modelPosition = m_position;
				//ちょっとだけモデルの座標を挙げる。
				modelPosition.y += 2.5f;
				m_modelRender.SetPosition(modelPosition);
				m_modelRender.Update();
				//HPが0になったら。
				if (m_hp <=0)
				{
					//ダウンステートに遷移する。
					m_bossState = enBossState_Death;
				}
				else {
					//被ダメージステートに遷移する。
					m_bossState = enBossState_ReceiveDamage;
				}
				//効果音を再生する。
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.4f);
				return;

			}
		}
	}

	{
		//プレイヤーのファイヤーボール用のコリジョンを取得する。
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_fireball");
		//for文で配列を回す。
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら。
			if (collision->IsHit(m_charaCon))
			{
				//bossが風属性の時炎ダメージ食らったら減る量
				if(m_zokusei==enZokusei_kaze){
					m_hp -= 30;
				}
				//bossが水属性の時炎ダメージ食らったら減る量
				else if(m_zokusei==enZokusei_mizu) {
					m_hp -= 5;
				}

				m_zokusei = enZokusei_hi;
				//HPが0になったら。
				if (m_hp <= 0)
				{
					//ダウンステートに遷移する。
					m_bossState = enBossState_Death;
				}
				else {
					//被ダメージステートに遷移する。
					m_bossState = enBossState_ReceiveDamage;
				}
				//効果音を再生する。
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.6f);
				return;
			}
		}
	}
		{
			//プレイヤーのかぜ用のコリジョンを取得する。
			const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_kaze");
			//for文で配列を回す。
			for (auto collision : collisions)
			{
				//コリジョンとキャラコンが衝突したら。
				if (collision->IsHit(m_charaCon))
				{
					//bossが炎属性の時風ダメージ食らったら減る量
					if (m_zokusei == enZokusei_hi) {
						if (m_hp < 600) {
							m_hp += 100;
						}
					}
					//bossが水属性の時風ダメージ食らったら減る量
					else if (m_zokusei == enZokusei_mizu) {
						m_hp -= 50;
					}
					m_zokusei = enZokusei_kaze;
					//HPが0になったら。
					if (m_hp <= 0)
					{
						//ダウンステートに遷移する。
						m_bossState = enBossState_Death;
					}
					else {
						//被ダメージステートに遷移する。
						m_bossState = enBossState_ReceiveDamage;
					}
					//効果音を再生する。
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(4);
					se->Play(false);
					se->SetVolume(0.6f);
					return;

				}
			}
		}
		{
			//プレイヤーのみず用のコリジョンを取得する。
			const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_mizu");
			//for文で配列を回す。
			for (auto collision : collisions)
			{
				//コリジョンとキャラコンが衝突したら。
				if (collision->IsHit(m_charaCon))
				{
					//bossが炎属性の時水ダメージ食らったら減る量
					if (m_zokusei == enZokusei_hi) {
						m_hp -= 50;
					}
					//bossが風属性の時水ダメージ食らったら減る量
					else if (m_zokusei == enZokusei_kaze) {
						m_hp -= 0;
					}
					//水属性に変換する。
					m_zokusei = enZokusei_mizu;
					//HPが0になったら。
					if (m_hp <= 0)
					{
						//ダウンステートに遷移する。
						m_bossState = enBossState_Death;
					}
					else {
						//被ダメージステートに遷移する。
						m_bossState = enBossState_ReceiveDamage;
					}
					//効果音を再生する。
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
	//攻撃ステートじゃなかったら。
	//処理しない。
	if (m_bossState != enBossState_Attack)
	{
		return;
	}

	//攻撃判定中であれば。
	if (m_isUnderAttack == true)
	{
		//攻撃用のコリジョンを作成する。
		MakeAttackCollision();
	}
}

const bool Boss::SearchPlayer() const
{
	//ボスからエネミーに向かうベクトルを計算する。
	Vector3 diff = m_player->GetPosition() - m_position;

	//ボスとプレイヤーの距離がある程度近かったら。
	if (diff.LengthSq() <= 700.0 * 700.0f)
	{
		//エネミーからプレイヤーに向かうベクトルを正規化する。
		diff.Normalize();
		//エネミーの正面のベクトルと、エネミーからプレイヤーに向かうベクトルの。
		//内積(cosθ)を求める。
		float cos = m_forward.Dot(diff);
		//内積(cosθ)から角度(θ)を求める。
		float angle = acosf(cos);
		//角度(θ)が180°より小さければ。
		if (angle <= (Math::PI / 180.0f) * 180.0f)
		{
			//プレイヤーを見つけた！
			return true;
		}
	}

	return false;
}

void Boss::MakeAttackCollision()
{
	//攻撃判定用のコリジョンオブジェクトを作成する。
	auto collisionObject = NewGO<CollisionObject>(0);
	//剣のボーンのワールド行列を取得する。
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	//ボックス状のコリジョンを作成する。
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(100.0f, 10.0f, 10.0f));
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("enemy_attack");
}

void Boss::MakeMahou()
{
	//ファイヤーボールのオブジェクトを作成する。
	Mahou* mahou = NewGO<Mahou>(0);
	Vector3 mahouPosition = m_position;
	//座標を少し上にする。
	mahouPosition.y += 70.0f;
	//座標を設定する。
	mahou->SetPosition(mahouPosition);
	//回転を設定する。
	mahou->SetRotation(m_rotation);
	//術者を設定する。
	mahou->SetEnMagician(Mahou::enMagician_Boss);
}

void Boss::ProcessCommonStateTransition()
{
	//各タイマーを初期化。
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	//プレイヤーを見つけたら。
	if (SearchPlayer() == true)
	{
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		//移動速度を設定する。
		m_moveSpeed = diff * 320.0f;
		//攻撃できる距離なら。
		if (IsCanAttack() == true)
		{
			//乱数によって、攻撃するか待機させるかを決定する。	
			int ram = rand() % 100;
			if (ram > 30)
			{
				//攻撃ステートに移行する。
				m_bossState = enBossState_Attack;
				m_isUnderAttack = false;
				return;
			}
			else
			{
				//待機ステートに移行する。
				m_bossState = enBossState_Idle;
				return;
			}

		}
		//攻撃できない距離なら。
		else
		{
			//乱数によって、追跡させるか魔法攻撃をするか決定する。	
			int ram = rand() % 100;
			if (ram > 40)
			{
				//追跡ステートに移行する。
				m_bossState = enBossState_Chase;
				return;
			}
			else {
				//現在が魔法攻撃ステートなら。
				if (m_bossState == enBossState_MagicAttack)
				{
					//連続で魔法を撃たせないように。
					//追跡ステートに移行する。
					m_bossState = enBossState_Chase;
					return;
				}
				//現在が魔法攻撃ステートでないなら。
				else
				{
					//魔法攻撃ステートに移行する。
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
	//プレイヤーを見つけられなければ。
	else
	{
		//待機ステートに移行する。
		m_bossState = enBossState_Idle;
		return;

	}
}

void Boss::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//待機時間がある程度経過したら。
	if (m_idleTimer >= 0.5f)
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
	}

}

void Boss::ProcessWalkStateTransition()
{
	//他のステートへ遷移する。
	ProcessCommonStateTransition();
}

void Boss::ProcessRunStateTransition()
{
	//他のステートへ遷移する。
	ProcessCommonStateTransition();
}

void Boss::ProcessChaseStateTransition()
{
	//攻撃できる距離なら。
	if (IsCanAttack())
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	//追跡時間がある程度経過したら。
	if (m_chaseTimer >= 0.5f)
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessAttackStateTransition()
{
	//攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessMagicAttackStateTransition()
{
	//魔法攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessReceiveDamageStateTransition()
{
	//被ダメージアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//攻撃されたら距離関係無しに、取り敢えず追跡させる。
		m_bossState = enBossState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		//移動速度を設定する。
		m_moveSpeed = diff * 100.0f;
	}
}

void Boss::ProcessDeathStateTransition()
{
	//ダウンアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_game->SetBossDie();
		//自身を削除する。
		//DeleteGO(this);
	}
}

void Boss::ManageState()
{
	switch (m_bossState)
	{
		//待機ステートの時。
	case enBossState_Idle:
		//待機ステートのステート遷移処理。
		ProcessIdleStateTransition();
		break;
		//追跡ステートの時。
	case enBossState_Chase:
		//追跡ステートのステート遷移処理。
		ProcessChaseStateTransition();
		break;
		//攻撃ステートの時。
	case enBossState_Attack:
		//攻撃ステートのステート遷移処理。
		ProcessAttackStateTransition();
		break;
		//魔法攻撃ステートの時。
	case enBossState_MagicAttack:
		//魔法攻撃ステートのステート遷移処理。
		ProcessMagicAttackStateTransition();
		break;
		//被ダメージステートの時。
	case enBossState_ReceiveDamage:
		//被ダメージステートのステート遷移処理。
		ProcessReceiveDamageStateTransition();
		break;
		//ダウンステートの時。
	case enBossState_Death:
		//ダウンステートのステート遷移処理。
		ProcessDeathStateTransition();
		break;
	}
}

void Boss::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.5f);
	switch (m_bossState)
	{
		//待機ステートの時。
	case enBossState_Idle:
		//待機アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.5f);
		break;
		//追跡ステートの時。
	case enBossState_Chase:
		m_modelRender.SetAnimationSpeed(0.5f);
		//走りアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//攻撃ステートの時。
	case enBossState_Attack:
		m_modelRender.SetAnimationSpeed(0.5f);
		//攻撃アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
		//魔法攻撃ステートの時。
	case enBossState_MagicAttack:
		m_modelRender.SetAnimationSpeed(0.5f);
		//魔法攻撃アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Mahou, 0.1f);
		break;
		//被ダメージステートの時。
	case enBossState_ReceiveDamage:
		m_modelRender.SetAnimationSpeed(0.5f);
		//被ダメージアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
		//ダウンステートの時。
	case enBossState_Death:
		//ダウンアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Death, 0.1f);
		break;
	default:
		break;
	}
}

void Boss::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//キーの名前が「attack_start」の時。
	if (wcscmp(eventName, L"attack_start") == 0) {
		//攻撃中判定をtrueにする。
		m_isUnderAttack = true;
		//攻撃エフェクトを発生させる。
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(enSlash);
		//大きさを設定する。
		effectEmitter->SetScale(Vector3::One * 12.0f);
		Vector3 effectPosition = m_position;
		//座標を少し上にする。
		effectPosition.y += 70.0f;
		//座標を設定する。
		effectEmitter->SetPosition(effectPosition);
		Quaternion rotation;
		rotation = m_rotation;
		rotation.AddRotationDegY(360.0f);
		//回転を設定する。
		effectEmitter->SetRotation(rotation);
		//エフェクトを再生する。
		effectEmitter->Play();
		//効果音を再生する。
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(3);
		se->Play(false);
		se->SetVolume(0.8f);
	}
	//キーの名前が「attack_end」の時。
	else if (wcscmp(eventName, L"attack_end") == 0) {
		//攻撃中判定をfalseにする。
		m_isUnderAttack = false;
	}
	//キーの名前が「magic_attack」の時。
	else if (wcscmp(eventName, L"mahou_start") == 0) {
		//ファイヤーボールのオブジェクトを作成する。
		MakeMahou();
	}
}

const bool Boss::IsCanAttack() const
{
	//ボスからプレイヤーに向かうベクトルを計算する。
	Vector3 diff = m_player->GetPosition() - m_position;
	//ボスとプレイヤーの距離が近かったら。
	if (diff.LengthSq() <= 100.0f * 100.0f)
	{
		//攻撃できる！
		return true;
	}
	//攻撃できない。
	return false;
}

void Boss::Render(RenderContext& rc)
{
	//モデルを描画する。
	m_modelRender.Draw(rc);
	//HPバー描画。
	m_bosshpbahaikei.Draw(rc);
	m_bosshpbar.Draw(rc);
	m_bosshpbapa.Draw(rc);
	m_spriteRendersankakukankei.Draw(rc);
	//画像の描画。
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