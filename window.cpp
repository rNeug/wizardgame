#include "window.hpp"
#include "abcgOpenGL.hpp"

void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
    bool keyDown = event.type == SDL_KEYDOWN;
    switch (event.key.keysym.sym) {
      case SDLK_UP:
      case SDLK_w:
        m_hp.m_velocity.y = keyDown ? 1.0f : 0.0f;
        break;
      case SDLK_DOWN:
      case SDLK_s:
        m_hp.m_velocity.y = keyDown ? -1.0f : 0.0f;
        break;
      case SDLK_LEFT:
      case SDLK_a:
        m_hp.m_velocity.x = keyDown ? -1.0f : 0.0f;
        break;
      case SDLK_RIGHT:
      case SDLK_d:
        m_hp.m_velocity.x = keyDown ? 1.0f : 0.0f;
        break;
      default:
        break;
    }

    if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_1) {
        m_hp.m_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); 
    } 
    if (event.key.keysym.sym == SDLK_2) {
        m_hp.m_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);  
    }
    if (event.key.keysym.sym == SDLK_3) {
        m_hp.m_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); 
    }
    if (event.key.keysym.sym == SDLK_4) {
        m_hp.m_color = glm::vec4(0.0f, 250.0f, 250.0f, 1.0f); 
    }
    
}

  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{ mousePosition.x - m_viewportSize.x / 2,
                         -(mousePosition.y - m_viewportSize.y / 2) };

    m_hp.m_rotation = std::atan2(direction.y, direction.x);
}

}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Create program to render the stars
  m_starsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "stars.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "stars.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 150, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;

  m_starLayers.create(m_starsProgram, 25);
  m_hp.create(m_objectsProgram);
  m_wizards.create(m_objectsProgram, 3);
  m_spells.create(m_objectsProgram);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_hp.update(m_gameData, deltaTime);
  m_starLayers.update(m_hp, deltaTime);
  m_wizards.update(m_hp, deltaTime);
  m_spells.update(m_hp, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_starLayers.paint();
  m_wizards.paint();
  m_spells.paint();
  m_hp.paint(m_gameData);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_starsProgram);
  abcg::glDeleteProgram(m_objectsProgram);

  m_wizards.destroy();
  m_spells.destroy();
  m_hp.destroy();
  m_starLayers.destroy();
}

void Window::checkCollisions() {
  // Check collision between ship and asteroids
  for (auto const &wizard : m_wizards.m_wizards) {
    auto const wizardsTranslation{wizard.m_translation};
    auto const distance{
        glm::distance(m_hp.m_translation, wizardsTranslation)};

    if (distance < m_hp.m_scale * 0.9f + wizard.m_scale * 0.85f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }
  }

  for (auto &spells : m_spells.m_spells) {
    if (spells.m_dead)
      continue;

    for (auto &wizard : m_wizards.m_wizards) {
      for (auto const i : {-2, 0, 2}) {
        for (auto const j : {-2, 0, 2}) {
          auto const wizardsTranslation{wizard.m_translation +
                                         glm::vec2(i, j)};
          auto const distance{
              glm::distance(spells.m_translation, wizardsTranslation)};

          if (distance < m_spells.m_scale + wizard.m_scale * 0.85f) {
            wizard.m_hit = true;
            spells.m_dead = true;
          }
        }
      }
    }

    // Break asteroids marked as hit
    for (auto const &wizard : m_wizards.m_wizards) {
      if (wizard.m_hit && wizard.m_scale > 0.10f) {
        std::uniform_real_distribution randomDist{-1.0f, 1.0f};
        std::generate_n(std::back_inserter(m_wizards.m_wizards), 3, [&]() {
          glm::vec2 const offset{randomDist(m_randomEngine),
                                 randomDist(m_randomEngine)};
          auto const newScale{wizard.m_scale * 0.5f};
          return m_wizards.makeWizard(
              wizard.m_translation + offset * newScale, newScale);
        });
      }
    }

    m_wizards.m_wizards.remove_if([](auto const &a) { return a.m_hit; });
  }
}

void Window::checkWinCondition() {
  if (m_wizards.m_wizards.empty()) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}