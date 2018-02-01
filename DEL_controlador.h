#ifndef PROYECTO_SDL2_CONTROLADOR_H
#define PROYECTO_SDL2_CONTROLADOR_H

#include "derivada_proyecto/configuracion.h"
#include "derivada_proyecto/input.h"
#include "derivada_proyecto/audio.h"
#include "derivada_proyecto/localizador.h"
#include "derivada_proyecto/cargador_recursos.h"
#include "recursos/recursos.h"

#include "controladores/controlador_interface.h"

#include "espaciable.h"
#include "graficos/representador.h"

class Controlador
{
	///////////////////
	// Propiedades

	protected:

	DLibH::Controlador_argumentos& controlador_argumentos;
	DLibH::Controlador_fps_SDL controlador_fps;
	Configuracion configuracion;
	DLibV::Pantalla pantalla;
	Localizador localizador;
	Input input;

	unsigned int id_idioma;

	///////////////////////////
	// Internos...

	protected:

	void inicializar_entorno_grafico(DLibV::Pantalla& pantalla, unsigned int w, unsigned int h);
	void configurar_entorno_grafico(DLibV::Pantalla& pantalla, unsigned int w, unsigned int h);

	void inicializar_entorno_audio();
	void configurar_entorno_audio();

	/////////////////////
	// Interfaz pública

	public:

	void inicializar();
	bool loop(Controlador_interface& CI);
	Controlador(DLibH::Controlador_argumentos&);
	~Controlador();
};

#endif
