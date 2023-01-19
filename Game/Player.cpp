#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "Mahou.h"
// CollisionObjectを使用するために、ファイルをインクルードする。
#include "collision/CollisionObject.h"

namespace
{
	const int MAXIMUM_HP = 200;
}
Player::Player()
{
	//アニメーションクリップをロードする。
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
	//ユニティちゃんのモデルを読み込む。
	m_modelRender.Init("Assets/modelData/player/Player.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisZ);

	//キャラコンを初期化する。
	m_charaCon.SetPosition(m_position);

	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		50.0f,			//高さ。
		m_position		//座標。
	);

	//剣に設定したボーンのIDを取得する。
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//アニメーションイベント用の関数を設定する。
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
	//ピボットを設定する。
	//ピボットは画像の中心である。
	//ピボットを中心に、画像が伸び縮みするので。
	//ピボットの値を変更することにより。
	//画像の大きさを変更しても、画像が移動しないようにする。
	m_hpBar.SetPivot(Vector2(0.0f, 0.5f));
	m_hpbapa.SetPosition(Vector3(-394.0f, -511.0f, 0.0f));
	m_hpbahaikei.SetPosition(Vector3(-394.0f, -511.0f, 0.0f));
	m_hpBar.SetPosition(Vector3(-444.0f, -511.0f, 0.0f));
	
	// スポットライトを初期化。
	m_spotLight.Init();
	//ライトの影響距離を設定。
	m_spotLight.SetRange(1500.0f);
	//ライトの色を設定。
	m_spotLight.SetColor(50.0f, 0.0f, 0.0f);
	//ライトの影響角度を設定。
	m_spotLight.SetAngle(Math::DegToRad(50.0f));
	//ライトの方向を設定。
	m_spotLight.SetDirection(m_forward2);
	//ライトの座標を設定。
	m_spotLight.SetPosition(m_position);

	return true;
	}
void Player::Update()
{
	if (m_CD)
	{
		debug.SetText(L"クールダウン");
		m_CDtimer += g_gameTime->GetFrameDeltaTime();
		if (m_CDtimer >= 3.0f) {
			m_CD = false; 
			m_CDtimer = 3.0f;
			
		}
	}
	else
	{
		debug.SetText(L"準備完了");
		debug.SetPosition(Vector3(670.0f, -400.0f, 0.0f));
	}
	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"%.2f", float(m_CDtimer));

	//表示するテキストを設定。
	debug2.SetText(wcsbuf);
	debug2.SetPosition(Vector3(670.0f, -440.0f, 0.0f));
	debug2.SetScale(2.0f);
	debug2.SetColor({ 1.0f,0.0f,0.0f,1.0f });

	//移動処理。
	Move();
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
	Vector3 scale = Vector3::One;
	//現HP/最大HPをHPバーのスケールにする。
	//int型同士の計算だと、小数点以下切り捨てになるので。
	//float型に変換して計算を行う。
	scale.x = float(m_hp) / float(MAXIMUM_HP);
	//スケールを設定。
	
	m_hpbapa.SetPosition(Vector3(-442.0f, -511.0f, 0.0f));
	m_hpbapa.Update();
	m_hpbahaikei.SetPosition(Vector3(-442.0f, -511.0f, 0.0f));
	m_hpbahaikei.Update();
	m_hpBar.SetScale(scale);
	m_hpBar.SetPosition(Vector3(-944.0f, -510.0f, 0.0f));
	m_hpBar.Update();

	//ライトの座標を設定。
	m_spotLight.SetPosition(m_position/* + Vector3(0.0f, 50.0f, 0.0f)*/);
	//ライトの方向を設定。
	m_spotLight.SetDirection(m_forward2);
	//ライトを更新。3q
	m_spotLight.Update();
}

void Player::Move()
{
	//移動できない状態であれば、移動処理はしない。
	if (IsEnableMove() == false)
	{
		return;
	}
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	//このフレームの移動量を求める。
	//左スティックの入力量を受け取る。
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();
	//カメラの前方方向と右方向を取得。
	Vector3 cameraForward = g_camera3D->GetForward();
	Vector3 cameraRight = g_camera3D->GetRight();
	//XZ平面での前方方向、右方向に変換する。
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();
	//XZ成分の移動速度をクリア。
	m_moveSpeed += cameraForward * lStick_y * 250.0f;	//奥方向への移動速度を加算。
	m_moveSpeed += cameraRight * lStick_x * 250.0f;		//右方向への移動速度を加算。
	//現在y軸移動は無し。
	/*if (g_pad[0]->IsTrigger(enButtonA) //Aボタンが押されたら
		&& m_charaCon.IsOnGround()  //かつ、地面に居たら
		) {
		//ジャンプする。
		m_moveSpeed.y = 400.0f;	//上方向に速度を設定して、
	}

	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();
	*/


	//キャラクターコントローラーを使用して、座標を更新。
	m_position = m_charaCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	if (m_charaCon.IsOnGround()) {
		//地面についた。
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		//重力を発生させる。
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
			//ダウンさせる。
			m_playerState = enPlayerState_Death;
		}

	}

	Vector3 modelPosition = m_position;
	//ちょっとだけモデルの座標を挙げる。
	//modelPosition.y += 2.5f;
	m_modelRender.SetPosition(modelPosition);
}

void Player::Rotation()
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

	//プレイヤーの正面ベクトルを計算する。
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Player::Collision()
{
	//被ダメージ、ダウン中、クリア時はダメージ判定をしない。
	if (m_playerState == enPlayerState_ReceiveDamage ||
		m_playerState == enPlayerState_Death ||
		m_playerState == enPlayerState_Clear)
	{
		return;
	}

	{
		//敵の攻撃用のコリジョンの配列を取得する。
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_attack");
		//配列をfor文で回す。
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら。
			if (collision->IsHit(m_charaCon))
			{
				//HPを減らす。
				m_hp -= 50;
				//HPが0になったら。
				if (m_hp <= 0)
				{
					//ダウンさせる。
					m_playerState = enPlayerState_Death;
				}
				//HPが0ではなかったら。
				else {
					//被ダメージリアクションをさせる。
					m_playerState = enPlayerState_ReceiveDamage;
				}
				//効果音を再生する。
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(7);
				se->Play(false);
				se->SetVolume(0.7f);
				return;
			}
		}
	}

	{
		//敵の撃ったファイヤーボールのコリジョンの配列を取得する。
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_fireball");
		//配列をfor文で回す。
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら。
			if (collision->IsHit(m_charaCon))
			{
			

				//HPを減らす。
				m_hp -= 30;
				//ノックバック                                             //ワールドの情報  //座標を取得
				btVector3 collisionpos=collision->GetbtCollisionObject().getWorldTransform().getOrigin();
				Vector3 Mahoupos;
				//btVector3をVectorに変換
				Mahoupos = { collisionpos.getX(),collisionpos.getY(),collisionpos.getZ() };
				//ノックバックのベクトルを求める
				m_nokubaku = Mahoupos - m_position;
				//Y方向必要がない。
				m_nokubaku.y = 0.0f;
				//ベクトルを正規化
				m_nokubaku.Normalize();
				//現在の座標からノックバック方向に減算、ノックバックは数値乗算。
				m_position -= m_nokubaku * 300;
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
				if (m_hp <= 0)
				{
					//ダウンさせる。
					m_playerState = enPlayerState_Death;
				}
				//HPが0ではなかったら。
				else {
					//被ダメージリアクションをさせる。
					m_playerState = enPlayerState_ReceiveDamage;
				}
				//効果音を再生する。
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
	//攻撃中でないなら、処理をしない。
	if (m_playerState != enPlayerState_Attack)
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

void Player::MakeAttackCollision()
{
	//コリジョンオブジェクトを作成する。
	auto collisionObject = NewGO<CollisionObject>(0);
	//剣のボーンのワールド行列を取得する。
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	//ボックス状のコリジョンを作成する。
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(110.0f, 15.0f, 15.0f));
	//剣のボーンのワールド行列をコリジョンに適用させる。
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("player_attack");
}

void Player::MakeMahou()
{
	//魔法を作成する。
	Mahou* mahou = NewGO<Mahou>(0, "Mahou");
	Vector3 mahouPosition = m_position;
	//座標を少し上にする。
	mahouPosition.y += 70.0f;
	mahou->SetPosition(mahouPosition);
	mahou->SetRotation(m_rotation);
	//術者はプレイヤーにする。
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
	//攻撃エフェクトを発生させる。
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	effectEmitter->Init(enSlash);
	effectEmitter->SetScale(Vector3::One * 11.0f);
	Vector3 effectPosition = m_position;
	//座標を少し上にする。
	effectPosition.y += 70.0f;
	effectEmitter->SetPosition(effectPosition);
	Quaternion rotation;
	rotation = m_rotation;
	//回転させる。
	rotation.AddRotationDegY(360.0f);
	rotation.AddRotationDegZ(180.0f);
	effectEmitter->SetRotation(rotation);
	//エフェクトを再生する。
	effectEmitter->Play();

	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(3);
	se->Play(false);
	se->SetVolume(0.8f);
}

void Player::ProcessCommonStateTransition()
{
	//敵を全滅させたら。
	if (m_game->GetBossDie()==true)
	{
		//クリアステートに移行する。
		m_playerState = enPlayerState_Winner;
		return;
	}
	//Yボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonY))
	{
		//攻撃ステートに移行する。
		m_playerState = enPlayerState_Attack;
		//フラグをfalseにする。
		m_isUnderAttack = false;
		return;
	}
	//Xボタンが押されたら。
	else if (g_pad[0]->IsTrigger(enButtonX)&&!m_CD)
	{
		m_CDtimer = 0.0f;
		//魔法攻撃ステートに移行する。
		m_playerState = enPlayerState_Mahou;

		//効果音を発生させる。
		/*SoundSource* se = NewGO<SoundSource>(0);
		se->Init(0);
		se->SetVolume(0.5f);
		se->Play(false);*/
		//エフェクトを発生させる。
		EffectEmitter* effect = NewGO<EffectEmitter>(0);
		effect->Init(enCast_fire);
		Vector3 effectPosition = m_position;
		effect->SetPosition(m_position);
		effect->SetScale(Vector3::One * 10.0f);
		effect->Play();

		m_CD = true;

		return;
	}
	//落下ステートのタイミング
	if (m_position.y<=-30.0f)
	{
		m_playerState = enPlayerState_Fall;
		return;
	}

	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//移動速度が一定以上だったら。
		//if (m_moveSpeed.LengthSq() >= 200.0f * 200.0f)
		{
			//ステートを走りにする。
			m_playerState = enPlayerState_Run;
			return;
		}

	}
	//xとzの移動速度が無かったら(スティックの入力が無かったら)。
	else
	{
		//ステートを待機にする。
		m_playerState = enPlayerState_Idle;
		return;
	}
}

void Player::ProcessIdleStateTransition()
{
	//ステートを遷移する。
	ProcessCommonStateTransition();
}


void Player::ProcessRunStateTransition()
{
	//ステートを遷移する。
	ProcessCommonStateTransition();
}

void Player::ProcessAttackStateTransition()
{
	//攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTransition();
	}
}

void Player::ProcessMagicAttackStateTransition()
{
	//魔法攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_playerState=enPlayerState_Idle;
		//ステートを遷移する。
		ProcessCommonStateTransition();
	}
}

void Player::ProcessReceiveDamageStateTransition()
{
	//被ダメージアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		
		//ステートを遷移する。
		ProcessCommonStateTransition();
	}
}
void Player::ProcessFallStateTransition()
{    	
	if(m_modelRender.IsPlayingAnimation() == false)
	    //ステートを遷移する。
		ProcessCommonStateTransition();
	
}


void Player::ProcessDownStateTransition()
{
	//ダウンアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ゲームオーバーを通知する。
		m_game->NotifyGameOver();

	}
}

void Player::ProcessWinnerStateTransition()
{
	//勝利アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{   
		//ゲームクリアを通知する。
		m_game->NotifyGameClear();
	}
}

void Player::ManageState()
{
	switch (m_playerState)
	{
		//待機ステートの時。
	case enPlayerState_Idle:
		//待機ステートのステート遷移処理。
		ProcessIdleStateTransition();
		break;
		//走りステートの時。
	case enPlayerState_Run:
		//走りステートのステート遷移処理。
		ProcessRunStateTransition();
		break;
		//攻撃ステートの時。
	case enPlayerState_Attack:
		//攻撃ステートのステート遷移処理。
		ProcessAttackStateTransition();
		break;
		//魔法攻撃ステートの時。
	case enPlayerState_Mahou:
		//魔法攻撃ステートのステート遷移処理。
		ProcessMagicAttackStateTransition();
		break;
		//被ダメージ時ステートの時。
	case enPlayerState_ReceiveDamage:
		//被ダメージ時ステートのステート遷移処理。
		ProcessReceiveDamageStateTransition();
		break;
		//ダウンステートの時。
	case enPlayerState_Death:
		//ダウンステートのステート遷移処理。
		ProcessDownStateTransition();
		break;
	case enPlayerState_Fall:
		//落下ステートのステート遷移処理。
		ProcessFallStateTransition();
		break;
		//クリアステートの時。
	case enPlayerState_Winner:
		//クリアステートのステート遷移処理。
		ProcessWinnerStateTransition();
		break;
	//	//クリアステートの時。
	//case enPlayerState_Clear:
	//	//クリアステートのステート遷移処理。
	//	ProcessClearStateTransition();
	//	break;
	}
}

void Player::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_playerState)
	{
		//待機ステートの時。
	case enPlayerState_Idle:
		//待機アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
		//走りステートの時。
	case enPlayerState_Run:
		m_modelRender.SetAnimationSpeed(1.2f);
		//走りアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//攻撃ステートの時。
	case enPlayerState_Attack:
		m_modelRender.SetAnimationSpeed(0.9f);
		//攻撃アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
		//魔法攻撃ステートの時。
	case enPlayerState_Mahou:
		m_modelRender.SetAnimationSpeed(1.2f);
		//魔法攻撃アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Mahou, 0.1f);
		break;
		//被ダメージステートの時。
	case enPlayerState_ReceiveDamage:
		//被ダメージアニメーションを再生。
		m_modelRender.SetAnimationSpeed(0.75f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
		//ダウンステートの時。
	case enPlayerState_Death:
		//ダウンアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Death, 0.1f);
		break;
	case enPlayerState_Fall:
		//落下アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Fall, 0.5f);
		break;
		//クリアステートの時。
	case enPlayerState_Winner:
		//クリアアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Winner, 0.1f);
		break;
	default:
		break;
	}
}

void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//キーの名前が「attack_start」の時。
	if (wcscmp(eventName, L"attack_start") == 0) {
		//斬撃エフェクトを作成する。
		MakeSlashingEffect();
		//攻撃中にする。
		m_isUnderAttack = true;
	}
	//キーの名前が「attack_end」の時。
	else if (wcscmp(eventName, L"attack_end") == 0) {
		//攻撃を終わる。
		m_isUnderAttack = false;
	}
	//キーの名前が「magic_attack」の時。
	else if (wcscmp(eventName, L"mahou_start") == 0) {
		//ファイヤーボールを作成する。
		MakeMahou();
	}
}

void Player::Render(RenderContext& rc)
{
	//モデルを描画する。
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
//各サウンドをロードする。
