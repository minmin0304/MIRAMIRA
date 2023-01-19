#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "BackGround.h"
#include "sound/SoundEngine.h"
#include "Boss.h"
#include "Title.h"
#include "Fade.h"


Game::Game()
{
    m_gameclear.Init("Assets/sprite/gamekuria.dds", 1920.0f, 1080.0f);
}

Game::~Game()
{

    //�F�X�ȃI�u�W�F�N�g���폜����B
    DeleteGO(m_gameCamera);
    DeleteGO(m_skyCube);
    DeleteGO(m_player);
    DeleteGO(m_background);
    DeleteGO(FindGO<Boss>("boss"));
    DeleteGO(m_bgm);
}

bool Game::Start()
{
    //�G�t�F�N�g��ǂݍ��ށB
    EffectEngine::GetInstance()->ResistEffect(enMagic_fire, u"Assets/effect/efk/magic_fire.efk");
    EffectEngine::GetInstance()->ResistEffect(enSlash, u"Assets/effect/efk/enemy_slash_01.efk");
    EffectEngine::GetInstance()->ResistEffect(enInazuma, u"Assets/effect/efk/inazuma.efk");
    EffectEngine::GetInstance()->ResistEffect(enDaburuinnpakuto, u"Assets/effect/efk/daburuinnpakuto.efk");
    EffectEngine::GetInstance()->ResistEffect(enCast_fire, u"Assets/effect/efk/cast_fire.efk");
    EffectEngine::GetInstance()->ResistEffect(enWater, u"Assets/effect/efk/Water.efk");


    g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
    g_camera3D->SetFar(200000.0f);

    //��̃I�u�W�F�N�g���쐬����B
    m_skyCube = NewGO<SkyCube>(0, "skycube");

    g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 0.1f);
    g_renderingEngine->SetCascadeNearAreaRates(0.01f, 0.1f, 0.5f);
    //���x�����\�z����B
    m_levelRender.Init("Assets/level3D/level.tkl",
        [&](LevelObjectData& objData)
        {
            //���O��human��������B
            if (objData.EqualObjectName(L"Player") == true)
            {

                //�v���C���[�̃I�u�W�F�N�g���쐬����B
                m_player = NewGO<Player>(0, "player");
                m_player->m_game = this;
                //���W��ݒ肷��B
                
                m_player->SetPosition(objData.position);
                //��]��ݒ肷��B
                m_player->SetRotation(objData.rotation);
                return true;
            }
            //���O��background��������B
            else if (objData.EqualObjectName(L"map") == true) {
                //�w�i�I�u�W�F�N�g���쐬����B
                m_background = NewGO<Background>(0, "background");
                //���W��ݒ肷��B
                m_background->SetPosition(objData.position);
                //�傫����ݒ肷��B
                m_background->SetScale(objData.scale);
                //��]��ݒ肷��B
                m_background->SetRotation(objData.rotation);
                return true;
            }
            
            //���O��boss��������B
            else if (objData.EqualObjectName(L"Boss") == true)
            {
                //�{�X�̃I�u�W�F�N�g���쐬����B
               Boss* boss = NewGO<Boss>(0, "boss");
                //���W��ݒ肷��B
                boss->SetPosition(objData.position);
                //��]��ݒ肷��B
                boss->SetRotation(objData.rotation);
                //�{�X��HP�͍��߂ɐݒ肷��B
                //HP��15�ɐݒ肷��B
                boss->SetHP(15);
                //�쐬�����G�l�~�[�̘a�𐔂������̂ŁA+1����
                return true;
            }
            return true;
        });
    //�����蔻���L��������B
    PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
   
    //�J�����̃I�u�W�F�N�g���쐬����B
    m_gameCamera = NewGO<GameCamera>(0, "gamecamera");


    //����ǂݍ��ށB
    g_soundEngine->ResistWaveFileBank(2, "Assets/sound/main.wav");
    //BGM���Đ�����B
    m_bgm = NewGO<SoundSource>(0);
    m_bgm->Init(2);
    m_bgm->Play(true);
    m_bgm->SetVolume(0.2f);

    m_fade = FindGO<Fade>("fade");
    m_fade->StartFadeIn();
    return true;
}

void Game::NotifyGameClear()
{
   /* m_isWaitFadeout = true;
    m_fade->StartFadeOut();*/
}

void Game::NotifyGameOver()
{
    m_isWaitFadeout = true;
    m_fade->StartFadeOut();
}

void Game::Update()
{


    if (m_bossdie == true) {
        m_gameclear.SetPosition(Vector3(0.0f, 1.1f, 0.0f));
        m_gameclear.Update();
        if (g_pad[0]->IsTrigger(enButtonA))
        {
            /*if (m_isWaitFadeout) {
                if (!m_fade->IsFade()) {*/
                    NewGO<Title>(0, "title");
                    DeleteGO(this);
             /*   }
            }*/
        }
    }
}
void Game::Render(RenderContext& rc)
{
    
    if (m_bossdie == true) {
        m_gameclear.SetPosition(Vector3(0.0f, 1.1f, 0.0f));
        m_gameclear.Update();
        m_gameclear.Draw(rc);
    }

}