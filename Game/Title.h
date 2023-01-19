#pragma once

//クラス宣言。
class Fade;

/// <summary>
/// タイトル。
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
	SpriteRender			m_spriteRender;					//タイトルの画像。
	float					m_alpha = 0.0f;					//pressbuttonのα値。
	SpriteRender			m_pressButton;					//pressbuttonの画像。
	SpriteRender            m_setumeRender1;                //説明画面のページ1
	SpriteRender            m_setumeRender2;                //説明画面のページ2
	SoundSource*            m_bgm = nullptr;				//タイトルのBGM。
	Fade*                   m_fade = nullptr;				//フェード。
	bool					m_isWaitFadeout = false;        //Fade
	int                     setume = 0;                     //説明画面を表示する
	int                     pe_c = 0;                       //説明画面のページ切り替え
	int						pe_max = 1;                     //説明画面のページの最大数
	int						pe_min = 0;                     //説明画面のページの最小数
	SpriteRender            m_ken;                          //説明が面の剣の画像
	//プレイヤーのステート。
	enum EnTitleState {
		enStart,
		enTutorial
	};
	EnTitleState m_titleState=enStart;
};

