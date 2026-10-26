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
    unsigned int resx = 1920;
    unsigned int resy = 1080;

    sf::Font font;
    font.openFromFile("fuente.TTF");

    // cargar texturas
    Texture texturaBomba;
    if (!texturaBomba.loadFromFile("img/bomba.png")) {
        cout << "Error cargando bomba.png" << endl;
    }
    Texture texturaBandera;
    if (!texturaBandera.loadFromFile("img/bandera.png")) {
        cout << "Error cargando bandera.png" << endl;
    }
    Texture texturaHud;
    if (!texturaHud.loadFromFile("img/hud.png")) {
        cout << "Error cargando hud.png" << endl;
    }
    Texture texturaBloque;
    if (!texturaBloque.loadFromFile("img/bloque.png")) {
        cout << "Error cargando bloque.png" << endl;
        return;
    }
    sf::Clock reloj;

    sf::Text textoTimer(font, "", 24);
    textoTimer.setPosition({10.f, 10.f});
    textoTimer.setFillColor(sf::Color::White);

    vector<Sprite> botones;
    int cantCuadradosX = 10;
    int cantCuadradosY = 10;


    for (int i = 0; i < cantCuadradosX; i++) {
        for (int j = 0; j < cantCuadradosY; j++) {

            sf::Sprite boton(texturaBloque);

            Vector2u tam = texturaBloque.getSize();

            float escalaX = 60.f / tam.x;
            float escalaY = 60.f / tam.y;

            boton.setScale({escalaX, escalaY});
            boton.setPosition({
                resx / 3.f + i * 65.f,
                100.f + 65.f * j
            });

            botones.push_back(boton);
        }
    }
    vector<bool> bloqueVisible(botones.size(), true);

    vector<bool> banderaActiva(botones.size(), false);

    uniform_int_distribution<int> distBomb(1, 10);
    int cantBombas = distBomb(gen);

    vector<int> indices(botones.size());
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), gen);

    vector<int> indicesBombas(indices.begin(), indices.begin() + cantBombas);

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
    //sprite hud
    Sprite hud(texturaHud);

    Vector2u tamHud = texturaHud.getSize();

    float escalaHudX = 1820.f / tamHud.x;
    float escalaHudY = 200.f / tamHud.y;

    hud.setScale({escalaHudX, escalaHudY});
    hud.setPosition({50, 880.f});
    // texto
    Text texto(font, "CANTIDAD DE BOMBAS: " + to_string(cantBombas), 30);
    texto.setPosition(Vector2f(200, resy-60));
    Sprite bombaHud(texturaBomba);
    bombaHud.setScale({escalaX*0.7, escalaY*0.7});
    bombaHud.setPosition(Vector2f(600, resy-60));

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
                                bloqueVisible[i] = false;
                            } else {
                                bloqueVisible[i] = false;

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
                                        bloqueVisible[idx] = false;
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
        window.draw(hud);
        window.draw(textoTimer);
        window.draw(texto);
        window.draw(bombaHud);

        for (const auto& b : bombas) {
            window.draw(b);
        }
        for (size_t i = 0; i < botones.size(); i++) {
            if (bloqueVisible[i]) {
                window.draw(botones[i]);
            }
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

    unsigned int resx = 1920;
    unsigned int resy = 1080;

    RenderWindow window(VideoMode::getDesktopMode(), "Buscaminas", State::Fullscreen);
    sf::Font font;
    font.openFromFile("fuente.TTF");

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
