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
	//BGM���폜����B
	DeleteGO(m_bgm);
}

bool Title::Start()
{
	//�摜��ǂݍ��ށB
	m_spriteRender.Init("Assets/sprite/title.dds", 1920, 1080);
	m_ken.Init("Assets/sprite/ken.dds", 64, 16);
	//m_pressButton.Init("Assets/sprite/button.dds", 1293, 646);
	//m_pressButton.SetPosition(Vector3(0.0f, -220.0f, 0.0f));

	//����ǂݍ��ށB
	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/tb.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/button.wav");
	g_soundEngine->ResistWaveFileBank(10, "Assets/sound/peji.wav");

	//BGM�B
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
		//������ʂŉE�{�^���ŉE�s��
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

		//������ʂō��{�^���ō��s��
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
				//���g���폜����B
				DeleteGO(this);
			}
		}
	

	m_pressButton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));
	//�摜�̍X�V�B
	m_spriteRender.Update();
	//m_pressButton.Update();
	m_ken.Update();
	//���l��ω�������B
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
		//���ʉ����Đ�����B
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(9);
		se->Play(false);
		se->SetVolume(0.6f);
	}
}



void Title::Render(RenderContext& rc)
{
	//�摜�̕`��B
	m_spriteRender.Draw(rc);
	//m_pressButton.Draw(rc);
	m_ken.Draw(rc);
	//�����̉�ʂɐ؂�ւ������
	if (setume == 1)
	{          //�y�[�W��1���ڂ̎�
	       if (pe_c == 0)
			{
				m_setumeRender1.Draw(rc);
		    }  //�y�[�W��2���ڂ̎�
		   else if (pe_c == 1)
		   {
			   m_setumeRender2.Draw(rc);
		   }   //�y�[�W��3���ڂ̎�
		  
		
	}
}