#include "wizards.hpp"

#include <glm/gtx/fast_trigonometry.hpp>

void Wizards::create(GLuint program, int quantity) {
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_wizards.clear();
  m_wizards.resize(quantity);

  for (auto &wizards : m_wizards) {
    wizards = makeWizards();

    do {
      Wizard.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(Wizard.m_translation) < 0.5f);
  }
}

void Wizards::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &wizards : m_wizards) {
    abcg::glBindVertexArray(Wizard.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &wizard.m_color.r);
    abcg::glUniform1f(m_scaleLoc, wizard.m_scale);
    abcg::glUniform1f(m_rotationLoc, wizard.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, wizard.m_translation.x + j,
                          wizard.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, wizard.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Wizards::destroy() {
  for (auto &wizards : m_wizards) {
    abcg::glDeleteBuffers(1, &wizards.m_VBO);
    abcg::glDeleteVertexArrays(1, &wizards.m_VAO);
  }
}

void Wizards::update(const HP &hp, float deltaTime) {
  for (auto &wizard : m_wizards) {
    wizard.m_translationLoc -= hp.m_velocity * deltaTime;
    wizard.m_rotationLoc = glm::wrapAngle(
        wizard.m_rotationLoc + wizard.m_angularVelocity * deltaTime);
    wizard.m_translationLoc += wizard.m_velocity * deltaTime;

    // Wrap-around
    if (wizard.m_translationLoc.x < -1.0f)
      wizard.m_translationLoc.x += 2.0f;
    if (wizard.m_translationLoc.x > +1.0f)
      wizard.m_translationLoc.x -= 2.0f;
    if (wizard.m_translationLoc.y < -1.0f)
      wizard.m_translationLoc.y += 2.0f;
    if (wizard.m_translationLoc.y > +1.0f)
      wizard.m_translationLoc.y -= 2.0f;
  }
}

Wizards::Wizard Wizards::makeWizards(glm::vec2 translation, float scale) {
  Wizard wizards;

  auto &re{m_randomEngine}; // Shortcut

  // Randomly pick the number of sides
  std::uniform_int_distribution randomSides(4, 10);
  wizards.m_polygonSides = randomSides(re);

  // Get a random color (blue, green, red, or yellow)
  std::uniform_int_distribution randomColor(0, 3);
  glm::vec4 color;

  // Randomly select one of the four colors
  switch (randomColor(re)) {
    case 0: // Blue
      color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
      break;
    case 1: // Green
      color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
      break;
    case 2: // Red
      color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
      break;
    case 3: // Yellow
      color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
      break;
  }
  wizards.m_color = color;
  wizards.m_rotation = 0.0f;
  wizards.m_scale = scale;
  wizards.m_translation = translation;

  // Get a random angular velocity
  wizards.m_angularVelocity = m_randomDist(re);

  // Get a random direction
  glm::vec2 const direction{m_randomDist(re), m_randomDist(re)};
  wizards.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry data
  std::vector<glm::vec2> positions;

  // Head
  positions.emplace_back(0.0f, 0.0f);
  positions.emplace_back(0.3f, 0.25f);
  positions.emplace_back(0.0f, 0.5f);

  // Body
  positions.emplace_back(0.15f, -0.25f);
  positions.emplace_back(0.15f, -0.75f);
  positions.emplace_back(-0.5f, -0.75f);
  positions.emplace_back(-0.5f, -0.25f);

  // Army
  positions.emplace_back(0.6f, -0.15f);
  positions.emplace_back(0.7f, -0.3f);
  positions.emplace_back(0.7f, 0.0f);
  positions.emplace_back(0.8f, -0.15f);
  positions.push_back(positions.at(0));

  // Generate VBO
  abcg::glGenBuffers(1, &wizards.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, Wizards.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &wizards.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(wizards.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, wizards.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return wizards;
}

