#ifndef ESPACIABLE_H
#define ESPACIABLE_H

/*Interfaz que exige los métodos para definir una caja en 2d y a partir de ahí,
monta el resto. Que defina una caja 2d no significa que realmente haya una
caja por ahí, por ejemplo, las celdas de un nivel no tienen cajas reales, aunque
devuelven una caja calculada. 

Está implícito también que implementa muchas funciones para temas de colisión,
como por ejemplo, saber si es sólido por todos lados o no, obtener información
de colisión y demás. Intentamos separarlo en dos interfaces pero teníamos tanto
el problema del diamante como que son casi la misma cosa. Se quedan, por tanto,
como están.
*/

#include <libDan2.h>

class Espaciable
{
	///////////////////////////////
	// Definiciones

	public:

	typedef DLibH::Caja<float, unsigned int> t_caja;
	enum t{T_X=1, T_Y=2};
	enum direcciones{D_ARRIBA=1, D_DERECHA=2, D_ABAJO=4, D_IZQUIERDA=8};

	///////////////////////////////
	// Propiedades e internas...

	protected:

	//////////////////////////////
	// Interfaz pública.

	public:

	Espaciable();
	~Espaciable();

	float acc_espaciable_x() const {return copia_caja().origen.x;}
	float acc_espaciable_y() const {return copia_caja().origen.y;}
	float acc_espaciable_fx() const {return copia_caja().origen.x+copia_caja().w;}
	float acc_espaciable_fy() const {return copia_caja().origen.y+copia_caja().h;}
	unsigned int acc_espaciable_w() const {return copia_caja().w;}
	unsigned int acc_espaciable_h() const {return copia_caja().h;}
	DLibH::Punto_2d<float> acc_espaciable_posicion() const {return copia_caja().origen;}

	bool en_colision_con(const Espaciable&, bool=false) const;

	//Estos son siempre según la geometría en la que 0.0 es el punto superior
	//a la izquierda.

	bool es_sobre(const Espaciable& otra) const {return es_sobre(copia_caja(), otra.copia_caja());}
	bool es_bajo(const Espaciable& otra) const {return es_bajo(copia_caja(), otra.copia_caja());}
	bool es_a_la_izquierda_de(const Espaciable& otra) const {return es_a_la_izquierda_de(copia_caja(), otra.copia_caja());}
	bool es_a_la_derecha_de(const Espaciable& otra) const {return es_a_la_derecha_de(copia_caja(), otra.copia_caja());}

	static bool es_sobre(const t_caja& una, const t_caja& otra) {return una.origen.y+una.h <= otra.origen.y;}
	static bool es_bajo(const t_caja& una, const t_caja& otra) {return una.origen.y >= otra.origen.y+otra.h;}
	static bool es_a_la_izquierda_de(const t_caja& una, const t_caja& otra) {return una.origen.x+una.w <= otra.origen.x;}
	static bool es_a_la_derecha_de(const t_caja& una, const t_caja& otra) {return una.origen.x >= otra.origen.x+otra.w;}

	void establecer_posicion(float x, float y)
	{
		mut_x_caja(x);	
		mut_y_caja(y);
	}

	////////////////////////////
	// A implementar

	virtual std::string espaciable_info() const=0;
	virtual t_caja copia_caja() const=0;	//Debe devolver una COPIA de la caja.

	//Si hay algún tipo de efecto específico lo registramos aquí. Por defecto no 
	//registramos nada, of course.
	//virtual Datos_colision obtener_datos_colision() const {return Datos_colision();}
	
	virtual void mut_x_caja(float)=0;	//Estos no tienen porqué hacer nada si no hay una caja real.
	virtual void mut_y_caja(float)=0;
	virtual void desplazar_caja(float, float)=0;	//Este desplazaría x e y unidades.
	virtual void mut_w_caja(unsigned int)=0;
	virtual void mut_h_caja(unsigned int)=0;
};

#endif
