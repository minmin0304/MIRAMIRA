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

    //色々なオブジェクトを削除する。
    DeleteGO(m_gameCamera);
    DeleteGO(m_skyCube);
    DeleteGO(m_player);
    DeleteGO(m_background);
    DeleteGO(FindGO<Boss>("boss"));
    DeleteGO(m_bgm);
}

bool Game::Start()
{
    //エフェクトを読み込む。
    EffectEngine::GetInstance()->ResistEffect(enMagic_fire, u"Assets/effect/efk/magic_fire.efk");
    EffectEngine::GetInstance()->ResistEffect(enSlash, u"Assets/effect/efk/enemy_slash_01.efk");
    EffectEngine::GetInstance()->ResistEffect(enInazuma, u"Assets/effect/efk/inazuma.efk");
    EffectEngine::GetInstance()->ResistEffect(enDaburuinnpakuto, u"Assets/effect/efk/daburuinnpakuto.efk");
    EffectEngine::GetInstance()->ResistEffect(enCast_fire, u"Assets/effect/efk/cast_fire.efk");
    EffectEngine::GetInstance()->ResistEffect(enWater, u"Assets/effect/efk/Water.efk");


    g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
    g_camera3D->SetFar(200000.0f);

    //空のオブジェクトを作成する。
    m_skyCube = NewGO<SkyCube>(0, "skycube");

    g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 0.1f);
    g_renderingEngine->SetCascadeNearAreaRates(0.01f, 0.1f, 0.5f);
    //レベルを構築する。
    m_levelRender.Init("Assets/level3D/level.tkl",
        [&](LevelObjectData& objData)
        {
            //名前がhumanだったら。
            if (objData.EqualObjectName(L"Player") == true)
            {

                //プレイヤーのオブジェクトを作成する。
                m_player = NewGO<Player>(0, "player");
                m_player->m_game = this;
                //座標を設定する。
                
                m_player->SetPosition(objData.position);
                //回転を設定する。
                m_player->SetRotation(objData.rotation);
                return true;
            }
            //名前がbackgroundだったら。
            else if (objData.EqualObjectName(L"map") == true) {
                //背景オブジェクトを作成する。
                m_background = NewGO<Background>(0, "background");
                //座標を設定する。
                m_background->SetPosition(objData.position);
                //大きさを設定する。
                m_background->SetScale(objData.scale);
                //回転を設定する。
                m_background->SetRotation(objData.rotation);
                return true;
            }
            
            //名前がbossだったら。
            else if (objData.EqualObjectName(L"Boss") == true)
            {
                //ボスのオブジェクトを作成する。
               Boss* boss = NewGO<Boss>(0, "boss");
                //座標を設定する。
                boss->SetPosition(objData.position);
                //回転を設定する。
                boss->SetRotation(objData.rotation);
                //ボスのHPは高めに設定する。
                //HPを15に設定する。
                boss->SetHP(15);
                //作成したエネミーの和を数えたいので、+1する
                return true;
            }
            return true;
        });
    //当たり判定を有効化する。
    PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
   
    //カメラのオブジェクトを作成する。
    m_gameCamera = NewGO<GameCamera>(0, "gamecamera");


    //音を読み込む。
    g_soundEngine->ResistWaveFileBank(2, "Assets/sound/main.wav");
    //BGMを再生する。
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