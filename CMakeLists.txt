project(WizardGame)
add_executable(${PROJECT_NAME} main.cpp window.cpp wizards.cpp spells.cpp
                               hp.cpp starlayers.cpp)
enable_abcg(${PROJECT_NAME})
