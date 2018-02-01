#ifndef CELDA_H
#define CELDA_H

#include "../graficos/representable.h"
#include "../juego/espaciable.h"
#include "../recursos.h"
#include "../../herramientas_proyecto/tabla_sprites.h"


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
	virtual std::string a_cadena() const {return "CELDA";}

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
	
	void mut_x(unsigned int v) {x=v;}
	void mut_y(unsigned int v) {y=v;}
	
	Celda(unsigned int x, unsigned int y);
	Celda(const Celda&);
	Celda& operator=(const Celda&);
	virtual ~Celda();

	void debug_establecer_considerada_colision(bool v) const {debug_considerada_colision=v;} //LOOOL.
	static void purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(std::vector<const Celda *>& celdas, const Espaciable::t_caja& caja);
	static void purgar_vector_de_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);
	static void purgar_vector_de_celdas_conservar_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);
	static unsigned int cuenta_celdas_por_tipo(std::vector<const Celda *>& celdas, unsigned int tipo);

	virtual unsigned int acc_tipo() const=0;
	
	virtual float calcular_y_arriba_para_x(float x) const {return acc_espaciable_y();}	//Devolvería el y sobre la celda para un x, principalmente para rampas.
	virtual bool es_valida() const {return true;} // {return tipo;}	//Para distinguirla de "celda no existe".
	virtual bool es_escalera() const {return false;} // {return tipo==T_ESCALERA_NORMAL || tipo==T_ESCALERA_SOLIDA;}
	virtual bool es_solida() const {return false;} // {return tipo==T_SOLIDA || tipo==T_REBOTE_SALTO || tipo==T_CONTROL_RAMPA;}
	virtual bool es_letal() const {return false;} // {return tipo==T_LETAL;}
	virtual bool es_escalera_solida() const {return false;}// {return tipo==T_ESCALERA_SOLIDA;}
	virtual bool es_rebote_salto() const {return false;} //{return tipo==T_REBOTE_SALTO;}
	virtual bool es_rampa() const {return false;} //{return tipo==T_RAMPA_SUBE_ID || tipo==T_RAMPA_SUBE_DI;}
	virtual bool es_control_rampa() const {return false;}// {return tipo==T_CONTROL_RAMPA;}
	virtual bool es_solida_solo_arriba() const {return false;} //{return tipo==T_ESCALERA_SOLIDA || tipo==T_SOLIDA_ARRIBA || tipo==T_RAMPA_SUBE_ID || tipo==T_RAMPA_SUBE_DI;}
	//Si tengo una celda de estas debajo no contará como algo sólido para la gravedad.
	virtual bool es_no_solida_para_gravedad() const {return false;}// {return tipo==T_ESCALERA_NORMAL;}
};

class Celda_no_valida:public Celda
{
	public:

	Celda_no_valida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return 0;}
	virtual bool es_valida() const {return false;} 
};

class Celda_solida:public Celda
{
	public:

	Celda_solida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_SOLIDA;}
	virtual bool es_solida() const {return true;}
};

class Celda_solida_arriba:public Celda
{
	public:

	Celda_solida_arriba(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_SOLIDA_ARRIBA;}
	virtual bool es_solida_solo_arriba() const {return true;}
};

class Celda_escalera_normal:public Celda
{
	public:

	Celda_escalera_normal(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_ESCALERA_NORMAL;}
	virtual bool es_escalera() const {return true;}
	virtual bool es_no_solida_para_gravedad() const {return true;}

};

class Celda_escalera_solida:public Celda
{
	public:

	Celda_escalera_solida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_ESCALERA_SOLIDA;}
	virtual bool es_escalera() const {return true;}
	virtual bool es_escalera_solida() const {return true;}
	virtual bool es_solida_solo_arriba() const {return true;}
};

class Celda_letal:public Celda
{
	public:

	Celda_letal(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_LETAL;}
	virtual bool es_letal() const {return true;}
};

class Celda_rebote_salto:public Celda
{
	public:

	Celda_rebote_salto(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_REBOTE_SALTO;}
	virtual bool es_solida() const {return true;}
	virtual bool es_rebote_salto() const {return true;}
};

class Celda_control_rampa:public Celda
{
	public:

	Celda_control_rampa(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_CONTROL_RAMPA;}
	virtual bool es_solida() const {return true;}
	virtual bool es_control_rampa() const {return true;}
};

class Celda_rampa_sube_id:public Celda
{
	public:

	Celda_rampa_sube_id(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_RAMPA_SUBE_ID;}
	virtual bool es_rampa() const {return true;}
	virtual bool es_solida_solo_arriba() const {return true;}
	virtual float calcular_y_arriba_para_x(float x) const {return acc_espaciable_fy() - x;}
};

class Celda_rampa_sube_di:public Celda
{
	public:

	Celda_rampa_sube_di(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_RAMPA_SUBE_DI;}
	virtual bool es_rampa() const {return true;}
	virtual bool es_solida_solo_arriba() const {return true;}
	virtual float calcular_y_arriba_para_x(float x) const {return acc_espaciable_y() + x;}
};

class Factoria_celdas
{
	private:

	Factoria_celdas();
	~Factoria_celdas();

	public:

	//TODO: Shmart phointerx.

	static Celda * fabricar(unsigned int x, unsigned int y, unsigned int tipo)
	{
		switch(tipo)
		{
			case Celda::T_SOLIDA: 		return new Celda_solida(x, y); break;
			case Celda::T_SOLIDA_ARRIBA:	return new Celda_solida_arriba(x, y); break;
			case Celda::T_ESCALERA_NORMAL:	return new Celda_escalera_normal(x, y); break;
			case Celda::T_ESCALERA_SOLIDA:	return new Celda_escalera_solida(x, y); break;
			case Celda::T_LETAL:		return new Celda_letal(x, y); break;
			case Celda::T_REBOTE_SALTO:	return new Celda_rebote_salto(x, y); break;
			case Celda::T_RAMPA_SUBE_ID:	return new Celda_rampa_sube_id(x, y); break;
			case Celda::T_RAMPA_SUBE_DI:	return new Celda_rampa_sube_di(x, y); break;
			case Celda::T_CONTROL_RAMPA:	return new Celda_control_rampa(x, y); break;
			default:
				//TODO...
				return 0;
			break;
		}
	}

	
};
#endif
