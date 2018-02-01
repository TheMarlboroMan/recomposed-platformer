#ifndef REPRESENTABLE_H
#define REPRESENTABLE_H

#include <libDan2.h>
#include <capa_compatibilidad_representaciones_libdansdl2.h>

extern DLibH::Log_base LOG;

class Representador;

/*Todos los elementos que vayan a ser representados como parte de un nivel
deben implementar la interface "representable". Uno de los métodos de la
misma consiste en recibir un Bloque_transformacion_representable y modificarlo
para luego pasarlo al representador.

El Bloque_transformacion_representable es una entidad que existe sólo una vez
para todas las cosas que se representan: es la alternativa a que cada entidad
representable tenga una copia de su representación.
*/

struct Bloque_transformacion_representable
{
	///////////////////////////////////
	// Definiciones...

	enum tipos{
		TR_BITMAP=1,
		TR_GRUPO=2
	};

	///////////////////////////////////
	// Propiedades

	private:

	DLibV::Representacion_bitmap_dinamica rep_bmp;
	DLibV::Representacion_agrupada_dinamica rep_agr;
	DLibV::Representacion * rep;
	
	///////////////////////////////////
	// Interfaz pública

	public:

	Bloque_transformacion_representable()
		:rep_bmp(), 
		rep_agr(true),
		rep(&rep_bmp)
	{
		rep_agr.imponer_alpha();
		rep_agr.imponer_modo_blend();
	}

	~Bloque_transformacion_representable()
	{
		rep_agr.vaciar_grupo();
	}

	void establecer_recorte(unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
	{
		rep->establecer_recorte(x, y, w, h);	
	}

	void establecer_posicion(unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
	{
		rep->establecer_posicion(x, y, w, h);
	}

	SDL_Rect acc_posicion() const 
	{
		return rep->acc_posicion();
	}

	void establecer_tipo(unsigned int t)
	{
		switch(t)
		{
			case TR_BITMAP: 
			default:
				rep_bmp.reiniciar_transformacion();
				rep=&rep_bmp;
			break;

			case TR_GRUPO:
				rep_agr.vaciar_grupo();
				rep=&rep_agr;
			break;
		}

		establecer_alpha(255);
		establecer_blend(DLibV::Representacion::BLEND_ALPHA);
//		establecer_mod_color(255, 255, 255);
	}

	void establecer_alpha(unsigned int v) 
	{
		if(v > 255) v=255;
		rep->establecer_alpha(v);
	}
	void establecer_blend(unsigned int v) {rep->establecer_modo_blend(v);}
//	void establecer_mod_color(unsigned int r, unsigned int g, unsigned int b) {rep->establecer_mod_color(r, g, b);}

	///////////////////
	// Metodos para manipular el grupo de representaciones...

	void insertar_en_grupo(DLibV::Representacion * r) {rep_agr.insertar_representacion(r);}

	///////////////////
	// Métodos para manipular la representación de bitmap...

	void invertir_horizontal(bool v) {rep_bmp.transformar_invertir_horizontal(v);}
	void invertir_vertical(bool v) {rep_bmp.transformar_invertir_vertical(v);}
	void rotar(float v) {rep_bmp.transformar_rotar(v);}
	void especificar_centro_rotacion(float x, float y) {rep_bmp.transformar_centro_rotacion(x, y);}

	void establecer_recurso(unsigned int i) //Por defecto establece el recorte al tamaño del recurso.
	{
		if(!DLibV::Gestor_texturas::comprobar(i)) LOG<<"SOLICITADO RECURSO "<<i<<", NO EXISTENTE"<<std::endl;
		else rep_bmp.establecer_textura(DLibV::Gestor_texturas::obtener(i));
	}
/*
	void establecer_recurso_sin_recortar(unsigned int i)
	{
		if(!DLibV::Gestor_texturas::comprobar(i)) LOG<<"SOLICITADO RECURSO "<<i<<", NO EXISTENTE"<<std::endl;
		else rep_bmp.establecer_textura_sin_recortar(DLibV::Gestor_texturas::obtener(i));
	}
*/
	//Establece un recurso que no tiene porqué estar en el gestor.	
	void establecer_textura_manual(DLibV::Textura * r) {rep_bmp.establecer_textura(r);}

	friend class Representador;
};

//La interface promete que no vas a cambiar nada cuando la representes. 
//Ahora mismo que lo estoy haciendo me parece una buena idea. Ya veremos más
//adelante...

class Representable
{
	public:

	virtual unsigned short int obtener_profundidad_ordenacion()const=0;
	virtual void transformar_bloque(Bloque_transformacion_representable &b)const=0;
	virtual std::string a_cadena()const=0;

	Representable() {}

	virtual ~Representable() {}
};

class Ordenador_representables
{
	public: 

	bool operator()(const Representable* a, const Representable* b) const
	{
		return a->obtener_profundidad_ordenacion() < b->obtener_profundidad_ordenacion();
	}
};

#endif
