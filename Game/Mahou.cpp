#include "stdafx.h"
#include "Mahou.h"
#include "Game.h"
#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Mahou::Mahou()
{

}

Mahou::~Mahou()
{
	//エフェクトの再生を停止する。
	m_effectEmitter->Stop();
	//色々削除。
	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool Mahou::Start()
{

	switch (m_zokusei) {

	case enZokusei_hi:
		//エフェクトのオブジェクトを作成する。
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enMagic_fire);
		break;
	case enZokusei_kaze:
		//エフェクトのオブジェクトを作成する。
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enDaburuinnpakuto);
		break;
	case enZokusei_mizu:
		//エフェクトのオブジェクトを作成する。
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(enWater);
		break;
	}

	//エフェクトの大きさを設定する。
	m_effectEmitter->SetScale(m_scale * 15.0f);
	//移動速度を計算。
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_position += m_moveSpeed * 50.0f;
	m_moveSpeed *= 600.0f;
	m_rotation.AddRotationDegY(360.0f);
	//回転を設定する。
	m_effectEmitter->SetRotation(m_rotation);
	m_effectEmitter->Play();

	//コリジョンオブジェクトを作成する。
	m_collisionObject = NewGO<CollisionObject>(0);
	//球状のコリジョンを作成する。
	m_collisionObject->CreateSphere(m_position, Quaternion::Identity, 35.0f * m_scale.z);
	//術者がプレイヤーだったら。
	if (m_enMagician == enMagician_Player)
	{
		//名前をplayer_fireballにする。
		switch (m_zokusei) {
		case enZokusei_hi:
			m_collisionObject->SetName("player_fireball");
			break;
		case enZokusei_kaze:
			m_collisionObject->SetName("player_kaze");
			break;
		case enZokusei_mizu:
			m_collisionObject->SetName("player_mizu");
			break;
		}
		
	}
	//術者がBossだったら。
	else if (m_enMagician == enMagician_Boss)
	{
		//名前をenemy_fireballにする。
		m_collisionObject->SetName("enemy_fireball");
	}

	//懲り所オブジェクトが自動で削除されないようにする。
	//m_collisionObject->SetIsEnableAutoDelete(false);
	m_collisionObject->SetTimeLimit(1.0f);
	//音を読み込む。
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/fire_cut_cut.wav");
	//効果音を再生する。
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(1);
	se->Play(false);
	se->SetVolume(1.5f);
	return true;
}

void Mahou::Update()
{

	switch (m_zokusei) {

	case enZokusei_hi:
		//座標を移動させる。
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//エフェクトの座標を設定する。
		m_effectEmitter->SetPosition(m_position);
		//コリジョンオブジェクトに座標を設定する。
		m_collisionObject->SetPosition(m_position);

		//タイマーを加算する。
		m_timer += g_gameTime->GetFrameDeltaTime();

		//タイマーが0.7f以上だったら。
		if (m_timer >= 0.7f)
		{
			//自身を削除する。
			DeleteGO(this);
		}
		break;
	case enZokusei_kaze:
		//座標を移動させる。
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//エフェクトの座標を設定する。
		m_effectEmitter->SetPosition(m_position);
		//コリジョンオブジェクトに座標を設定する。
		m_collisionObject->SetPosition(m_position);

		//タイマーを加算する。
		m_timer += g_gameTime->GetFrameDeltaTime();

		//タイマーが0.7f以上だったら。
		if (m_timer >= 2.5f)
		{
			//自身を削除する。
			DeleteGO(this);
		}
		break;
	case enZokusei_mizu:
		//座標を移動させる。
		m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();
		//エフェクトの座標を設定する。
		m_effectEmitter->SetPosition(m_position);
		//コリジョンオブジェクトに座標を設定する。
		m_collisionObject->SetPosition(m_position);

		//タイマーを加算する。
		m_timer += g_gameTime->GetFrameDeltaTime();

		//タイマーが0.7f以上だったら。
		if (m_timer >=1.5f)
		{
			//自身を削除する。
			DeleteGO(this);
		}
		break;
	}

	
}