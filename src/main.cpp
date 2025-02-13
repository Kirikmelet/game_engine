#include "app.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS

int main() {
  game::engine::Application *app = new game::engine::Application("lol");
  return app->run();
}
