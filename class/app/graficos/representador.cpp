#include "representador.h"

Representador::Representador()
{

}

Representador::~Representador()
{

}

unsigned int Representador::generar_vista(
	DLibV::Pantalla& pantalla, 
	DLibV::Camara& camara, 
	const std::vector<Representable *>& v)
{
	unsigned int total=0;

	for(Representable * r : v)
	{
		r->transformar_bloque(bloque_transformacion);
		if(bloque_transformacion.rep->volcar(pantalla, camara)) ++total;
	}

	return total;
}
