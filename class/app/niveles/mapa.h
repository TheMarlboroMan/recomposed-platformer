#ifndef MAPA_H
#define MAPA_H

#include <exception>
#include "nivel.h"
#include "../juego/conexiones.h"
#include <libDan2.h>

/*La clase mapa es el conglomerado de la geometría del nivel y cualquier
otra cosa que pueda contener. El cargador de mapas crearía un mapa (aunque
la interfaz no impide que lo haga el controlador) y eso es lo que el controlador
maneja.
*/

extern DLibH::Log_base LOG;

struct Excepcion_mapa
{
	enum class tipos {ENTRADA_INVALIDA};
	tipos tipo;
	Excepcion_mapa(tipos t):tipo(t){}
};

class Mapa
{
	///////////////
	// Interfaz pública

	public:

	Mapa();
	Mapa(const std::string& nombre_fichero);
	Mapa(Mapa&& m):
		info_camara(std::move(m.info_camara)),
		nivel(std::move(m.nivel)),
		entradas(std::move(m.entradas)),
		salidas(std::move(m.salidas)),
		importado(m.importado)
	{}

	Mapa& operator=(Mapa&& m)
	{
		info_camara=std::move(m.info_camara);
		nivel=std::move(m.nivel);
		entradas=std::move(m.entradas);
		salidas=std::move(m.salidas);
		importado=m.importado;
		return *this;
	}

	~Mapa();


	bool es_valido() const {return importado;}
	const Nivel& acc_nivel() const {return nivel;}
	Nivel& acc_nivel() {return nivel;}
	const Entrada_nivel& obtener_entrada_nivel(unsigned int indice) const;
	Salida_nivel const * const obtener_salida_nivel_colision(const Espaciable& e);
	
	bool es_camara_scroll_x() const {return info_camara.scroll_x;}
	bool es_camara_scroll_y() const {return info_camara.scroll_y;}
	int acc_camara_foco_w() const {return info_camara.foco_w;}
	int acc_camara_foco_h() const {return info_camara.foco_h;}

	///////////////
	// Propiedades

	private:

	struct Info_camara
	{
		bool scroll_x, scroll_y;
		int foco_w, foco_h;
		Info_camara():scroll_x(false), scroll_y(false), foco_w(800), foco_h(600) {}
	} info_camara;

	Nivel nivel;
	std::vector<Entrada_nivel> entradas;
	std::vector<Salida_nivel> salidas;
	bool importado;

	///////////////
	// Métodos
	private:
	Mapa(const Mapa&)=delete;
	std::vector<int> obtener_vector_enteros(const std::string& cadena, const char separador);
	void importar_nivel(const std::string& nombre_fichero);
};

#endif
