#pragma once

#include <ctime>
#include <charconv>

#include "State.hpp"
#include "Game.hpp"

class SplashState : public State
{
  public:
    explicit SplashState(const std::shared_ptr<GameData>& data, const AppCUI::Controls::SingleApp* app);

    SplashState(const SplashState& other)     = default;
    SplashState(SplashState&& other) noexcept = default;
    SplashState& operator=(const SplashState& other) = default;
    SplashState& operator=(SplashState&& other) noexcept = default;

    ~SplashState();

    void Init() override;

    bool HandleEvent(AppCUI::Controls::Control* ctrl, AppCUI::Controls::Event eventType, int controlID) override;
    bool Update() override;
    void Draw(AppCUI::Graphics::Renderer& renderer) override;

    void Pause() override;
    void Resume() override;

  private:
    const std::shared_ptr<GameData>& data;

    clock_t initialTime;

    const int stateDuration = 3; // seconds

    AppCUI::Controls::SingleApp* app = nullptr; // to be passed further

    AppCUI::Utils::Reference<AppCUI::Controls::Label> label = nullptr;
    AppCUI::Utils::Reference<AppCUI::Controls::Label> labelCounter = nullptr;

    static const std::string GetTextFromNumber(const unsigned long long value);
};
