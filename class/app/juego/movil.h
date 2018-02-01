#ifndef MOVIL_H
#define MOVIL_H

/*Modelo de cosa que se mueve en el espacio... Lo más probable es que si algo
es móvil también sea "Espaciable". En todo caso, esta es la base para el
cálculo de movimiento.


*/

#include <libDan2.h>

class Movil
{
	//////////////////////
	// Definiciones...

	public:

	enum t_vector {V_X=0, V_Y=1};

	//////////////////////
	// Propiedades...

	private:

	DLibH::Vector_2d<float> vector;

	///////////////////////
	// Para clases derivadas...

	protected:

	//delta: tiempo que ha pasado, vector: referencia a la parte del vector, factor: cantidad de fuerza a aplicar al vector.
	float integrar_vector(float delta, float& vector, float factor);
	void sumar_vector(float, unsigned int);
	void establecer_vector(float, unsigned int);
	float& ref_vector_x() {return vector.x;}
	float& ref_vector_y() {return vector.y;}

	///////////////////////
	// Interface pública.

	public:

	Movil();
	~Movil();

	const DLibH::Vector_2d<float>& acc_vector() const {return vector;}
	DLibH::Vector_2d<float> acc_vector() {return vector;}

	float acc_vector_x() {return vector.x;}
	float acc_vector_y() {return vector.y;}

};

#endif
