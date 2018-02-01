#ifndef CONTROLADOR_JUEGO_H
#define CONTROLADOR_JUEGO_H

#include "controlador_base.h"
#include "../app/graficos/representador.h"
#include "../app/cargadores/cargador_mapas.h"
#include "../app/juego/jugador.h"
#include <class/controlador_argumentos.h>

#include <fstream>

extern DLibH::Log_base LOG;

class Controlador_juego:public Controlador_base
{
	private:

	Cargador_mapas cargador_mapas;
	Mapa mapa;
	DLibV::Camara camara;
	Representador representador;
	Jugador jugador;
	Entrada_nivel entrada_actual;
	
	bool TEST_jugador_debe_ajustar_y;
	

	void importar_nivel(unsigned int indice_nivel, unsigned int entrada);
	Input_usuario recoger_input_usuario(const Input_base& input);
	void procesar_input_jugador(const Input_base& input, float delta);
	void logica_jugador(float delta);

	bool jugador_puede_rebotar_con_pared(int dir) const;

	void movimiento_jugador_suelo_aire_eje_x(float delta, const DLibH::Vector_2d<float>& v, const Espaciable::t_caja& caja_original);
	void movimiento_jugador_suelo_aire_eje_y(float delta, const DLibH::Vector_2d<float>& v, const Espaciable::t_caja& caja_original);

	void controlar_colisiones_jugador_escalera();
	void controlar_subida_escalera(short int dir);
	void controlar_bajada_escalera(short int dir);
	void evaluar_perder_vida();
	void evaluar_cambio_nivel();
	void evaluar_enfoque_camara();

	public:

	Controlador_juego(Director_estados &d, const Herramientas_proyecto::Controlador_argumentos& carg);

	virtual void preloop(const Input_base& input, float delta)
	{
		mapa.acc_nivel().debug_limpiar_celdas_consideradas_colision();
	}

	virtual void postloop(const Input_base& input, float delta)
	{

	}

	virtual void loop(const Input_base& input, float delta);

	virtual void dibujar(DLibV::Pantalla& pantalla);

};

#endif
