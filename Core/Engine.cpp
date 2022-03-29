//
// Created by nedo on 12.03.2022.
//

#include "Engine.hpp"
#include "Logger.hpp"

#include <imgui-SFML.h>

Engine::Engine(unsigned int width, unsigned int height, const std::string &title, unsigned int style) {
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), title, style);
	window->setFramerateLimit(60);
	SPDLOG_INFO("Created new window. Size: {}x{} | Title: \"{}\" | Style: {}", width, height, title, style);

	resourceManager = std::make_shared<ResourceManager>("Resources");
	SPDLOG_DEBUG("Resource Manager initialized");

	auto configFile = resourceManager->GetDocument("Config");
	if(configFile.empty())
	{
		auto path = (std::filesystem::path(resourceManager->GetResourceDirectory()) / "Documents" / "Config.xml").string();
		config = Config(path);
		config.Create();
		SPDLOG_DEBUG("Created config ({})", path);
	}
	else
	{
		config = Config(configFile);
		config.Load();
		SPDLOG_DEBUG("Loaded config");
	}

	ImGui::SFML::Init(*window);
	SPDLOG_DEBUG("ImGui-SFML initialized");
}

Engine::~Engine() {
	ImGui::SFML::Shutdown();
	SPDLOG_DEBUG("ImGui-SFML disabled");
}

const std::shared_ptr<ResourceManager> &Engine::GetResourceManager() const {
	return resourceManager;
}

void Engine::Tick() {
	ProcessStateMachine();

	/*if(!state) {
		SPDLOG_CRITICAL("Cannot run the engine without a state!");
		std::exit(1);
	}*/

	sf::Event event{};
	while(window->pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if(event.type == sf::Event::Closed)
		{
			window->close();
		}
	}
	auto delta = deltaClock.restart();
	ImGui::SFML::Update(*window, delta);

	if(!states.empty())
		GetActiveState()->Update(delta);

	window->clear(backgroundColor);
	if(!states.empty())
		window->draw(*GetActiveState());

	ImGui::SFML::Render(*window);
	window->display();
}

sf::Vector2f Engine::GetViewSize() const {
	return window->getView().getSize();
}

void Engine::SetBackgroundColor(const sf::Color &backgroundColor) {
	Engine::backgroundColor = backgroundColor;
}

Config &Engine::GetConfig() {
	return config;
}

std::unique_ptr<sf::RenderWindow> &Engine::GetWindow() {
	return window;
}