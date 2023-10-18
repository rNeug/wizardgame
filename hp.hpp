#ifndef HP_HPP_
#define HP_HPP_

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"

class HP {
public:
  void create(GLuint program);
  void paint(GameData const &gameData);
  void destroy();
  void update(GameData const &gameData, float deltaTime);

  glm::vec4 m_color{1};
  float m_rotation{};
  float m_scale{0.125f};
  glm::vec2 m_translation{};
  glm::vec2 m_velocity{};

  abcg::Timer m_trailBlinkTimer;
  abcg::Timer m_bulletCoolDownTimer;

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
};
#endif