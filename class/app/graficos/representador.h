#ifndef REPRESENTADOR_H
#define REPRESENTADOR_H

#include "representable.h"

/*El representador funciona de la siguiente forma: desde el controlador 
generamos un array de punteros a "Representables". Se le pasa el array al 
representador que primero lo ordena por profundidad y luego genera un bloque de
transformación que pasa y procesa para cada uno de ellos.

El representador, por supuesto, sólo funciona con aquellas entidades que sean
"Representable", esto es, para todo aquello que forme parte del mundo. HUDS,
mensajes y otras cosas irían representadas de otro modo.
*/

class Representador
{
	/////////////////////////
	// Propiedades
	
	private:

	Bloque_transformacion_representable bloque_transformacion;

	/////////////////////////
	// Internos...

	private:

	/////////////////////////
	// Interface pública

	public:

	unsigned int generar_vista(DLibV::Pantalla&, DLibV::Camara&, const std::vector<Representable *>&);

	Representador();
	~Representador();
};

#endif
