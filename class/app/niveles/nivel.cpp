#include "nivel.h"

Nivel::Nivel()
	:w_en_celdas(0), h_en_celdas(0),
	celda_no_existe(0,0)
{

}

Nivel::~Nivel()
{
	rejilla.clear();
}

unsigned int Nivel::calcular_indice_celda(unsigned int px, unsigned int py) const {return (py * w_en_celdas) + px;}

void Nivel::actualizar_celda(unsigned int px, unsigned int py, unsigned int pt)
{
	if(px >= w_en_celdas || py >= h_en_celdas)
	{
		LOG<<"ERROR: Se intenta acceder a celda "<<px<<","<<py<<" cuando los limites son "<<w_en_celdas<<","<<h_en_celdas<<std::endl;
	}
	else
	{
		unsigned int indice=calcular_indice_celda(px, py);
		actualizar_celda_por_indice(indice, pt);
	}
}

void Nivel::actualizar_celda_por_indice(unsigned int indice, unsigned int pt)
{
	auto insertar=[this](unsigned int indice, unsigned int pt)
	{
		Coordenadas coor=indice_a_coordenadas(indice);
		rejilla.insert(std::make_pair(indice, std::unique_ptr<Celda> {Factoria_celdas::fabricar(coor.x, coor.y, pt)} ) );
	};

	if(rejilla.count(indice)) 
	{
		rejilla.erase(indice);
		insertar(indice, pt);
	}
	else
	{
		insertar(indice, pt);	
	}
}

Nivel::Coordenadas Nivel::indice_a_coordenadas(unsigned int indice)
{
	Coordenadas resultado;

	resultado.y=floor(indice / w_en_celdas);
	resultado.x=indice % w_en_celdas;

	return resultado;
}

/*Esta es sólo para el mundo de fuera... No es const, devuelve la celda REAL.*/

Celda& Nivel::obtener_celda_en_rejilla(unsigned int px, unsigned int py)
{
	unsigned int indice=calcular_indice_celda(px, py);
	if(rejilla.count(indice)) 
	{
		return * rejilla.at(indice);
	}
	else 
	{
		celda_no_existe.mut_x(px);
		celda_no_existe.mut_y(py);
		//TODO: Maybe fuck this y hacer throw???
		return celda_no_existe;
	}
}

Celda& Nivel::obtener_celda_en_coordenadas(float px, float py)
{
	DLibH::Punto_2d<int> p=rejilla_para_coordenadas(px, py);
	return obtener_celda_en_rejilla(p.x, p.y);
}

DLibH::Punto_2d<int> Nivel::rejilla_para_coordenadas(float px, float py)
{
	int x=floor(px / Celda::DIM_CELDA);
	int y=floor(py / Celda::DIM_CELDA);
	return DLibH::Punto_2d<int>(x, y);
}


void Nivel::obtener_celdas_representables(std::vector<Representable *>& v, const Espaciable::t_caja& caja) const
{
	std::vector<const Celda *> en_vista=celdas_en_caja<const Celda *>(caja);
	//Que le den a esto... Te puedes volver loco con tanto const.
	for(const Celda * c : en_vista) v.push_back( const_cast<Celda *>(c) );
}

/*Especificando ceil hacemos que las colisiones por abajo y por la derecha sean
siempre muy exactas. Especificando floor conseguimos el caso contrario... El 
caso que no escojamos tendremos que tratarlo...*/

unsigned int Nivel::mundo_a_indice_celdas(float val) const
{
	int v=floor(val);
	return floor(v / Celda::DIM_CELDA); //Si W y H tuvieran valores distintos sería un tema.
}

void Nivel::dimensionar_y_reiniciar(unsigned int w, unsigned int h)
{
	rejilla.clear();
	w_en_celdas=w;
	h_en_celdas=h;
}

const std::vector<const Celda *> Nivel::celdas_para_caja(const Espaciable::t_caja& c, bool marcar) const
{
	//Taaanto odio.
	std::vector<const Celda *> resultado=celdas_en_caja<const Celda *>(c);
	if(marcar) for(const Celda * c : resultado) c->debug_establecer_considerada_colision(true);
	return resultado;
}

template<typename T> 
std::vector<T> Nivel::celdas_en_caja(const Espaciable::t_caja& c) const
{
	float punto_inicio_x=c.origen.x;
	float punto_fin_x=c.origen.x+c.w;
	float punto_inicio_y=c.origen.y;
	float punto_fin_y=c.origen.y+c.h;

	/*Acerca de esos ceil y -1... Imaginemos que el mundo es una rejilla
	de 16x16. El objeto del jugador mide 16 de ancho. Si está en la posición
	0 cabe justo en la columna 0, pero el código te dirá que también
	está dentro de la columna 1 puesto que 0+16 (x+w) son 16, que corresponde
	a la columna 1. El -1 se encarga de eso.
	El ceil... Cuando le hemos restado 1, 16 se queda en 15. Si intentamos
	avanzar un poco, 16.2 se queda en 15.2, que no hace colisión. Al darle
	un poco más de caña si que lo hace... Es una mierda, pero funciona.*/

	int ini_x=mundo_a_indice_celdas(punto_inicio_x);
	int fin_x=mundo_a_indice_celdas(ceil(punto_fin_x-1.0));
	int ini_y=0;
	int fin_y=mundo_a_indice_celdas(ceil(punto_fin_y-1.0));

	if(ini_x < 0) ini_x=0;
	if(fin_x < 0) fin_x=0;
	if(ini_y < 0) ini_y=0;

	std::vector<T> resultado;

	do
	{
		ini_y=mundo_a_indice_celdas(punto_inicio_y);
		do
		{
			if(ini_x >= 0 && ini_y >= 0) 
			{			
				unsigned int indice=calcular_indice_celda(ini_x, ini_y);

				if(rejilla.count(indice)) 
				{
					resultado.push_back( ( rejilla.at(indice).get()));
				}
			}
			++ini_y;

		}while(ini_y <= fin_y);

		++ini_x;
	}while(ini_x <= fin_x);

	return resultado;
}
