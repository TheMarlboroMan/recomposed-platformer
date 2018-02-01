#ifndef CONEXIONES_H
#define CONEXIONES_H

#include "actor.h"


class Base_entrada_salida:public Actor
{
	//////////////////////
	// Interfaz pública...

	public:

	Base_entrada_salida(float x, float y, unsigned int pid)
		:Actor(x, y, W, H), id(pid)
	{}

	unsigned int acc_id() const {return id;}

	///////////////////////
	// Propiedades...

	private:

	unsigned int id;	//Id de entrada o id con la entrada que conecta, indiferente.

	///////////////////////
	// Definiciones...

	protected:

	static const int W=32;
	static const int H=32;

	///////////////////////
	// Implementación de actor...

	private:

	virtual float obtener_peso() const {return 0.0;}
};

class Entrada_nivel:public Base_entrada_salida
{
	/////////////////
	// Interfaz pública...

	public:	

	Entrada_nivel(float x, float y, unsigned int pid, unsigned int pdir, unsigned int pest)
		:Base_entrada_salida(x, y, pid),
		direccion(pdir), estado(pest)
	{}

	Entrada_nivel()
		:Base_entrada_salida(0, 0, 0),
		direccion(0), estado(0)
	{}

	unsigned int acc_direccion() const {return direccion;}
	unsigned int acc_estado() const {return estado;}

	/////////////////
	// Implementación de espaciable...

	virtual std::string espaciable_info() const {return "ENTRADA NIVEL "+std::to_string(acc_id());}

	///////////////
	// Propiedades.

	private:

	unsigned int direccion;
	unsigned int estado;

	
};

class Salida_nivel:public Base_entrada_salida
{
	////////////////////
	// Interfaz pública...

	public:

	Salida_nivel(float x, float y, unsigned int pid, unsigned int pniv)
		:Base_entrada_salida(x, y, pid),
		nivel_comunica(pniv)
	{}

	unsigned int acc_nivel_comunica() const {return nivel_comunica;}	

	/////////////////
	// Implementación de espaciable...

	virtual std::string espaciable_info() const {return "SALIDA NIVEL PARA "+std::to_string(nivel_comunica)+" :: "+std::to_string(acc_id());}

	/////////////////////
	// Propiedades

	unsigned int nivel_comunica;	
};

#endif
