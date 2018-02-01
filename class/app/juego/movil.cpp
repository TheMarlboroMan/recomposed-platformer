#include "movil.h"

Movil::Movil()
	:vector(0.0, 0.0)
{

}

Movil::~Movil()
{

}

float Movil::integrar_vector(float delta, float &vec, float factor)
{
	float copia_vector=vec;
	vec+=factor * delta;
	float val=copia_vector + vec;
	return val * 0.5 * delta;
}

void Movil::sumar_vector(float c, unsigned int t)
{
	switch(t)
	{
		case V_X: vector.x+=c; break;
		case V_Y: vector.y+=c; break;
	}
}

void Movil::establecer_vector(float c, unsigned int t)
{
	switch(t)
	{
		case V_X: vector.x=c; break;
		case V_Y: vector.y=c; break;
	}
}
