#include "mapa.h"
#include <class/lector_txt.h>
#include <source/string_utilidades.h>

using namespace DLibH;

Mapa::Mapa()
	:importado(false)
{

}

Mapa::Mapa(const std::string& nombre_fichero)
	:importado(false)
{
	importar_nivel(nombre_fichero);
}

Mapa::~Mapa()
{

}

const Entrada_nivel& Mapa::obtener_entrada_nivel(unsigned int indice) const
{
	auto it=std::find_if(entradas.begin(), entradas.end(), [indice](const Entrada_nivel& e) {return e.acc_id()==indice;});

	if(it==entradas.end())
	{
		throw Excepcion_mapa(Excepcion_mapa::tipos::ENTRADA_INVALIDA);
	}
	else
	{
		return *it;
	}
}

Salida_nivel const * const Mapa::obtener_salida_nivel_colision(const Espaciable& e)
{
	auto it=std::find_if(salidas.begin(), salidas.end(), [&e](const Salida_nivel& s) {return e.en_colision_con(s);});

	if(it==salidas.end())
	{
		return nullptr;
	}
	else
	{
		return &(*it);
	}
}

std::vector<int> Mapa::obtener_vector_enteros(const std::string& cadena, const char separador)
{
	std::vector<std::string> valores=Herramientas::explotar(cadena, separador);
	std::vector<int> resultado;
	resultado.reserve(valores.size());
	for(const std::string& c : valores) resultado.push_back(std::stoi(c));
	return resultado;
}

void Mapa::importar_nivel(const std::string& nombre_fichero)
{
	Herramientas_proyecto::Lector_txt L(nombre_fichero, '?');

	LOG<<"INFO: Cargando nivel en fichero "<<nombre_fichero<<std::endl;

	if(!L)
	{
		LOG<<"ERROR: Imposible localizar nivel en fichero "<<nombre_fichero<<std::endl;
	}
	else
	{	
		const std::string TIPO_ESTRUCTURA="[ESTRUCTURA]";
		const std::string TIPO_FIN_ESTRUCTURA="[/ESTRUCTURA]";
		const std::string TIPO_INFO="[INFO]";
		const std::string TIPO_REJILLA="[REJILLA]";
		const std::string TIPO_CELDAS="[CELDAS]";
		const std::string TIPO_META="[META]";
		const std::string TIPO_CIERRA_META="[/META]";
		const std::string TIPO_LOGICA="[LOGICA]";
		const std::string TIPO_OBJETOS="[OBJETOS]";

		enum class t_estados {NADA, ESTRUCTURA, INFO, REJILLA, CELDAS, META, FIN_META, LOGICA, OBJETOS, FIN};
		t_estados estado=t_estados::NADA;

		std::string linea;
//		unsigned int indice=0;

		auto toi=[](const std::string& s) {return std::atoi(s.c_str());};

		while(true)
		{
			linea=L.leer_linea();
			if(!L) break;

			if(linea==TIPO_ESTRUCTURA) 
			{
				LOG<<"INFO: Iniciando lectura de lineas ESTRUCTURA"<<std::endl;
				estado=t_estados::ESTRUCTURA;
			}
			else if(linea==TIPO_INFO) 
			{
				LOG<<"INFO: Iniciando lectura de lineas INFO"<<std::endl;
				estado=t_estados::INFO;
			}
			else if(linea==TIPO_REJILLA) 
			{
				LOG<<"INFO: Iniciando lectura de lineas REJILLA"<<std::endl;
				estado=t_estados::REJILLA;
			}
			else if(linea==TIPO_CELDAS) 
			{
				LOG<<"INFO: Iniciando lectura de lineas CELDAS"<<std::endl;
				estado=t_estados::CELDAS;
			}
			else if(linea==TIPO_CIERRA_META) 
			{
				LOG<<"INFO: Iniciando lectura de lineas CIERRA_META"<<std::endl;
				estado=t_estados::FIN_META;
			}
			else if(linea==TIPO_META) 
			{
				LOG<<"INFO: Iniciando lectura de lineas META"<<std::endl;
				estado=t_estados::META;
			}
			else if(linea==TIPO_LOGICA) 
			{
				LOG<<"INFO: Iniciando lectura de lineas LOGICA"<<std::endl;
				estado=t_estados::LOGICA;
			}
			else if(linea==TIPO_OBJETOS) 
			{
				LOG<<"INFO: Iniciando lectura de lineas OBJETOS"<<std::endl;
				estado=t_estados::OBJETOS;
			}
			else if(linea==TIPO_FIN_ESTRUCTURA) 
			{
				LOG<<"INFO: Iniciando lectura de lineas FIN_ESTRUCTURA"<<std::endl;
				estado=t_estados::FIN;
			}

			else
			{
//TODO: Validar longitud de la información siempre.

				switch(estado)
				{
					case t_estados::NADA: break;
					case t_estados::ESTRUCTURA: break;
					case t_estados::INFO: break;
					case t_estados::LOGICA: break;
					case t_estados::FIN_META: break;
					case t_estados::FIN: break;
					case t_estados::REJILLA:
					{
						std::vector<std::string> valores=Herramientas_proyecto::explotar(linea, ',');
						LOG<<"nivel de "<<valores[0]<<" x "<<valores[1]<<std::endl;
						nivel.dimensionar_y_reiniciar(toi(valores[0]), toi(valores[1]));
					}
					case t_estados::CELDAS:
					{
						std::vector<std::string> valores=Herramientas_proyecto::explotar(linea, ' ');
						LOG<<"Iniciando lectura de "<<valores.size()<<" celdas"<<std::endl;

						for(const auto& v : valores)
						{
							const auto& partes=Herramientas_proyecto::explotar(v, ',');
							int x=toi(partes[0]), y=toi(partes[1]), tipo=toi(partes[2]);

							int indice=0;
							switch(tipo)
							{
								default:
								case 1: indice=0; break;
								case 2: indice=Celda::T_SOLIDA; break;
								case 3: indice=Celda::T_SOLIDA_ARRIBA; break;
								case 4: indice=Celda::T_ESCALERA_NORMAL; break;
								case 5: indice=Celda::T_ESCALERA_SOLIDA; break;
								case 6: indice=Celda::T_LETAL; break;
								case 7: indice=Celda::T_RAMPA_SUBE_DI; break;
								case 8: indice=Celda::T_REBOTE_SALTO; break;
								case 9: indice=Celda::T_RAMPA_SUBE_ID; break;
								case 10: indice=Celda::T_CONTROL_RAMPA; break;
							}

							if(indice) nivel.actualizar_celda(x, y, indice);
						}
					}			
					break;
					case t_estados::META:
					{
						auto partes=Herramientas_proyecto::explotar(linea, ':', 1);
						if(partes.size()==2) 
						{
							if(partes[0]=="CAMARA")
							{
								std::vector<std::string> valores=Herramientas_proyecto::explotar(partes[1], ' ');	
								info_camara.scroll_x=std::stoi(valores[0]);
								info_camara.scroll_y=std::stoi(valores[1]);
								info_camara.foco_w=std::stoi(valores[2]);
								info_camara.foco_h=std::stoi(valores[3]);
								LOG<<"scroll camara x:"<<valores[0]<<" scroll camara y:"<<valores[1]<<" => "<<info_camara.scroll_x<<","<<info_camara.scroll_y<<" ["<<info_camara.foco_w<<","<<info_camara.foco_h<<"]"<<std::endl;
							}
						}
					}
					break;

					case t_estados::OBJETOS:
					{
						std::vector<std::string> partes=Herramientas_proyecto::explotar(linea, ',');
						int tipo=toi(partes[0]), x=toi(partes[1]), y=toi(partes[2]);
						switch(tipo)
						{
							//Entrada
							case 1:
								//TODO: Comprobar que ya existe, etc etc...
								if(partes.size()!=6)
								{
									LOG<<"ERROR: Importando entrada, la linea "<<linea<<" no tiene 6 campos."<<std::endl;
								}
								else
								{
									unsigned int id=toi(partes[3]);
									unsigned int direccion=toi(partes[4]);
									unsigned int estado=toi(partes[5]);
									entradas.push_back(Entrada_nivel(x, y, id, direccion, estado));
								}
							break;
	
							case 2:
								if(partes.size()!=5)
								{
									LOG<<"ERROR: Importando entrada, la linea "<<linea<<" no tiene 5 campos."<<std::endl;
								}
								else
								{
									unsigned int nivel=toi(partes[3]);
									unsigned int id_entrada=toi(partes[4]);
									salidas.push_back(Salida_nivel(x, y, id_entrada, nivel));
								}
							break;
							
							default:
								LOG<<"ERROR: Importando objetos, la linea "<<linea<<" no es reconocible."<<std::endl;
							break;
						}
					}
					break;
				}
			}
		}

		importado=true;
	}
}
