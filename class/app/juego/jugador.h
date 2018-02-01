#ifndef JUGADOR_H
#define JUGADOR_H

#include <memory>
#include "actor.h"
#include "movil.h"
#include "../../herramientas_proyecto/tabla_sprites.h"
#include "../recursos.h"
#include "jugador_estados.h"
#include "input_usuario.h"

class Jugador:public Actor, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	private:

	static const float VELOCIDAD_MINIMA_SALTO_REBOTE;
	static const float VECTOR_SALTO;
	static const float SEGUNDOS_COOLDOWN_ESCALERA;	//Tiempo que tarda en poderse coger una escalera tras soltar otra (en segundos).
	static const float VECTOR_HORIZONTAL_SALTO_REBOTE;
	static const float VECTOR_VERTICAL_FORZAR_AGACHARSE;	//Velocidad vertical mínima ante que la que se agacha cuando toca el suelo.
	static const float VECTOR_VERTICAL_SALTO_REBOTE;
	static const float VECTOR_VERTICAL_SALTO_REBOTE_EXTRA;
	static const float VECTOR_VERTICAL_SALTO_REBOTE_CORTO;
	static const float VECTOR_VERTICAL_MAXIMO_SALTO_REBOTE;
	static const float VECTOR_HORIZONTAL_SALTO_ESCALERA;
	static const float VECTOR_VERTICAL_SALTO_ESCALERA;
	static const float VECTOR_INICIAL_IMPULSO;
	static const float VECTOR_INICIAL_SALTO_IMPULSO;
	static const float VECTOR_SALTO_IMPULSO;
	static const float MAXIMA_VELOCIDAD_CAIDA;
	static const float VECTOR_SALTO_ALTO;
	static const float VECTOR_INICIAL_IMPULSO_SALTO_ALTO;	//El vector para el impulso en salto vertical.
//	static const float MAXIMA_VELOCIDAD_SUBIDA;

	static const int DISTANCIA_MAXIMA_BAJAR_ESCALERA=10;
	static const int DISTANCIA_MAXIMA_SUBIR_ESCALERA=10;

	/////////////////////////
	// Propiedades...

	private:

	std::unique_ptr<Estado_jugador> estado;
	Input_usuario input;

	Tabla_sprites TREC;
	t_direcciones direccion;

	float cooldown_escalera;

	/////////////////////////
	// Implementando Actor...

	private:

	virtual float obtener_peso() const 
	{
		return estado->obtener_peso();
	}

	///////////////////////////
	// Implementando Espaciable

	public:

	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////
	// Implementación de Representable.

	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const {return 20;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "JUGADOR EN "+std::to_string(acc_espaciable_x())+","+std::to_string(acc_espaciable_y());}

	/////////////////////////
	// Métodos internos.
	
	private:

	void cambiar_estado(Estado_jugador * e);
	void iniciar_cooldown_escalera() {cooldown_escalera=SEGUNDOS_COOLDOWN_ESCALERA;}

	////////////////////////
	// Interfaz pública...

	public:

	Jugador(float x=0.0, float y=0.0);
	~Jugador();

	//const Input_usuario& acc_input() const {return input;}
	void recibir_input(const Input_usuario& iu) {input=iu;}
	void turno(float delta);

	void procesar_estado(float delta);

	void accion_gravedad(float delta, float valor_gravedad);

	void establecer_en_escalera(const Espaciable& e);
	void establecer_en_agachado() 
	{
		cambiar_estado(new Estado_jugador_agachado());		
		establecer_vector(0.0, Movil::V_X);
	}
	void establecer_en_aire() {cambiar_estado(new Estado_jugador_aire());}
	void establecer_en_suelo(bool con_cooldown) 
	{
		float cd=con_cooldown ? Estado_jugador::COOLDOWN_IMPULSO : 0.0;
		cambiar_estado(new Estado_jugador_suelo(cd));
	}
	void establecer_en_preparar_rebote_salto(short int dir);
	void establecer_en_impulso(short int dir);
	void establecer_en_impulso_salto_alto(short int dir);
	void establecer_en_salto_impulso();

	//Cosas que se pueden imponer desde el controlador.
	void chocar_con_suelo() {establecer_vector(0.0, Movil::V_Y); establecer_en_suelo(false);}
	void chocar_con_techo() {establecer_vector(0.0, Movil::V_Y); establecer_en_aire();}
	void chocar_con_pared() {establecer_vector(0.0, Movil::V_X);}
	void chocar_con_suelo_salto_impulso() {establecer_vector(0.0, Movil::V_Y); cambiar_estado(new Estado_jugador_salto_impulso_toca_suelo());}
	void clavar_aterrizaje() {establecer_vector(0.0, Movil::V_X); establecer_vector(0.0, Movil::V_Y); cambiar_estado(new Estado_jugador_agachado_salto());}
	void clavar_fin_impulso() {establecer_vector(0.0, Movil::V_X);}
		
	void saltar();
	void saltar_en_escalera(float multiplicador);
	void saltar_alto();
	void bajar_escalera_en_aire();
	void bajar_escalera(const Espaciable& celda_destino);
	void realizar_rebote_salto();

	Espaciable::t_caja obtener_caja_suelo() const; 		//Una caja plana en la unidad más baja del jugador.
	Espaciable::t_caja obtener_caja_escalera() const;	
	Espaciable::t_caja obtener_caja_debajo() const;		//Una caja plana un pixel por debajo del jugador.
	Espaciable::t_caja obtener_caja_rebote_salto(t_direcciones dir) const;
//	Espaciable::t_caja obtener_caja_snap() const;		//

	Estado_jugador::t_estados obtener_estado() const {return estado->acc_estado();}
	void recibir_visitante_estado(Visitante_estado_jugador& v) {estado->recibir_visitante(v);}

	bool es_en_suelo() const {return estado->acc_estado()==Estado_jugador::t_estados::SUELO;}
	bool es_en_aire() const {return estado->acc_estado()==Estado_jugador::t_estados::AIRE;}
	bool es_en_impulso() const {return estado->acc_estado()==Estado_jugador::t_estados::IMPULSO;}
	bool es_en_rebote_salto() const {return estado->acc_estado()==Estado_jugador::t_estados::REBOTE_SALTO;}
	bool es_en_escalera() const {return estado->acc_estado()==Estado_jugador::t_estados::ESCALERA;}
	bool es_en_preparar_rebote_salto() const {return estado->acc_estado()==Estado_jugador::t_estados::PREPARAR_REBOTE_SALTO;}
	bool es_en_salto_impulso() const {return estado->acc_estado()==Estado_jugador::t_estados::SALTO_IMPULSO;}
	bool es_en_salto_impulso_toca_suelo() const {return estado->acc_estado()==Estado_jugador::t_estados::SALTO_IMPULSO_TOCA_SUELO;}
	bool es_en_agachado_salto() const {return estado->acc_estado()==Estado_jugador::t_estados::AGACHADO_SALTO;}
	bool es_en_agachado() const {return estado->acc_estado()==Estado_jugador::t_estados::AGACHADO;}

	//TODO: Bufffff.... Hacer que sea parte a implementar de cada clase estado, se nos olvida un estado y la jodemos.
	bool esta_en_el_aire() const {
			return estado->acc_estado()==Estado_jugador::t_estados::AIRE || 
			estado->acc_estado()==Estado_jugador::t_estados::REBOTE_SALTO || 
			estado->acc_estado()==Estado_jugador::t_estados::SALTO_IMPULSO ||
			estado->acc_estado()==Estado_jugador::t_estados::SALTO_ALTO ||
			estado->acc_estado()==Estado_jugador::t_estados::SALTO_ALTO_IMPULSO ||
			estado->acc_estado()==Estado_jugador::t_estados::SALTO_ESCALERA;}

	bool esta_en_el_suelo() const {
			return estado->acc_estado()==Estado_jugador::t_estados::SUELO || 
			estado->acc_estado()==Estado_jugador::t_estados::IMPULSO;}

	bool puede_subir_escalera() const {return !cooldown_escalera;}
	bool puede_saltar() const {return estado->acc_estado()==Estado_jugador::t_estados::SUELO || estado->acc_estado()==Estado_jugador::t_estados::ESCALERA || estado->acc_estado()==Estado_jugador::t_estados::PREPARAR_REBOTE_SALTO;}
	bool ha_realizado_input_impulso() const;

	bool es_encarando_direccion(t_direcciones dir) const {return direccion==dir;}
	bool es_cumple_condiciones_para_salto_rebote(t_direcciones dir) const;
	bool es_cumple_condiciones_para_clavar_fin_impulso() const {return !input.mov_horizontal;}
	bool es_cae_a_velocidad_agacharse() const {return acc_vector().y >= VECTOR_VERTICAL_FORZAR_AGACHARSE;}

	//Comprobar si es posible bajar una escalera cuando hemos llegado al último peldaño por abajo y abajo hay algo sólido...
	//basicamente comprueba la distancia y poco más. Luego item, pero por arriba.
	bool comprobar_bajar_escalera_abajo(const Espaciable& c) const {return c.acc_espaciable_y() - acc_espaciable_fy() <= DISTANCIA_MAXIMA_BAJAR_ESCALERA;}
	bool comprobar_bajar_escalera_arriba(const Espaciable& c) const {return acc_espaciable_fy() - c.acc_espaciable_y() <= DISTANCIA_MAXIMA_SUBIR_ESCALERA;}	
};

#endif
