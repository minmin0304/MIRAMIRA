#include "stdafx.h"
#include "Title.h"

#include "Game.h"
#include "Fade.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Title::Title()
{
	m_setumeRender1.Init("Assets/sprite/setume1.dds", 1920.0f, 1080.0f);
	m_setumeRender2.Init("Assets/sprite/setume2.dds", 1920.0f, 1080.0f);
	
}

Title::~Title()
{
	//BGMを削除する。
	DeleteGO(m_bgm);
}

bool Title::Start()
{
	//画像を読み込む。
	m_spriteRender.Init("Assets/sprite/title.dds", 1920, 1080);
	m_ken.Init("Assets/sprite/ken.dds", 64, 16);
	//m_pressButton.Init("Assets/sprite/button.dds", 1293, 646);
	//m_pressButton.SetPosition(Vector3(0.0f, -220.0f, 0.0f));

	//音を読み込む。
	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/tb.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/button.wav");
	g_soundEngine->ResistWaveFileBank(10, "Assets/sound/peji.wav");

	//BGM。
	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(8);
	m_bgm->Play(true);
	m_bgm->SetVolume(0.1f);

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();
	return true;
}

void Title::Update()
{
	switch (m_titleState)
	{
	case enStart:
		m_ken.SetPosition(Vector3(-236.0f, -228.0f, 0.0f));
		m_ken.Update();
		break;
	case enTutorial:
		m_ken.SetPosition(Vector3(-236.0f, -342.0f, 0.0f));
		m_ken.Update();
		break;
	default:
		break;
	}
	if (g_pad[0]->IsTrigger(enButtonUp) || g_pad[0]->IsTrigger(enButtonDown))
	{
		if (setume != 0)
		{
			return;
	    }
		if (m_titleState == enStart)
		{
			m_titleState = enTutorial;
			
		}
		else if (m_titleState == enTutorial)
		{
			m_titleState = enStart;
			
		}
	}
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		switch (m_titleState)
		{
		case enStart:
			TitleStart();
			break;
		case enTutorial:
			Setume();
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(9);
			se->Play(false);
			se->SetVolume(0.6f);
			break;
		}
	};
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		setume = 0;
	}
	if (setume)
	{
		//説明画面で右ボタンで右行く
		if (g_pad[0]->IsTrigger(enButtonRight))
		{
			if (pe_c != pe_max)
			{
				pe_c++;

			}
			m_bgm = NewGO<SoundSource>(0);
			m_bgm->Init(10);
			m_bgm->Play(false);
			m_bgm->SetVolume(0.5f);
		}

		//説明画面で左ボタンで左行く
		if (g_pad[0]->IsTrigger(enButtonLeft))
		{
			if (pe_c != pe_min)
			{
				pe_c--;
			}
			m_bgm = NewGO<SoundSource>(0);
			m_bgm->Init(10);
			m_bgm->Play(false);
			m_bgm->SetVolume(0.5f);
		}

		m_setumeRender1.Update();
		m_setumeRender2.Update();
	}
	

		if (m_isWaitFadeout) {
			if (!m_fade->IsFade()) {
				NewGO<Game>(0, "game");
				//自身を削除する。
				DeleteGO(this);
			}
		}
	

	m_pressButton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));
	//画像の更新。
	m_spriteRender.Update();
	//m_pressButton.Update();
	m_ken.Update();
	//α値を変化させる。
	if (m_isWaitFadeout)
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 20.5f;
	}
	else
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	}
}

void Title::Setume()
{
	if (setume == 0)
	{
		pe_c = 0;
		setume = 1;
	}
	
}

void Title::TitleStart()
{
	if (!m_isWaitFadeout) {
		m_isWaitFadeout = true;
		m_fade->StartFadeOut();
		//効果音を再生する。
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(9);
		se->Play(false);
		se->SetVolume(0.6f);
	}
}



void Title::Render(RenderContext& rc)
{
	//画像の描画。
	m_spriteRender.Draw(rc);
	//m_pressButton.Draw(rc);
	m_ken.Draw(rc);
	//説明の画面に切り替わったら
	if (setume == 1)
	{          //ページが1枚目の時
	       if (pe_c == 0)
			{
				m_setumeRender1.Draw(rc);
		    }  //ページが2枚目の時
		   else if (pe_c == 1)
		   {
			   m_setumeRender2.Draw(rc);
		   }   //ページが3枚目の時
		  
		
	}
}