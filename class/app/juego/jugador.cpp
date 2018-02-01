#include "jugador.h"

const float Jugador::VECTOR_SALTO=-270.0;
const float Jugador::SEGUNDOS_COOLDOWN_ESCALERA=0.3;
const float Jugador::VECTOR_HORIZONTAL_SALTO_REBOTE=150.0;
const float Jugador::VECTOR_VERTICAL_SALTO_REBOTE=-250.0;
const float Jugador::VECTOR_VERTICAL_SALTO_REBOTE_EXTRA=-310.0;
const float Jugador::VECTOR_VERTICAL_SALTO_REBOTE_CORTO=-190.0;
const float Jugador::VELOCIDAD_MINIMA_SALTO_REBOTE=70.0;
const float Jugador::VECTOR_VERTICAL_MAXIMO_SALTO_REBOTE=80.0;
const float Jugador::VECTOR_HORIZONTAL_SALTO_ESCALERA=200.0;
const float Jugador::VECTOR_VERTICAL_SALTO_ESCALERA=-80.0;
const float Jugador::VECTOR_INICIAL_IMPULSO=400.0;
const float Jugador::VECTOR_INICIAL_SALTO_IMPULSO=380.0;
const float Jugador::VECTOR_SALTO_IMPULSO=-120.0;
const float Jugador::MAXIMA_VELOCIDAD_CAIDA=600.0;
const float Jugador::VECTOR_VERTICAL_FORZAR_AGACHARSE=300.0;
const float Jugador::VECTOR_SALTO_ALTO=-450.0;
const float Jugador::VECTOR_INICIAL_IMPULSO_SALTO_ALTO=380.0;
//const float Estado_jugador::MAXIMA_VELOCIDAD_SUBIDA=600.0;


Jugador::Jugador(float x, float y)
	:Actor(x, y, 16, 40), Movil(),
	estado(new Estado_jugador_suelo(false)),
	TREC("data/recortes/jugador.dat"),	
	direccion(t_direcciones::DERECHA),
	cooldown_escalera(0.0)
{
}

Jugador::~Jugador()
{

}

void Jugador::transformar_bloque(Bloque_transformacion_representable &b) const
{
	//TODO: Esta aproximación resultará del todo inútil cuando haya animación.
	class Vis:public Visitante_estado_jugador
	{
		private:

		const Input_usuario& input;

		public:
	
		unsigned int indice_sprite;

		Vis(const Input_usuario& i):input(i), indice_sprite(0) {}
	
		virtual void visitar(Estado_jugador&) {}
		virtual void visitar(Estado_jugador_suelo&) {indice_sprite=0;}
		virtual void visitar(Estado_jugador_aire&) {indice_sprite=2;}
		virtual void visitar(Estado_jugador_escalera&) 
		{
			if(input.mov_horizontal) indice_sprite=5;
			else indice_sprite=1;
		}
		virtual void visitar(Estado_jugador_preparar_rebote_salto&) {indice_sprite=4;}
		virtual void visitar(Estado_jugador_rebote_salto&) {indice_sprite=2;}
		virtual void visitar(Estado_jugador_impulso&) {indice_sprite=4;}
		virtual void visitar(Estado_jugador_salto_impulso&) {indice_sprite=2;}
		virtual void visitar(Estado_jugador_salto_impulso_toca_suelo&) {}
		virtual void visitar(Estado_jugador_agachado_salto&) {indice_sprite=3;}
		virtual void visitar(Estado_jugador_agachado&) {indice_sprite=3;}
		virtual void visitar(Estado_jugador_salto_alto&) {indice_sprite=2;}
		virtual void visitar(Estado_jugador_salto_alto_impulso&) {indice_sprite=2;}
		virtual void visitar(Estado_jugador_salto_escalera&) {indice_sprite=4;}
	}vis(input);

	//Se asume que todos los frames van mirando a la derecha.

	bool invertir_si_izquierda=true;
	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(255);
	b.establecer_recurso(Recursos_graficos::RT_JUGADOR);
	estado->recibir_visitante(vis);
	Frame_sprites f=TREC.obtener(vis.indice_sprite);

	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		if(invertir_si_izquierda && direccion==t_direcciones::IZQUIERDA) 
		{
			b.invertir_horizontal(true); 

	/*Cuando el ancho de la caja lógica sea distinto del ancho de la caja
	de representación ajustaremos la posición.*/

			if(f.w != acc_espaciable_w())
			{
				f.desp_x-=f.w-acc_espaciable_w();
			}

		}
		b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
	}
}

void Jugador::accion_gravedad(float delta, float valor_gravedad)
{
	integrar_vector(delta, ref_vector_y(), obtener_peso()*valor_gravedad);
	if(acc_vector().y > MAXIMA_VELOCIDAD_CAIDA) establecer_vector(MAXIMA_VELOCIDAD_CAIDA, Movil::V_Y);
}

void Jugador::procesar_estado(float delta)
{
	//Cargar input...
	estado->cargar_datos(acc_vector(), input, direccion);

	//Se realizaría el proceso por parte del estado.
	estado->procesar(delta);

	//Actualizar los datos...
	auto v=estado->acc_vector();
	establecer_vector(v.x, Movil::V_X);
	establecer_vector(v.y, Movil::V_Y);
	direccion=estado->acc_direccion();
}

/*Todos estos métodos devuelven cajas en función de varias necesidades: por
ejemplo, la caja justo en el suelo del jugador (se usa para escaleras), la
caja que se usa para subir a escaleras, la que está inmediatamente debajo 
(para gravedad...), la que se usa para buscar celdas de rebote de salto...*/

Espaciable::t_caja Jugador::obtener_caja_suelo() const
{
	Espaciable::t_caja resultado=copia_caja();
	resultado.origen.y+=resultado.h-1;
	resultado.h=1;
	return resultado;
}

Espaciable::t_caja Jugador::obtener_caja_escalera() const
{
	Espaciable::t_caja resultado=copia_caja();
	resultado.h=resultado.h / 2;
	return resultado;
}

Espaciable::t_caja Jugador::obtener_caja_debajo() const
{
	Espaciable::t_caja resultado=obtener_caja_suelo();
	resultado.origen.y++;
	return resultado;
}
/*
Espaciable::t_caja Jugador::obtener_caja_snap() const
{
	Espaciable::t_caja resultado=obtener_caja_debajo();
	resultado.h+=4;
	return resultado;
}
*/
Espaciable::t_caja Jugador::obtener_caja_rebote_salto(t_direcciones dir) const
{
	Espaciable::t_caja resultado=copia_caja();
	if(dir==t_direcciones::DERECHA)
	{
		resultado.origen.x+=resultado.w;
	}
	else
	{
		resultado.origen.x--;
	}

	resultado.w=1;

	return resultado;
}

void Jugador::establecer_en_escalera(const Espaciable& e)
{
	cambiar_estado(new Estado_jugador_escalera());

	establecer_vector(0, Movil::V_X);
	establecer_vector(0, Movil::V_Y);

	float pos=(e.acc_espaciable_w() / 2) - acc_espaciable_w()/2;
	mut_x_caja(e.acc_espaciable_x()+pos);
}

void Jugador::establecer_en_impulso(short int dir)
{
	float v=VECTOR_INICIAL_IMPULSO  * dir;
	direccion=dir > 0 ? t_direcciones::DERECHA : t_direcciones::IZQUIERDA;
	cambiar_estado(new Estado_jugador_impulso());
	establecer_vector(v, Movil::V_X);
	establecer_vector(0.0, Movil::V_Y);	
}

void Jugador::establecer_en_impulso_salto_alto(short int dir)
{
	float v=VECTOR_INICIAL_IMPULSO_SALTO_ALTO  * dir;
	direccion=dir > 0 ? t_direcciones::DERECHA : t_direcciones::IZQUIERDA;
	cambiar_estado(new Estado_jugador_salto_alto_impulso());
	establecer_vector(v, Movil::V_X);
	establecer_vector(0.0, Movil::V_Y);	
}

/*Se reestablece el vector x y se da un nuevo vector y.*/

void Jugador::establecer_en_salto_impulso()
{
	cambiar_estado(new Estado_jugador_salto_impulso());
	float v=VECTOR_INICIAL_SALTO_IMPULSO  * (direccion==t_direcciones::IZQUIERDA ? -1.0 : 1.0);
	establecer_vector(v, Movil::V_X);
	establecer_vector(VECTOR_SALTO_IMPULSO, Movil::V_Y);	
}

/*Se comprueba que el jugador puede efectuar un salto con rebote. Se necesita
estar en el aire, una velocidad vertical mínima, estar mirando en la misma
dirección que comprobamos y una velocidad vertical compatible (si está cayendo
tiene que caer poca cosa.*/
 
bool Jugador::es_cumple_condiciones_para_salto_rebote(t_direcciones dir) const
{
	auto v=acc_vector();

	return esta_en_el_aire() &&
		v.y <= VECTOR_VERTICAL_MAXIMO_SALTO_REBOTE && 
		es_encarando_direccion(dir) && 
		fabs(v.x) >= VELOCIDAD_MINIMA_SALTO_REBOTE;
}

void Jugador::realizar_rebote_salto()
{
	cambiar_estado(new Estado_jugador_rebote_salto());
	float d=direccion==t_direcciones::DERECHA ? 1.0 : -1.0;
	float vh=VECTOR_HORIZONTAL_SALTO_REBOTE * d;
	establecer_vector(vh, Movil::V_X);

	float vv=!input.mov_vertical ? VECTOR_VERTICAL_SALTO_REBOTE :
		(input.mov_vertical==-1 ? VECTOR_VERTICAL_SALTO_REBOTE_EXTRA : VECTOR_VERTICAL_SALTO_REBOTE_CORTO);

	establecer_vector(vv, Movil::V_Y);
}

/*Esto se usa cuando bajando de una escalera damos con una celda. 
celda_destino es la celda que queda a los pies.*/

void Jugador::bajar_escalera(const Espaciable& celda_destino)
{
	mut_y_caja(celda_destino.acc_espaciable_y() - acc_espaciable_h());
	establecer_en_suelo(false);
	establecer_vector(0.0, Movil::V_X);
	establecer_vector(0.0, Movil::V_Y);
	iniciar_cooldown_escalera();
}

void Jugador::bajar_escalera_en_aire()
{
	cambiar_estado(new Estado_jugador_aire());
	iniciar_cooldown_escalera();
}

void Jugador::saltar() 
{
	establecer_vector(VECTOR_SALTO, Movil::V_Y); 
	establecer_en_aire();
}

void Jugador::saltar_alto() 
{
	establecer_vector(VECTOR_SALTO_ALTO, Movil::V_Y); 
	cambiar_estado(new Estado_jugador_salto_alto());
}

/*Multiplicador depende de si recibimos input para algún lado o no: al saltar 
desde una escalera se sale con un vector x considerable si se especifica una
dirección.*/

void Jugador::saltar_en_escalera(float multiplicador)
{
	establecer_vector(VECTOR_VERTICAL_SALTO_ESCALERA, Movil::V_Y); 
	establecer_vector(VECTOR_HORIZONTAL_SALTO_ESCALERA * multiplicador, Movil::V_X); 
	cambiar_estado(new Estado_jugador_salto_escalera());
	iniciar_cooldown_escalera();
}

/* El parámetro dir indica la dirección a la que haremos que el jugador mire. 
Esto no está basado en su dirección actual, sino en la dirección relativa con
respecto a la pared que haya chocado.*/

void Jugador::establecer_en_preparar_rebote_salto(short int dir)
{
	cambiar_estado(new Estado_jugador_preparar_rebote_salto());
	establecer_vector(0.0, Movil::V_Y);
	establecer_vector(0.0, Movil::V_X);
	direccion=dir > 0 ? t_direcciones::DERECHA : t_direcciones::IZQUIERDA;
}

/*Este método se llama sólo con input vertical desde el controlador y sirve
para indicar si se cumplen los requisitos para iniciar el impulso.
Siendo esto propio del suelo, hacemos un dynamic cast.
*/

bool Jugador::ha_realizado_input_impulso() const
{
	//TODO: Hmmm... Repitiendo código. Nah, mejor poner una clase padre para ambos
	//y comprobar el estado y hacer un dynamic_cast a una única entidad.
	if(estado->acc_estado()==Estado_jugador::t_estados::SUELO)
	{
		return dynamic_cast<Estado_jugador_suelo *>(estado.get())->ha_realizado_input_impulso();
	}
	else if(estado->acc_estado()==Estado_jugador::t_estados::SALTO_ALTO)
	{
		return dynamic_cast<Estado_jugador_salto_alto *>(estado.get())->ha_realizado_input_impulso();
	}
	return false;

}

void Jugador::turno(float delta)
{
	auto proc=[delta](float &val)
	{
		val-=delta;
		if(val < 0.0) val=0.0;
	};

	if(cooldown_escalera) proc(cooldown_escalera);

	//Realizar el turno del estado, que puede contemplar
	//un cambio a otro estado.

	estado->turno(delta);
	if(estado->es_cambia_estado())
	{
		Estado_jugador::t_estados ne=estado->acc_nuevo_estado();

		switch(ne)
		{
			case Estado_jugador::t_estados::AIRE: establecer_en_aire(); break;
			case Estado_jugador::t_estados::SUELO: establecer_en_suelo(false); break;	//El false es para que no haya cooldown de impulso. Esto es cuando se pone en pie tras clavar un salto.
			default: break;
		}
	}
}

void Jugador::cambiar_estado(Estado_jugador * e)
{
	Estado_jugador::t_caja caja=Estado_jugador::calcular_cambio_estados(*(estado.get()), *e);
	estado.reset(e);
	mut_w_caja(caja.w);
	mut_h_caja(caja.h);
	desplazar_caja(caja.origen.x, caja.origen.y);
}
