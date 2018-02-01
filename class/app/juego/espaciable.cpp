#include "espaciable.h"

Espaciable::Espaciable()
{

}

Espaciable::~Espaciable()
{

}

bool Espaciable::en_colision_con(const Espaciable& e, bool roce_es_colision) const
{
	return copia_caja().es_en_colision_con(e.copia_caja(), roce_es_colision);
}
