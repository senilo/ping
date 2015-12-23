/*
 Anton Olason, 2015-12-22
*/

#include <SFML/Graphics.hpp>
#include <list>
#include <cmath>
#include <fstream>

/*
 Some vector functions
*/
float length(sf::Vector2f &v){
    return std::sqrt(v.x*v.x + v.y*v.y);
}

void set_length(sf::Vector2f &v, float new_length){
    v = v / length(v) * new_length;
}

sf::Vector2f fromPolar(float length, float angle){
    return sf::Vector2f(length*std::cos(angle), length*std::sin(angle));
}

// Set origin to center
void centerTextOrigin(sf::Text &text){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
}

/*
 Här börjar main!
*/
int main()
{
    // Redirect to a file
    std::ofstream file("stderr.log");
    std::streambuf* previous = sf::err().rdbuf(file.rdbuf());

    const int SCREEN_HEIGHT = 200;
    const int SCREEN_WIDTH = (int) (SCREEN_HEIGHT * 1.8);
    const float BALL_SIZE = 10.f;
    const int PADDLE_LENGTH = 50;
    const int PADDLE_THICKNESS = 10;
    const int PADDLE_OFFSET = 10;
    const float PADDLE_SPEED = 3.0f;
    const float INITIAL_BALL_SPEED = 4.f;

    sf::Color table_color(0, 102, 51);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Ping!");
    window.setVerticalSyncEnabled(true);

    sf::RectangleShape ball(sf::Vector2f(BALL_SIZE, BALL_SIZE));
    ball.setOrigin(sf::Vector2f(BALL_SIZE / 2, BALL_SIZE / 2));
    ball.setFillColor(sf::Color::White);

    sf::Vector2f ball_speed(0, 0);

    sf::RectangleShape p1_paddle(sf::Vector2f((float) PADDLE_THICKNESS, (float) PADDLE_LENGTH));
    p1_paddle.setOrigin(sf::Vector2f(PADDLE_THICKNESS / 2, PADDLE_LENGTH / 2));
    p1_paddle.setFillColor(sf::Color::White);
    p1_paddle.setPosition((float) PADDLE_OFFSET, 3 * SCREEN_HEIGHT / 4);

    sf::RectangleShape p2_paddle(sf::Vector2f((float) PADDLE_THICKNESS, (float) PADDLE_LENGTH));
    p2_paddle.setOrigin(sf::Vector2f(PADDLE_THICKNESS / 2, PADDLE_LENGTH / 2));
    p2_paddle.setFillColor(sf::Color::White);
    p2_paddle.setPosition(SCREEN_WIDTH - PADDLE_OFFSET, SCREEN_HEIGHT / 4);

    sf::RectangleShape field_boundary(sf::Vector2f((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
    field_boundary.setOutlineColor(sf::Color::White);
    field_boundary.setOutlineThickness(-4.0);
    field_boundary.setFillColor(sf::Color::Transparent);

    sf::RectangleShape field_net(sf::Vector2f(4, (float) SCREEN_HEIGHT));
    field_net.setPosition(sf::Vector2f(SCREEN_WIDTH / 2 - 2, 0));
    field_net.setFillColor(sf::Color::Black);

    sf::RectangleShape field_center_line(sf::Vector2f((float) SCREEN_WIDTH, 1));
    field_center_line.setPosition(sf::Vector2f(0, SCREEN_HEIGHT / 2 - 0.5));

    std::list<sf::Drawable *> draw_list;
    draw_list.push_back(&field_boundary);
    draw_list.push_back(&field_center_line);
    draw_list.push_back(&field_net);

    draw_list.push_back(&ball);
    draw_list.push_back(&p1_paddle);
    draw_list.push_back(&p2_paddle);

    int p1_points = 0;
    int p2_points = 0;

    sf::Font font;
    if (!font.loadFromFile("freefont/FreeSansBold.ttf")){
        sf::err() << "Could not open 'freefont/FreeSansBold.ttf'" << std::endl;
    }
    sf::Text p1_score_text("0", font);
    p1_score_text.setPosition(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 4);
    p1_score_text.setColor(sf::Color::White);
    centerTextOrigin(p1_score_text);

    sf::Text p2_score_text("0", font);
    p2_score_text.setPosition(SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT / 4);
    p2_score_text.setColor(sf::Color::White);
    centerTextOrigin(p2_score_text);

    sf::Text end_text("", font);
    end_text.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    draw_list.push_back(&p1_score_text);
    draw_list.push_back(&p2_score_text);
    draw_list.push_back(&end_text);
    
    enum State { START, SERVE, PLAYING, GAME_END };
    State state = START;
    // game loop
    while (window.isOpen())
    {
        if (!window.hasFocus()){
            sf::sleep(sf::milliseconds(50));
            continue;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) p2_paddle.move(-sf::Vector2f(0, PADDLE_SPEED));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) p2_paddle.move(sf::Vector2f(0, PADDLE_SPEED));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) p1_paddle.move(-sf::Vector2f(0, PADDLE_SPEED));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) p1_paddle.move(sf::Vector2f(0, PADDLE_SPEED));

        if (state == START){
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                state = SERVE;
                while (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    sf::sleep(sf::milliseconds(10));
            }
        }
        else if (state == SERVE) {
            int player_to_serve = ((p1_points + p2_points) / 2) % 2 + 1;
            if (player_to_serve == 1){
                ball.setPosition(p1_paddle.getPosition() + sf::Vector2f(15, 0));
            }
            else {
                ball.setPosition(p2_paddle.getPosition() - sf::Vector2f(15, 0));
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                state = PLAYING;
                if (player_to_serve == 1) {
                    ball_speed.x = 2;
                    ball_speed.y = -1;
                }
                else {
                    ball_speed.x = -2;
                    ball_speed.y = 1;
                }
                set_length(ball_speed, INITIAL_BALL_SPEED);
            }
        }
        else if (state == PLAYING){
            ball.move(ball_speed);
            if (ball_speed.y < 0 && ball.getPosition().y < 5)
                ball_speed.y = -ball_speed.y;
            if (ball_speed.y > 0 && ball.getPosition().y > SCREEN_HEIGHT - 5)
                ball_speed.y = -ball_speed.y;
            if (ball_speed.x > 0 && ball.getGlobalBounds().intersects(p2_paddle.getGlobalBounds())) {
                float y_diff = -ball.getPosition().y + p2_paddle.getPosition().y;
                float angle = std::atan(y_diff / (PADDLE_LENGTH / 2));
                ball_speed = fromPolar(length(ball_speed) * 1.05f, angle + 3.14159f);
            }
            if (ball_speed.x < 0 && ball.getGlobalBounds().intersects(p1_paddle.getGlobalBounds())) {
                float y_diff = ball.getPosition().y - p1_paddle.getPosition().y;
                float angle = std::atan(y_diff / (PADDLE_LENGTH / 2));
                ball_speed = fromPolar(length(ball_speed) * 1.05f, angle);
            }
            if (ball.getPosition().x < -BALL_SIZE){
                p2_points++;
                state = SERVE;
                p2_score_text.setString(std::to_string(p2_points));
                centerTextOrigin(p2_score_text);
            }
            if (ball.getPosition().x > SCREEN_WIDTH + BALL_SIZE){
                p1_points++;
                state = SERVE;
                p1_score_text.setString(std::to_string(p1_points));
                centerTextOrigin(p1_score_text);
            }
            if (std::abs(p1_points - p2_points) >= 2 && (p1_points >= 11 || p2_points >= 11)){
                if (p1_points > p2_points){
                    end_text.setString("Player 1 wins!");
                }
                else {
                    end_text.setString("Player 2 wins!");
                }
                centerTextOrigin(end_text);
                state = GAME_END;
            }
        }
        else if (state == GAME_END){
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                p1_points = 0;
                p2_points = 0;
                end_text.setString("");
                state = SERVE;
                p1_score_text.setString(std::to_string(p1_points));
                centerTextOrigin(p1_score_text);
                p2_score_text.setString(std::to_string(p2_points));
                centerTextOrigin(p2_score_text);
                while (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    sf::sleep(sf::milliseconds(10));
            }
        }
        window.clear(table_color);
        for (sf::Drawable *s : draw_list){
            window.draw(*s);
        }
        window.display();
    }
    return 0;
}
