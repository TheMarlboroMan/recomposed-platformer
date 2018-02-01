#include "bootstrap_aplicacion.h"

#include "../class/controladores/controlador_juego.h"

using namespace App;

void App::loop_aplicacion(Kernel_app& kernel)
{
	//Declaraciones de herramientas externas.
	Director_estados DI;
	const auto& CARG=kernel.acc_controlador_argumentos();

	//Controladores e interfaces.
	Controlador_juego C_J(DI, CARG);
	Interface_controlador * IC=&C_J;

	//Loop principal.
	while(kernel.loop(*IC))
	{
/*		if(DI.es_cambio_estado())
		{
			switch(DI.acc_estado_actual())
			{
				case Director_estados::t_estados::INTRO: break;
				case Director_estados::t_estados::EJEMPLO: break;
			}

			switch(DI.acc_estado_deseado())
			{
				case Director_estados::t_estados::INTRO: IC=&C_I; break;
				case Director_estados::t_estados::EJEMPLO: IC=&C_E; break;
			}

			DI.confirmar_cambio_estado();
		}
*/
	};
}
