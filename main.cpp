#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include <string>
using namespace std;
using namespace sf;

void partida(RenderWindow& window){
    random_device rd;
    mt19937 gen(rd());
    unsigned int resx = 900;
    unsigned int resy = 900;

    sf::Font font;
    font.openFromFile("ARIAL.TTF");

    sf::Clock reloj;

    sf::Text textoTimer(font, "", 24);
    textoTimer.setPosition({10.f, 10.f});
    textoTimer.setFillColor(sf::Color::White);

    vector<sf::RectangleShape> botones;
    int cantCuadradosX = 10;
    int cantCuadradosY = 10;

    // crear el grid de botones (UNA sola vez)
    for (int i = 0; i < cantCuadradosX; i++) {
        for (int j = 0; j < cantCuadradosY; j++) {
            sf::RectangleShape boton(sf::Vector2f(60.f, 60.f));
            boton.setPosition({resx/6 + i * 65.f, 100.f + 65 * j});
            boton.setFillColor(sf::Color::White);
            botones.push_back(boton);
        }
    }

    // vector paralelo: qué casilleros tienen bandera
    vector<bool> banderaActiva(botones.size(), false);

    // cuántas bombas van a haber
    uniform_int_distribution<int> distBomb(1, 10);
    int cantBombas = distBomb(gen);

    // elegir índices únicos al azar dentro del grid
    vector<int> indices(botones.size());
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), gen);

    vector<int> indicesBombas(indices.begin(), indices.begin() + cantBombas);

    // cargar texturas
    Texture texturaBomba;
    if (!texturaBomba.loadFromFile("bomba.png")) {
        cout << "Error cargando bomba.png" << endl;
    }
    Texture texturaBandera;
    if (!texturaBandera.loadFromFile("bandera.png")) {
        cout << "Error cargando bandera.png" << endl;
    }

    Vector2u tamaniotexturaBomba = texturaBomba.getSize();
    float escalaX = 60.f / tamaniotexturaBomba.x;
    float escalaY = 60.f / tamaniotexturaBomba.y;

    // crear un sprite por cada bomba, en la posición del botón correspondiente
    vector<Sprite> bombas;
    for (int idx : indicesBombas) {
        cout << idx << endl;
        Sprite bomba(texturaBomba);
        bomba.setScale({escalaX, escalaY});
        bomba.setPosition(botones[idx].getPosition());
        bombas.push_back(bomba);
    }

    // un solo sprite de bandera, reutilizado para dibujar todas
    Sprite bandera(texturaBandera);
    bandera.setScale({escalaX, escalaY});

    // texto
    Text texto(font, to_string(cantBombas), 30);
    texto.setPosition(Vector2f(resx/2+30, resy-60));
    Sprite bombaHud(texturaBomba);
    bombaHud.setScale({escalaX*0.7, escalaY*0.7});
    bombaHud.setPosition(Vector2f(resx/2-30, resy-60));

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                Vector2f mousePos = window.mapPixelToCoords(mouseButton->position);

                if (mouseButton->button == Mouse::Button::Left) {
                    for (size_t i = 0; i < botones.size(); i++) {
                        if (botones[i].getGlobalBounds().contains(mousePos) && !banderaActiva[i]) {
                            cout << "Se clickeo el boton " << i << endl;

                            bool clickeoBomba = false;
                            for (int j = 0; j < indicesBombas.size(); j++) {
                                if ((int)i == indicesBombas[j]) {
                                    clickeoBomba = true;
                                    break;
                                }
                            }

                            if (clickeoBomba) {
                                cout << "game over" << endl;
                                botones[i].setFillColor(sf::Color::Transparent);
                            } else {
                                botones[i].setFillColor(sf::Color::Transparent);

                                int offsets[9] = {-11, -10, -9, -1, 0, 1, 9, 10, 11};
                                for (int k = 0; k < 9; k++) {
                                    int idx = i + offsets[k];
                                    if (idx < 0 || idx >= (int)botones.size()) continue;

                                    bool esBomba = false;
                                    for (int j = 0; j < indicesBombas.size(); j++) {
                                        if (idx == indicesBombas[j]) {
                                            esBomba = true;
                                            break;
                                        }
                                    }

                                    if (!esBomba) {
                                        botones[idx].setFillColor(sf::Color::Transparent);
                                    }
                                }
                            }
                        }
                    }
                }

                if (mouseButton->button == Mouse::Button::Right) {
                    for (size_t i = 0; i < botones.size(); i++) {
                        if (botones[i].getGlobalBounds().contains(mousePos)) {
                            banderaActiva[i] = !banderaActiva[i]; // toggle
                        }
                    }
                }
            }
        }

        float segundos = reloj.getElapsedTime().asSeconds();
        textoTimer.setString("Tiempo: " + to_string((int)segundos));

        // acá dibujás tus objetos
        window.clear(sf::Color::Black);
        window.draw(textoTimer);
        window.draw(texto);
        window.draw(bombaHud);

        for (const auto& b : bombas) {
            window.draw(b);
        }
        for (const auto& b : botones) {
            window.draw(b);
        }

        // dibujar banderas encima de los botones que la tienen activa
        for (size_t i = 0; i < botones.size(); i++) {
            if (banderaActiva[i]) {
                bandera.setPosition(botones[i].getPosition());
                window.draw(bandera);
            }
        }

        window.display();
    }
}

int main() {
    random_device rd;
    mt19937 gen(rd());

    unsigned int resx = 900;
    unsigned int resy = 900;

    RenderWindow window(VideoMode({resx, resy}), "Mi juego");
    sf::Font font;
    font.openFromFile("ARIAL.TTF");

    sf::RectangleShape rectangle(sf::Vector2f(100.f, 50.f));
    rectangle.setPosition(Vector2f((resx - 100) / 2.f, 300.f));
    rectangle.setFillColor(sf::Color::Red);

    Text texto(font, "Jugar", 24);
    texto.setPosition(Vector2f((resx - 100) / 2.f, 300.f));

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseButton->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mouseButton->position);
                    if (rectangle.getGlobalBounds().contains(mousePos)) {
                        cout << "jugar" << endl;
                        partida(window);
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        window.draw(rectangle);
        window.draw(texto);
        window.display();
    }

    return 0;
}
