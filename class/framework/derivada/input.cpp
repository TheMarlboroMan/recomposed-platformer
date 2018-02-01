#include "input.h"

void Input::configurar()
{
	mapa_teclado.insert(std::make_pair(I_ESCAPE, SDL_SCANCODE_ESCAPE));
	mapa_teclado.insert(std::make_pair(I_ESPACIO, SDL_SCANCODE_SPACE));
	mapa_teclado.insert(std::make_pair(I_IZQUIERDA, SDL_SCANCODE_LEFT));
	mapa_teclado.insert(std::make_pair(I_DERECHA, SDL_SCANCODE_RIGHT));
	mapa_teclado.insert(std::make_pair(I_ARRIBA, SDL_SCANCODE_UP));
	mapa_teclado.insert(std::make_pair(I_ABAJO, SDL_SCANCODE_DOWN));
	mapa_teclado.insert(std::make_pair(I_SALTAR, SDL_SCANCODE_SPACE));

	mapa_raton.insert(std::make_pair(I_CLICK_I, SDL_BUTTON_LEFT));
	mapa_raton.insert(std::make_pair(I_CLICK_D, SDL_BUTTON_RIGHT));
}
