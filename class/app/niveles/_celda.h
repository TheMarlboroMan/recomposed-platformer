#ifndef CELDA_H
#define CELDA_H

#include "../graficos/representable.h"
#include "../espaciable.h"
#include "../recursos/recursos.h"
#include "../herramientas_proyecto/tabla_sprites.h"


class Celda:public Representable, public Espaciable
{
	///////////////////////////
	// Definiciones

	public:

	static const int DIM_CELDA=32; //Dimensiones de una celda en el mundo.

	enum tipos {
		T_SOLIDA=10,
		T_SOLIDA_ARRIBA=20,
		T_ESCALERA_NORMAL=30,
		T_ESCALERA_SOLIDA=31,
		T_LETAL=40,
		T_REBOTE_SALTO=50,
		T_RAMPA_SUBE_ID=60,
		T_RAMPA_SUBE_DI=65,
		T_CONTROL_RAMPA=70
	};

	//////////////
	// Propiedades

	private:

	static Tabla_sprites TREC;

	unsigned int x; //X e Y en función de rejilla.
	unsigned int y;
	unsigned int tipo;
	unsigned int flags;

	mutable bool debug_considerada_colision;

	//////////////////////////////
	// Métodos internos...

	unsigned int obtener_x_mundo() const {return x*DIM_CELDA;}
	unsigned int obtener_xw_mundo() const {return (x*DIM_CELDA)+DIM_CELDA;}
	unsigned int obtener_y_mundo() const {return y*DIM_CELDA;}
	unsigned int obtener_yh_mundo() const {return (y*DIM_CELDA)+DIM_CELDA;}

	////////////////////
	// Implementando Representable...

	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const;
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "CELDA EN "+DLibH::Herramientas::entero_a_cadena(x)+","+DLibH::Herramientas::entero_a_cadena(y)+" DEL TIPO "+DLibH::Herramientas::entero_a_cadena(tipo);}

	////////////////////
	// Implementando Colisionable / Espaciable...

	virtual std::string espaciable_info() const {return a_cadena();}
	virtual Espaciable::t_caja copia_caja() const {return Espaciable::t_caja(obtener_x_mundo(), obtener_y_mundo(), DIM_CELDA, DIM_CELDA);}
	virtual void mut_x_caja(float) {}	//Prácticamente un hack.
	virtual void mut_y_caja(float) {}
	virtual void mut_w_caja(unsigned int) {}
	virtual void mut_h_caja(unsigned int) {}
	virtual void desplazar_caja(float, float) {}

	////////////////////
	// Interface pública

	public:

	unsigned int acc_x() const {return x;}
	unsigned int acc_y() const {return y;}
	virtual unsigned int acc_tipo() const {return tipo;}
	
	void mut_x(unsigned int v) {x=v;}
	void mut_y(unsigned int v) {y=v;}
	void mut_tipo(unsigned int v) {tipo=v;}
	void mut_flags(unsigned int v) {flags=v;}

	Celda(unsigned int, unsigned int, unsigned int, unsigned int);
	Celda(const Celda&);
	Celda& operator=(const Celda&);
	~Celda();

	void debug_establecer_considerada_colision(bool v) const {debug_considerada_colision=v;} //LOOOL.
	static void purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(std::vector<const Celda *>& celdas, const Espaciable::t_caja& caja);
	static void purgar_vector_de_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);
	static void purgar_vector_de_celdas_conservar_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);
	static unsigned int cuenta_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);

	bool es_valida() const {return tipo;}	//Para distinguirla de "celda no existe".

	bool es_escalera() const {return tipo==T_ESCALERA_NORMAL || tipo==T_ESCALERA_SOLIDA;}
	bool es_solida() const {return tipo==T_SOLIDA || tipo==T_REBOTE_SALTO || tipo==T_CONTROL_RAMPA;}
	bool es_letal() const {return tipo==T_LETAL;}
	bool es_escalera_solida() const {return tipo==T_ESCALERA_SOLIDA;}
	bool es_rebote_salto() const {return tipo==T_REBOTE_SALTO;}
	bool es_rampa() const {return tipo==T_RAMPA_SUBE_ID || tipo==T_RAMPA_SUBE_DI;}
	bool es_control_rampa() const {return tipo==T_CONTROL_RAMPA;}

	bool es_solida_solo_arriba() const {return tipo==T_ESCALERA_SOLIDA || tipo==T_SOLIDA_ARRIBA || tipo==T_RAMPA_SUBE_ID || tipo==T_RAMPA_SUBE_DI;}

	//Si tengo una celda de estas debajo no contará como algo sólido para la gravedad.
	bool es_no_solida_para_gravedad() const {return tipo==T_ESCALERA_NORMAL;}
};

class Celda_no_valida:public Celda
{

};

class Celda_solida:public Celda
{

};

class Celda_solida_arriba:public Celda
{

};

class Celda_escalera_normal:public Celda
{

};

class Celda_escalera_solida:public Celda
{

};

class Celda_letal:public Celda
{

};

class Celda_rebote_salto:public Celda
{

};

class Celda_control_rampa:public Celda
{

};

class Celda_rampa_sube_id:public Celda
{

};

class Celda_rampa_sube_di:public Celda
{

};
#endif
