#ifndef CARGADOR_MAPAS_H
#define CARGADOR_MAPAS_H

/*El cargador de mapas se usará mantener la cuenta de los mapas existentes
mediante el archivo de niveles y de fabricar niveles pasando el id adecuado.
*/

#include <map>
#include <stdexcept>
#include <libDan2.h>
#include "../niveles/mapa.h"

extern DLibH::Log_base LOG;

struct Excepcion_cargador_mapas
{
	enum class tipos {INDICE_INCORRECTO};
	tipos tipo;
	Excepcion_cargador_mapas(tipos t):tipo(t) {}
};

class Cargador_mapas
{
	////////////////////
	// Definiciones 

	private:

	typedef std::map<unsigned int, std::string> t_mapa;

	////////////////////
	// Propiedades.

	private:

	t_mapa niveles;

	////////////////////
	// Métodos.

	private:

	void cargar(const std::string& ruta_listado);

	public:

	Cargador_mapas(const std::string& ruta_listado);
	~Cargador_mapas();

	Mapa fabricar_mapa_indice(unsigned int indice) const;
};

#endif
