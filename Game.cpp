#include "Game.h"

Game* Game::instance = nullptr;
Game* Game::GetInstance()
{
	if (instance == nullptr)
		instance = new Game();
	return instance;
}

void Game::Start()
{
    window.create(sf::VideoMode({800, 600}), "Pac-man", sf::Style::Close);
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
#ifdef _DEBUG
    sf::Font dfont;
    sf::Text fpsText;
    // Load the Arial font from the assets folder
    if (!dfont.loadFromFile("assets/fonts/Arial.ttf")) {
        // Handle error if the font fails to load
        std::cerr << "Error loading Arial font from assets/fonts/Arial.ttf" << std::endl;
        return;
    }

    // Initialize the text object for displaying FPS
    fpsText.setFont(dfont);
    fpsText.setCharacterSize(14);           // Font size
    fpsText.setFillColor(sf::Color::White); // Text color
    fpsText.setPosition(10, 10);            // Position on screen (top-left corner)
#endif
    Time* time = Time::GetInstance();
    while (window.isOpen() && running)
    {
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

        // Update objects


        // Draw objects
        

        // spin until minimum delta time has passed
#ifdef _DEBUG
        auto spinstart = std::chrono::high_resolution_clock::now();
#endif
        time->Spin(start);
#ifdef _DEBUG
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - spinstart);
        float spintime = duration.count() / 1000000.0f;
        fpsText.setString("FPS: " + std::to_string(1.0f / time->GetDeltaTime()) + "\nDelta Time: " + std::to_string(time->GetDeltaTime())
            + "\nSpin Time: " + std::to_string(spintime) + "\nTime spent spinning: " +
            std::to_string(spintime / time->GetDeltaTime() * 100.0f) + "%" + "\nFrame Count: " + std::to_string(time->GetFrameCount()));
        window.draw(fpsText);
#endif
        window.display();
    }
}
