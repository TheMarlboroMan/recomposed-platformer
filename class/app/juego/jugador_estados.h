#ifndef JUGADOR_ESTADOS_H
#define JUGADOR_ESTADOS_H

#include <memory>
#include <iostream>

#include "input_usuario.h"
#include <herramientas/vector_2d/vector_2d.h>
#include <herramientas/caja/caja.h>

class Jugador;

class Estado_jugador;
class Estado_jugador_suelo;
class Estado_jugador_aire;
class Estado_jugador_escalera;
class Estado_jugador_preparar_rebote_salto;
class Estado_jugador_rebote_salto;
class Estado_jugador_impulso;
class Estado_jugador_salto_impulso;
class Estado_jugador_salto_impulso_toca_suelo;
class Estado_jugador_agachado_salto;
class Estado_jugador_agachado;
class Estado_jugador_salto_alto;
class Estado_jugador_salto_alto_impulso;
class Estado_jugador_salto_escalera;

class Visitante_estado_jugador
{
	public:

	Visitante_estado_jugador() {}
	
//	virtual void visitar(Estado_jugador&)=0;
	virtual void visitar(Estado_jugador_suelo&)=0;
	virtual void visitar(Estado_jugador_aire&)=0;
	virtual void visitar(Estado_jugador_escalera&)=0;
	virtual void visitar(Estado_jugador_preparar_rebote_salto&)=0;
	virtual void visitar(Estado_jugador_rebote_salto&)=0;
	virtual void visitar(Estado_jugador_impulso&)=0;
	virtual void visitar(Estado_jugador_salto_impulso&)=0;
	virtual void visitar(Estado_jugador_salto_impulso_toca_suelo&)=0;
	virtual void visitar(Estado_jugador_agachado_salto&)=0;
	virtual void visitar(Estado_jugador_agachado&)=0;
	virtual void visitar(Estado_jugador_salto_alto&)=0;
	virtual void visitar(Estado_jugador_salto_alto_impulso&)=0;
	virtual void visitar(Estado_jugador_salto_escalera&)=0;
};

enum class t_direcciones {IZQUIERDA, DERECHA};

/*Cada uno de los posibles estados del jugador está codificado en una clase.
Los efectos que tiene sobre el movimiento del jugador se codifican en las 
clases derivadas de Estado_jugador.

Todo el proceso se separa en dos partes:

procesar(float delta) => Modifica el vector y la dirección del jugador, 
	posiblemente también el propio estado, en función del vector y el
	input corriente.

turno(float delta) => ejecuta las acciones temporales de cada turno, como
	por ejemplo, las cuentas atrás entre estados.
*/

class Estado_jugador
{
	//////////////////
	// Definiciones.
	public:

	enum class t_estados {
		SUELO, 				//Estado normal en suelo.
		AIRE, 				//Estado cayendo o saltando.
		ESCALERA, 			//Estado en escalera.
		PREPARAR_REBOTE_SALTO, 		//Cuando está pegado a una pared en la que se puede rebotar.
		REBOTE_SALTO, 			//Cuando está rebotando de pared en pared.
		IMPULSO, 			//Cuando está realizando impulso.
		SALTO_IMPULSO,			//Cuando está ejecutando el salto en impulso.
		SALTO_IMPULSO_TOCA_SUELO,	//Durante el salto impulso, tocar el suelo. Es un estado efímero que inmediatamente coloca al jugador como "suelo", pero permite clavar el aterrizaje.
		AGACHADO_SALTO,			//Estado forzado a quedarse parado tras el fin de un salto que has clavado o una caida larga.
		AGACHADO,			//Agachado por propia voluntad.
		SALTO_ALTO,			//Ejecutando el salto alto.
		SALTO_ALTO_IMPULSO,		//Ejecutando el impulso tras salto alto.
		SALTO_ESCALERA			//Ejecutando salto de escalera.
	};

	typedef DLibH::Caja<int, unsigned int> t_caja;	//La caja que usaremos para definir transformaciones entre estados.

	//Todas las medidas en unidades por segundo, salvo que se diga lo contrario.
	static const float COOLDOWN_IMPULSO;		//Tiempo que debe transcurrir para poder volver a usar el impulso, en segundos.
	static const float VELOCIDAD_ESCALERA;
	static const float COOLDOWN_AGACHADO_SALTO;	//Tiempo que debe estar parado después de clavar un salto.
	static const float DECELERACION_IMPULSO;	//Deceleración en estado de impulso.
	static const float MARGEN_REBOTE_SALTO;		//Tiempo que tarda en caer cuando está intentando rebotar en pared (en segundos).
	static const float VEL_MINIMA_SUELO;
	static const float VEL_MAXIMA_SUELO;	
	static const float ACELERACION_SALTO;
	static const float VELOCIDAD_FRENO_MANUAL;
	static const float ACELERACION_SUELO;
	static const float DECELERACION_SUELO;
	static const float MARGEN_IMPULSO;		//Margen de tiempo entre inputs para realizar un impulso.
	static const float ACELERACION_SALTO_ALTO;
	static const float VEL_MAXIMA_HORIZONTAL_SALTO_ALTO;
	static const float PESO_DEFECTO;
	static const float DECELERACION_SALTO_ESCALERA;

	////////////////////
	//Propiedades...

	private:

	DLibH::Vector_2d<float> v;
	Input_usuario iu;
	t_direcciones direccion;
	bool cambia_estado;
	t_estados nuevo_estado;

	////////////////////
	//Métodos

	protected:

	void reducir_valor_con_delta(float &v, float delta)
	{
		v-=delta;
		if(v < 0.0) v=0.0;
	}

	////////////////////
	//Interface.

	public:

	/*Este devolvería la caja que se aplica para transformar un estado en 
	otro. Todos los estados se comparan con el método "caja_original" que
	se define aquí, que se basaría en "estar en pie.*/

	virtual t_caja obtener_caja_estado() const=0;
	virtual t_estados acc_estado() const=0;
	virtual void recibir_visitante(Visitante_estado_jugador&)=0;

	/*El método procesar debe alterar el vector y la dirección en función
	del input del jugador, si tal cosa es posible.*/
	virtual void procesar(float delta)=0;
	virtual void turno(float delta)=0;
	virtual float obtener_peso() const {return PESO_DEFECTO;}

	//////////////////
	//Implementar.

	static t_caja calcular_cambio_estados(const Estado_jugador& anterior, const Estado_jugador& nuevo)
	{
		//Estado base...
		t_caja resultado(0, 0, 16, 40);

		//Salida del estado anterior... Restaremos la y y la x al 
		//estado base.
		t_caja caja_anterior=anterior.obtener_caja_estado();
		resultado.origen.x-=caja_anterior.origen.x;
		resultado.origen.y-=caja_anterior.origen.y;

		//Entrada del estado nuevo... Sumando los valores del nuevo.
		t_caja caja_nuevo=nuevo.obtener_caja_estado();
		resultado.origen.x+=caja_nuevo.origen.x;
		resultado.origen.y+=caja_nuevo.origen.y;
		resultado.w=caja_nuevo.w;
		resultado.h=caja_nuevo.h;

		return resultado;
	}

	void cargar_datos(const DLibH::Vector_2d<float> pv, const Input_usuario& piu, t_direcciones pdir) 
	{
		v=pv;
		iu=piu;
		direccion=pdir;
	}

	const DLibH::Vector_2d<float>& acc_vector() const {return v;}
	const Input_usuario& acc_input() const {return iu;}
	const t_direcciones acc_direccion() const {return direccion;}
	bool es_cambia_estado() const {return cambia_estado;}
	const t_estados acc_nuevo_estado() const {return nuevo_estado;}

	void establecer_vector_x(float x) {v.x=x;}
	void establecer_vector_y(float y) {v.y=y;}
	void establecer_direccion(t_direcciones d) {direccion=d;}
	void cambiar_estado(t_estados e) 
	{
		cambia_estado=true;
		nuevo_estado=e;
	}

	Estado_jugador()
		:cambia_estado(false), nuevo_estado(t_estados::SUELO) 
	{}
	virtual ~Estado_jugador() {}
};

/******************************************************************************/

class Estado_jugador_suelo:public Estado_jugador
{
	private:

	short int dir_keyup_impulso;	//Dirección del último keyup.
	float margen_impulso;	//Márgen de tiempo que tenemos entre el keyup y el keywodn.
	float cooldown_impulso;	//Tiempo que debe pasar antes de poder usar el impulso de nuevo.

	public:

	Estado_jugador_suelo(float cd)
		:dir_keyup_impulso(0), 
		margen_impulso(0.0), 
		cooldown_impulso(cd)
	{}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SUELO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta);

	virtual void turno(float delta)
	{
		reducir_valor_con_delta(margen_impulso, delta);
		reducir_valor_con_delta(cooldown_impulso, delta);
		if(!margen_impulso) dir_keyup_impulso=0;
	}

	bool ha_realizado_input_impulso() const
	{
		if(dir_keyup_impulso && margen_impulso && !cooldown_impulso)
		{
			auto input=acc_input();

			if( (dir_keyup_impulso > 0 && input.mov_horizontal > 0)
			|| (dir_keyup_impulso < 0 && input.mov_horizontal < 0))
			{
				return true;
			}
		}
	
		return false;
	}
};

class Estado_jugador_aire:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::AIRE;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta);
	virtual void turno(float delta) {}
};

class Estado_jugador_escalera:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::ESCALERA;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta)
	{
		auto input=acc_input();

		//No vamos a permitir apuntar hacia los lados y subir o bajar a la vez. Paramos el vector y luego veremos si se activa.
		establecer_vector_y(0.0);
		if(input.mov_horizontal) establecer_direccion(input.mov_horizontal < 0 ? t_direcciones::IZQUIERDA : t_direcciones::DERECHA);
		else if(input.mov_vertical) establecer_vector_y(input.mov_vertical * VELOCIDAD_ESCALERA);
	}

	virtual void turno(float delta) {}
};

class Estado_jugador_preparar_rebote_salto:public Estado_jugador
{
	private:

	float cooldown;

	public:

	Estado_jugador_preparar_rebote_salto():
		cooldown(MARGEN_REBOTE_SALTO)
	{}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::PREPARAR_REBOTE_SALTO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}
	virtual void turno(float delta)
	{
		reducir_valor_con_delta(cooldown, delta);
		if(!cooldown) cambiar_estado(t_estados::AIRE);
	}
};

class Estado_jugador_rebote_salto:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::REBOTE_SALTO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}
	virtual void turno(float delta){}
};

class Estado_jugador_impulso:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::IMPULSO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta)
	{
		float vx=acc_vector().x;
		float v=fabs(vx);
		if(v)
		{
			v-=DECELERACION_IMPULSO * delta;
			v*= vx > 0.0 ? 1.0 : -1.0;
			establecer_vector_x(v);
		}
	}

	virtual void turno(float delta){}
};

class Estado_jugador_salto_impulso:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SALTO_IMPULSO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}
	virtual void turno(float delta){}
};

class Estado_jugador_salto_impulso_toca_suelo:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SALTO_IMPULSO_TOCA_SUELO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}
	virtual void turno(float delta){}
};

class Estado_jugador_agachado_salto:public Estado_jugador
{
	private:

	float cooldown;

	public:

	Estado_jugador_agachado_salto()
		:cooldown(COOLDOWN_AGACHADO_SALTO)
	{}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 10, 16, 30);}
	virtual t_estados acc_estado() const {return t_estados::AGACHADO_SALTO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}

	virtual void turno(float delta)
	{		
		reducir_valor_con_delta(cooldown, delta);
		if(!cooldown) cambiar_estado(t_estados::SUELO);
	}
};

class Estado_jugador_agachado:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 10, 16, 30);}
	virtual t_estados acc_estado() const {return t_estados::AGACHADO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta) {}
	virtual void turno(float delta){}
};

class Estado_jugador_salto_alto:public Estado_jugador
{
	private:

	short int dir_keyup_impulso;	//Dirección del último keyup.
	float margen_impulso;	//Márgen de tiempo que tenemos entre el keyup y el keywodn.

	public:

	Estado_jugador_salto_alto()
		:dir_keyup_impulso(0), margen_impulso(0.0)
	{}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SALTO_ALTO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual void procesar(float delta);
	virtual void turno(float delta)
	{
		reducir_valor_con_delta(margen_impulso, delta);
		if(!margen_impulso) dir_keyup_impulso=0;
	}

	bool ha_realizado_input_impulso() const
	{
		if(dir_keyup_impulso && margen_impulso)
		{
			auto input=acc_input();

			if( (dir_keyup_impulso > 0 && input.mov_horizontal > 0)
			|| (dir_keyup_impulso < 0 && input.mov_horizontal < 0))
			{
				return true;
			}
		}
	
		return false;
	}
};

class Estado_jugador_salto_alto_impulso:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SALTO_ALTO_IMPULSO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual float obtener_peso() const {return 0.0;}
	virtual void procesar(float delta)
	{
		float vx=acc_vector().x;
		float v=fabs(vx);

		//Esto que parece un sinsentido es una solución: si estás haciendo
		//impulso y chocas con una pared te frena pero no te cambia el
		//estado, de modo que sigue frenando y terminas un poco hacia atrás.
		if(v)
		{
			v-=DECELERACION_IMPULSO * delta;
			v*= vx > 0.0 ? 1.0 : -1.0;
			establecer_vector_x(v);
		}
	}
	virtual void turno(float delta){}
};

class Estado_jugador_salto_escalera:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 40);}
	virtual t_estados acc_estado() const {return t_estados::SALTO_ESCALERA;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}
	virtual float obtener_peso() const {return PESO_DEFECTO * 0.7;}
	virtual void procesar(float delta)
	{
/*		float vx=acc_vector().x;
		float v=fabs(vx);

		//Esto que parece un sinsentido es una solución: si estás haciendo
		//impulso y chocas con una pared te frena pero no te cambia el
		//estado, de modo que sigue frenando y terminas un poco hacia atrás.
		if(v)
		{
			v-=DECELERACION_SALTO_ESCALERA * delta;
			v*= vx > 0.0 ? 1.0 : -1.0;
			establecer_vector_x(v);
		}
*/
	}
	virtual void turno(float delta)
	{
		if(acc_vector().y >= 0.0) cambiar_estado(t_estados::AIRE);
	}
};

#endif
