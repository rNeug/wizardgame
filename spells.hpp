#ifndef SPELLS_HPP_
#define SPELLS_HPP_

#include <list>

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"
#include "hp.hpp"

class OpenGLWindow;

class Spells {
public:
  void create(GLuint program);
  void paint();
  void destroy();
  void update(HP &hp, const GameData &gameData, float deltaTime);

  struct spells {
    bool m_dead{};
    glm::vec2 m_translation{};
    glm::vec2 m_velocity{};
  };

  std::list<spells> m_spells;

  float m_scale{0.015f};

private:
  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_VAO{};
  GLuint m_VBO{};
};

#endif