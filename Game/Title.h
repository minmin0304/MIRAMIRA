#pragma once

//�N���X�錾�B
class Fade;

/// <summary>
/// �^�C�g���B
/// </summary>
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Setume();
	void TitleStart();
	void Render(RenderContext& rc);
private:
	SpriteRender			m_spriteRender;					//�^�C�g���̉摜�B
	float					m_alpha = 0.0f;					//pressbutton�̃��l�B
	SpriteRender			m_pressButton;					//pressbutton�̉摜�B
	SpriteRender            m_setumeRender1;                //������ʂ̃y�[�W1
	SpriteRender            m_setumeRender2;                //������ʂ̃y�[�W2
	SoundSource*            m_bgm = nullptr;				//�^�C�g����BGM�B
	Fade*                   m_fade = nullptr;				//�t�F�[�h�B
	bool					m_isWaitFadeout = false;        //Fade
	int                     setume = 0;                     //������ʂ�\������
	int                     pe_c = 0;                       //������ʂ̃y�[�W�؂�ւ�
	int						pe_max = 1;                     //������ʂ̃y�[�W�̍ő吔
	int						pe_min = 0;                     //������ʂ̃y�[�W�̍ŏ���
	SpriteRender            m_ken;                          //�������ʂ̌��̉摜
	//�v���C���[�̃X�e�[�g�B
	enum EnTitleState {
		enStart,
		enTutorial
	};
	EnTitleState m_titleState=enStart;
};

