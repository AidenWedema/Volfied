#include "Game.h"
#include "Player.h"
#include "Score.hpp"
#include "Debug.hpp"

void Game::Start()
{
    window.create(sf::VideoMode({ 800, 600 }), "Volfied", sf::Style::Close);
    level = -1;
    SceneManager::GetInstance()->LoadScene("Main Menu");

	//RNG::seed = std::chrono::system_clock::now().time_since_epoch().count();

    Time* time = Time::GetInstance();
    time->SetFps(60);
    time = nullptr;
    delete time;
    while (true)
    {
        switch (gameState)
        {
        case Game::CLOSE:
            return;
        case Game::MENU:
            Run();
            break;
        case Game::GAME:
            Run();
            break;
        case Game::END:
            level = -1;
            Player::lives = 5;
            Score::Reset();
            SceneManager::GetInstance()->LoadScene("Main Menu");
            gameState = GAME;
            break;
        case Game::WIN:
            break;
        default:
            gameState = Game::MENU;
            break;
        }
    }
}

void Game::Run()
{
    SceneManager* sceneManager = SceneManager::GetInstance();
    Time* time = Time::GetInstance();

    while (window.isOpen() && running)
    {
		if (gameState != Game::GAME)
			return;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                gameState = Game::CLOSE;
            }
            if (event.type == sf::Event::Resized)
                window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
        }
        auto start = std::chrono::high_resolution_clock::now();
        window.clear();

        sceneManager->SwapSceneIfAvailable();

		// Poll input
		Input::GetInstance()->Update();

        // Check for Reset input
        if (Input::GetInstance()->GetKey("Reset"))
        {
            std::string currentSceneName = sceneManager->GetActiveScene()->name;
            sceneManager->LoadScene(currentSceneName);
        }



		
        // Cycle the RNG
        RNG::GetRand();

		Scene* activeScene = sceneManager->GetActiveScene();
        if (activeScene != nullptr)
        {
            // Update objects
            activeScene->Update();

            // Draw objects
            activeScene->Draw(window);
        }

        // Debug draw
        Debug::Draw();

        // spin until minimum delta time has passed
#ifdef _DEBUG
        auto spinstart = std::chrono::high_resolution_clock::now();
#endif
        time->Spin(start);
#ifdef _DEBUG
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - spinstart);
        float spintime = duration.count() / 1000000.0f;
        std::string debugString = "FPS: " + std::to_string(1.0f / time->GetDeltaTime()) + "\nDelta Time: " + std::to_string(time->GetDeltaTime())
            + "\nSpin Time: " + std::to_string(spintime) + "\nTime spent spinning: " + std::to_string(spintime / time->GetDeltaTime() * 100.0f) +
            "%\nFrame Count: " + std::to_string(time->GetFrameCount()) + "\nRNG: " + std::to_string(RNG::seed);
        Debug::DrawText(debugString, Vector2(10, 10));
        Debug::Draw();
#endif
        window.display();
    }
}
