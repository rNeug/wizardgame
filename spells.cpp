#include "spells.hpp"

#include <glm/gtx/rotate_vector.hpp>

void Spells::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  m_spells.clear();

  // Create geometry data
  auto const sides{10};
  std::vector<glm::vec2> positions{{0, 0}};
  auto const step{M_PI * 2 / sides};
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Spells::paint() {
  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);
  abcg::glUniform4f(m_colorLoc, 0, 1, 0, 1);
  abcg::glUniform1f(m_rotationLoc, 0);
  abcg::glUniform1f(m_scaleLoc, m_scale);

  for (auto const &spells : m_spells) {
    abcg::glUniform2f(m_translationLoc, spells.m_translation.x,
                      spells.m_translation.y);

    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
  }

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Spells::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Spells::update(HP &hp, const GameData &gameData, float deltaTime) {
  if (gameData.m_input[gsl::narrow<size_t>(Input::Fire)] &&
      gameData.m_state == State::Playing) {
    if (hp.m_spellsCoolDownTimer.elapsed() > 250.0 / 1000.0) {
      hp.m_spellsCoolDownTimer.restart();

      auto const forward{glm::rotate(glm::vec2{1.0f, 0.0f}, hp.m_rotation)};
      auto const right{glm::rotate(glm::vec2{1.0f, 0.0f}, hp.m_rotation)};
      auto const cannonOffset{(11.0f / 15.5f) * hp.m_scale};
      auto const spellsSpeed{2.0f};

      spells spells{.m_dead = false,
                    .m_translation = hp.m_translation + right * cannonOffset,
                    .m_velocity = hp.m_velocity + forward * spellsSpeed};
      m_spells.push_back(spells);

      hp.m_velocity -= forward * 0.1f;
    }
  }

  for (auto &spells : m_spells) {
    spells.m_translation -= hp.m_velocity * deltaTime;
    spells.m_translation += spells.m_velocity * deltaTime;
    if (spells.m_translation.x < -1.1f)
      spells.m_dead = true;
    if (spells.m_translation.x > +1.1f)
      spells.m_dead = true;
    if (spells.m_translation.y < -1.1f)
      spells.m_dead = true;
    if (spells.m_translation.y > +1.1f)
      spells.m_dead = true;
  }

  m_spells.remove_if([](auto const &p) { return p.m_dead; });
}