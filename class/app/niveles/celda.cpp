
#include "celda.h"

//TODO TODO...
extern DLibH::Log_base LOG;
//TODO TODO

Tabla_sprites Celda::TREC("data/recortes/bloques.dat");

Celda::Celda(unsigned int px, unsigned int py)
	:Representable(),
	x(px), y(py),
	debug_considerada_colision(false)
{

}

Celda::Celda(const Celda& o)
	:Representable()
{
	x=o.x;
	y=o.y;
}

Celda& Celda::operator=(const Celda& o)
{
	Representable::operator=(o);
	x=o.x;
	y=o.y;
	return *this;
}

Celda::~Celda()
{

}

unsigned short int Celda::obtener_profundidad_ordenacion() const
{
	return 10;
}

void Celda::transformar_bloque(Bloque_transformacion_representable &b) const
{
	Frame_sprites f=TREC.obtener(acc_tipo());
	if(f)
	{
		unsigned int alpha=debug_considerada_colision ? 127 : 255;

		b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
		b.establecer_alpha(alpha);
		b.establecer_recurso(Recursos_graficos::RT_BLOQUES);
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion( (x*DIM_CELDA)+f.desp_x, (y*DIM_CELDA)+f.desp_y, DIM_CELDA, DIM_CELDA);
	}
}

void Celda::purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(std::vector<const Celda *>& celdas, const Espaciable::t_caja& caja)
{
	auto comprobar_pasables_por_arriba=[&caja](const Celda * cel) -> bool
	{
		if(cel->es_solida_solo_arriba())
		{
			return ! Espaciable::es_sobre(caja, cel->copia_caja());
		}
		else return false;
	};

	std::vector<const Celda *>::iterator fin=std::remove_if(celdas.begin(), celdas.end(), comprobar_pasables_por_arriba);
	celdas.erase(fin, celdas.end());
}

void Celda::purgar_vector_de_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo)
{
	auto pred=[tipo](const Celda * cel) -> bool
	{
		return cel->acc_tipo()==tipo;
	};

	std::vector<const Celda *>::iterator fin=std::remove_if(celdas.begin(), celdas.end(), pred);
	celdas.erase(fin, celdas.end());
}

void Celda::purgar_vector_de_celdas_conservar_tipo(std::vector<const Celda *>& celdas, unsigned int tipo)
{
	auto pred=[tipo](const Celda * cel) -> bool
	{
		return cel->acc_tipo()!=tipo;
	};

	std::vector<const Celda *>::iterator fin=std::remove_if(celdas.begin(), celdas.end(), pred);
	celdas.erase(fin, celdas.end());
}

unsigned int Celda::cuenta_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo)
{
	unsigned int resultado=0;
	for(const Celda * c : celdas) resultado+=(unsigned int) c->acc_tipo()==tipo;
	return resultado;
}
