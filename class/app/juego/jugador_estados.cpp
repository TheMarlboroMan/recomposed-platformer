#include "jugador_estados.h"

const float Estado_jugador::COOLDOWN_IMPULSO=0.500;
const float Estado_jugador::COOLDOWN_AGACHADO_SALTO=0.700;
const float Estado_jugador::DECELERACION_IMPULSO=1200.0;
const float Estado_jugador::MARGEN_REBOTE_SALTO=0.8;
const float Estado_jugador::VELOCIDAD_ESCALERA=100.0;
const float Estado_jugador::VEL_MINIMA_SUELO=20.0;
const float Estado_jugador::VEL_MAXIMA_SUELO=120.0;
const float Estado_jugador::ACELERACION_SALTO=50.0;
const float Estado_jugador::VELOCIDAD_FRENO_MANUAL=600.0;
const float Estado_jugador::ACELERACION_SUELO=140.0;
const float Estado_jugador::DECELERACION_SUELO=300.0;
const float Estado_jugador::MARGEN_IMPULSO=0.2;
const float Estado_jugador::ACELERACION_SALTO_ALTO=80.0;
const float Estado_jugador::VEL_MAXIMA_HORIZONTAL_SALTO_ALTO=30.0;
const float Estado_jugador::PESO_DEFECTO=500.0;
const float Estado_jugador::DECELERACION_SALTO_ESCALERA=300.0;

/* Se procesa sólo el input horizontal. Si no se toca nada se va frenando 
lentamente. Si se mueve en la misma dirección se acelera, si se mueve en la
dirección contraria no se produce un cambio de dirección al momento, sino que
es necesario frenar hasta una velocidad mínima.*/

void Estado_jugador_suelo::procesar(float delta)
{
	auto input=acc_input();

	//Si acabamos de levantar una tecla horizontal las acumulamos..
	if(input.keyup_horizontal)
	{
		if(!dir_keyup_impulso)
		{
			dir_keyup_impulso=input.keyup_horizontal;
			margen_impulso=MARGEN_IMPULSO;
		}
	}

	//Esto no es elseif... Hemos hecho un keyup pero no impide que por otro lado haya un keydown.

	float vx=acc_vector().x;
	float v=fabs(vx);

	if(!input.mov_horizontal && vx)
	{		
		v-=DECELERACION_SUELO * delta;
		if(v < 0.0) v=0.0;
		else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO; //En efecto, para controlar la salida del salto largo.
		v*= vx > 0.0 ? 1.0 : -1.0;
	}
	else if(input.mov_horizontal)
	{
		bool misma_direccion=(vx >= 0.0 && input.mov_horizontal > 0) || (vx <= 0.0 && input.mov_horizontal < 0);

		//En la misma dirección aceleraremos.
		if(misma_direccion)
		{
			v+=delta * ACELERACION_SUELO;
			if(v < VEL_MINIMA_SUELO) v=VEL_MINIMA_SUELO;
			else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO;
			v*=(float)input.mov_horizontal;
		}
		//En la dirección contraria frenaremos. Si llegamos a la velocidad mínima invertimos el sentido de la marcha.
		else
		{
			v-=delta * VELOCIDAD_FRENO_MANUAL;
			if(v < VEL_MINIMA_SUELO) //Ahora es posible dar la vuelta, de modo que ponemos el vector apuntando hacia la dirección deseada.
			{
				v=VEL_MINIMA_SUELO;
				v*=(float)input.mov_horizontal;
			}
			else //Aún no es posible dar la vuelta. Ponemos al vector apuntando hacia el vector actual.
			{
				v*=vx < 0.0 ? -1.0 : 1.0;
			}
		}
	}

	establecer_vector_x(v);
	if(v) establecer_direccion(v > 0.0 ? t_direcciones::DERECHA : t_direcciones::IZQUIERDA);
}

/*Durante el salto no se frena si soltamos el input, pero podemos ajustar el
vector a la mitad de la velocidad normal. No se permite el cambio de dirección.
Se implica con esto que el salto más largo es el que se hace con toda la 
carrerilla puesto que la aceleración durante el salto es peor.*/

void Estado_jugador_aire::procesar(float delta)
{
	auto input=acc_input();
	auto vec=acc_vector();

	if(input.mov_horizontal)
	{
		float vx=vec.x;
		float v=fabs(vx);
		float factor=delta * ACELERACION_SALTO;
		bool misma_direccion=(vx >= 0.0 && input.mov_horizontal > 0) || (vx <= 0.0 && input.mov_horizontal < 0);

		if(misma_direccion)
		{
			v+=factor;
			if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO;	//Se usa la velocidad máxima del suelo también.
			v*=(float)input.mov_horizontal;
		}
		else
		{
			v-=factor;
			if(v < VEL_MINIMA_SUELO) //Ahora es posible dar la vuelta, de modo que ponemos el vector apuntando hacia la dirección deseada.
			{
				v=VEL_MINIMA_SUELO;
				v*=(float)input.mov_horizontal;
			}
			else //Aún no es posible cambiar la dirección.
			{
				v*=vx < 0.0 ? -1.0 : 1.0;
			}
		}

		establecer_vector_x(v);
	}
}

/*Como un salto normal, pero con menos aceleración aún...*/

void Estado_jugador_salto_alto::procesar(float delta)
{
	auto input=acc_input();
	auto vec=acc_vector();

	//Si acabamos de levantar una tecla horizontal las acumulamos..
	if(input.keyup_horizontal)
	{
		if(!dir_keyup_impulso)
		{
			dir_keyup_impulso=input.keyup_horizontal;
			margen_impulso=MARGEN_IMPULSO;
		}
	}

	//Esto no es elseif... Hemos hecho un keyup pero no impide que por otro lado haya un keydown.

	if(input.mov_horizontal)
	{
		float vx=vec.x;
		float v=fabs(vx);
		float factor=delta * ACELERACION_SALTO_ALTO;
		bool misma_direccion=(vx >= 0.0 && input.mov_horizontal > 0) || (vx <= 0.0 && input.mov_horizontal < 0);

		if(misma_direccion)
		{
			v+=factor;
			if(v > VEL_MAXIMA_HORIZONTAL_SALTO_ALTO) v=VEL_MAXIMA_HORIZONTAL_SALTO_ALTO;
			v*=(float)input.mov_horizontal;
		}
		else
		{
			//TODO: El factor sería mayor aquí siempre que estemos aún viajando en la misma dirección?????.

			v-=factor;
			if(v < VEL_MINIMA_SUELO) //Ahora es posible dar la vuelta, de modo que ponemos el vector apuntando hacia la dirección deseada.
			{
				v=VEL_MINIMA_SUELO;
				v*=(float)input.mov_horizontal;
			}
			else //Aún no es posible cambiar la dirección.
			{
				v*=vx < 0.0 ? -1.0 : 1.0;
			}
		}

		establecer_vector_x(v);
	}
}
