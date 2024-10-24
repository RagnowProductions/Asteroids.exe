#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

const float PI = 3.14159265f;

class Bullet {
public:
    sf::CircleShape shape;
    float speed;
    sf::Vector2f direction;

    Bullet(float x, float y, float angle) {
        shape.setRadius(2);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(x, y);
        speed = 10.f;
        direction = sf::Vector2f(std::cos(angle), std::sin(angle));
    }

    void update() {
        shape.move(direction * speed);
    }
};

class Asteroid {
public:
    sf::CircleShape shape;
    float size;

    Asteroid(float x, float y, float size) {
        this->size = size;
        shape.setRadius(size);
        shape.setFillColor(sf::Color::White);
        shape.setPosition(x, y);
    }

    void split(std::vector<Asteroid>& asteroids) {
        if (size > 5) { // Split if size is larger than 5
            asteroids.emplace_back(shape.getPosition().x, shape.getPosition().y, size / 2);
            asteroids.emplace_back(shape.getPosition().x, shape.getPosition().y, size / 2);
        }
    }
};

class Player {
public:
    sf::CircleShape shape;
    float rotationSpeed;

    Player() {
        shape.setPointCount(3); // Triangle
        shape.setRadius(20);
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin(20, 20);
        shape.setPosition(400, 300);
        rotationSpeed = 5.f;
    }

    void rotate(float angle) {
        shape.rotate(angle);
    }

    void move(float deltaTime) {
        float angle = shape.getRotation() * PI / 180; // Convert to radians
        shape.move(std::cos(angle) * 100.f * deltaTime, std::sin(angle) * 100.f * deltaTime);
    }

    void shrink() {
        shape.setRadius(shape.getRadius() - 1);
        shape.setOrigin(shape.getRadius(), shape.getRadius());
    }

    void grow() {
        shape.setRadius(shape.getRadius() + 1);
        shape.setOrigin(shape.getRadius(), shape.getRadius());
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Asteroids Game");
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Asteroid> asteroids;

    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Spawn 10 asteroids at random locations
    for (int i = 0; i < 10; ++i) {
        float x = std::rand() % 800;
        float y = std::rand() % 600;
        asteroids.emplace_back(x, y, 20);
    }

    sf::Clock bulletClock;
    float bulletDuration = 2.f; // Bullet lifetime in seconds

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Player controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.rotate(-player.rotationSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.rotate(player.rotationSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            player.move(1.f / 60.f); // Move forward
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            player.shrink();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            // Shoot a bullet
            float angle = player.shape.getRotation() * PI / 180; // Convert to radians
            bullets.emplace_back(player.shape.getPosition().x, player.shape.getPosition().y, angle);
        }

        // Update bullets
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->update();
            if (bulletClock.getElapsedTime().asSeconds() > bulletDuration) {
                it = bullets.erase(it); // Remove bullets after 2 seconds
            } else {
                ++it;
            }
        }

        // Collision detection and asteroid splitting
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            bool bulletHit = false;
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                if (bulletIt->shape.getGlobalBounds().intersects(asteroidIt->shape.getGlobalBounds())) {
                    asteroidIt->split(asteroids);
                    bulletHit = true;
                    asteroidIt = asteroids.erase(asteroidIt); // Remove the asteroid
                    break; // Exit the loop after hitting one asteroid
                } else {
                    ++asteroidIt;
                }
            }
            if (bulletHit) {
                bulletIt = bullets.erase(bulletIt); // Remove bullet if hit
            } else {
                ++bulletIt;
            }
        }

        // Clear the window
        window.clear();

        // Draw everything
        window.draw(player.shape);
        for (const auto& bullet : bullets) {
            window.draw(bullet.shape);
        }
        for (const auto& asteroid : asteroids) {
            window.draw(asteroid.shape);
        }

        // Display the contents of the window
        window.display();
    }

    return 0;
}
