#ifndef NIVEL_H
#define NIVEL_H

#include <map>
#include <memory>
#include "celda.h"

/*
El nivel funciona como un contenedor de celdas. Algunas de las celdas podrían
tener significación para colocar actores pero los actores irían en otro
contenedor distinto, para no saturar esta clase.
*/

class Nivel
{
	///////////////////////////
	// Definiciones

	public:

	enum flags_obtener_celdas {F_CON_SOLO_SOLIDAS_ARRIBA=1};

	struct Coordenadas
	{
		unsigned int x;
		unsigned int y;
		Coordenadas():x(0), y(0) {}
	};

	///////////////////////////
	// Propiedades

	private:

	unsigned int w_en_celdas; //Alto y ancho en celdas, no en 
	unsigned int h_en_celdas; //dimensiones del mundo.
	std::map<unsigned int, std::unique_ptr<Celda>> rejilla; //La rejilla se almacena como un mapa, no como un vector.
	Celda_no_valida celda_no_existe; //Esta es la que devolvemos si pedimos una celda que no existe en el mapa.

	///////////////////////////
	// Internas.

	unsigned int calcular_indice_celda(unsigned int, unsigned int) const;
	unsigned int mundo_a_indice_celdas(float) const;
	template<typename T> std::vector<T> celdas_en_caja(const Espaciable::t_caja& caja) const;
	Nivel(const Nivel&)=delete;

	///////////////////////////
	// Interface pública

	public:

	Coordenadas indice_a_coordenadas(unsigned int);
	unsigned int acc_w_en_celdas() const {return w_en_celdas;}
	unsigned int acc_h_en_celdas() const {return h_en_celdas;}
	unsigned int acc_w_en_mundo() const {return w_en_celdas * Celda::DIM_CELDA;}
	unsigned int acc_h_en_mundo() const {return h_en_celdas * Celda::DIM_CELDA;}
	void actualizar_celda(unsigned int x, unsigned int y, unsigned int tipo);
	void actualizar_celda_por_indice(unsigned int indice, unsigned int tipo);
	Celda& obtener_celda_en_rejilla(unsigned int, unsigned int);
	Celda& obtener_celda_en_coordenadas(float, float);
	static DLibH::Punto_2d<int> rejilla_para_coordenadas(float, float);
	const std::vector<const Celda *> celdas_para_caja(const Espaciable::t_caja& caja, bool marcar=false) const;

	void obtener_celdas_representables(std::vector<Representable *>&, const Espaciable::t_caja& caja) const;
	void dimensionar_y_reiniciar(unsigned int w, unsigned int h);

	void debug_limpiar_celdas_consideradas_colision()
	{
		for(const std::pair<const unsigned int, std::unique_ptr<Celda> >& par : rejilla) par.second->debug_establecer_considerada_colision(false);
	}

	Nivel();
	~Nivel();

	Nivel(Nivel&& n):
		w_en_celdas(n.w_en_celdas),
		h_en_celdas(n.h_en_celdas),
		rejilla(std::move(n.rejilla)),
		celda_no_existe(n.celda_no_existe)
	{}

	Nivel& operator=(Nivel&& n)
	{
		w_en_celdas=n.w_en_celdas;
		h_en_celdas=n.h_en_celdas;
		rejilla=std::move(n.rejilla);
		celda_no_existe=n.celda_no_existe;
		return *this;
	}
};

#endif
