#include "controlador_juego.h"
#include <cassert>

Controlador_juego::Controlador_juego(Director_estados &d, const Herramientas_proyecto::Controlador_argumentos& CARG)
	:Controlador_base(d),
	//TODO: Directamente salta fuera de la aplicación si no se encuentra el fichero...
	cargador_mapas("data/recursos/niveles.txt"),
	TEST_jugador_debe_ajustar_y(false)
{
	//TODO: Poner en constantes...
	camara=DLibV::Camara(0, 0, 800, 600);


	int id_nivel=1, id_entrada=1;

	int pos_nivel=CARG.buscar("-n");
	int pos_entrada=CARG.buscar("-e");

	if(pos_entrada!=-1 || pos_nivel!=-1)
	{
		if(pos_nivel != -1) id_nivel=std::stoi(CARG.acc_argumento(pos_nivel+1));
		if(pos_entrada != -1) id_entrada=std::stoi(CARG.acc_argumento(pos_entrada+1));
	}

	importar_nivel(id_nivel, id_entrada);
}

void Controlador_juego::loop(const Input_base& input, float delta)
{
	if(input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		abandonar_aplicacion();
	}
	else
	{
/*			if(input.es_input_down(Input::I_ESPACIO)) 
		{
			Audio::insertar_sonido(
				DLibA::Estructura_sonido(
				DLibA::Gestor_recursos_audio::obtener_sonido(1), 128));	
		}
*/
		//Aquí se recoge el input del jugador y se procesa en función de su estado.
		procesar_input_jugador(input, delta);

		//Aquí, en función del estado, se realiza el movimiento.
		logica_jugador(delta);

		evaluar_perder_vida();
		evaluar_cambio_nivel();

		std::string cadena_debug;
		auto vj=jugador.acc_vector();
		cadena_debug+="x:"+std::to_string(jugador.acc_espaciable_x())+
			" y:"+std::to_string(jugador.acc_espaciable_y())+"\n"+
			"fx:"+std::to_string(jugador.acc_espaciable_fx())+
			" fy:"+std::to_string(jugador.acc_espaciable_fy())+"\n"+
			"w:"+std::to_string(jugador.acc_espaciable_w())+
			" h:"+std::to_string(jugador.acc_espaciable_h())+"\n"+
			"v("+std::to_string(vj.x)+","+std::to_string(vj.y)+")";

		mut_debug(cadena_debug);
	}
}

void Controlador_juego::evaluar_cambio_nivel()
{
	Salida_nivel const * const salida=mapa.obtener_salida_nivel_colision(jugador);
	if(salida)
	{
		importar_nivel(salida->acc_nivel_comunica(), salida->acc_id());
	}
}

/*En este método se recoge y procesa el input del jugador.

En primer lugar se recoge el input y se pasa al jugador. Luego se efectúa el
paso de turno del jugador, que puede implicar un nuevo estado.

A continuación el control se pasa a un visitante que según el estado actual
y el input puede aún cambiar el estado una vez más. Para algunos cálculos 
más grandes nos apoyamos en variables del visitante que luego inspeccionamos
desde el controlador.

Una vez decidido el estado se establece como invariante para el siguiente
paso: "para este estado y este tiempo haz lo que sea". De esta parte salen
modificados el vector y la dirección del jugador, por ejemplo.

En ningún momento se realiza el movimiento: eso lo dejamos para las lógicas
del jugador.
*/

void Controlador_juego::procesar_input_jugador(const Input_base& input, float delta)
{
	Input_usuario iu=recoger_input_usuario(input);
	jugador.recibir_input(iu);

	//Esto va justo aquí, porque puede comportar un cambio de estado.
	jugador.turno(delta);

	//Según el estado actual y el input, ¿podemos recalcular el estado?.
	//Tenemos este visitante aquí, que lo usaremos para que no se nos pase
	//ni un sólo estado. Podríamos hacer un switch case, pero me gusta la
	//idea de que si añadimos estados nuevos no se van a pasar de largo.	

	class Vis:public Visitante_estado_jugador
	{
		private:

		Jugador& jugador;
		Input_usuario& iu;
		Nivel& nivel;
	
		bool controlar_bajada_escalera;
		bool controlar_subida_escalera;
		bool agacharse;

		public:

		Vis(Jugador& pj, Input_usuario& pi, Nivel& pn):
			jugador(pj), iu(pi), nivel(pn), 
			controlar_bajada_escalera(false), controlar_subida_escalera(false),
			agacharse(false)
		{}

		bool es_controlar_bajada_escalera() const {return controlar_bajada_escalera;}
		bool es_controlar_subida_escalera() const {return controlar_subida_escalera;}
		bool es_agacharse() const {return agacharse;}

		virtual void visitar(Estado_jugador_suelo&) 
		{
			if(iu.mov_horizontal && jugador.ha_realizado_input_impulso()) jugador.establecer_en_impulso(iu.mov_horizontal);
			else if(iu.mov_vertical && !jugador.es_en_escalera()) 
			{
				controlar_subida_escalera=true;
				agacharse=iu.mov_vertical==1;
			}
			//TODO: ¿Realmente procede el puede saltar?
			else if(iu.saltar && jugador.puede_saltar()) jugador.saltar();
		}

		virtual void visitar(Estado_jugador_aire&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
		}

		virtual void visitar(Estado_jugador_escalera&) 
		{
			//TODO: Realmente procede "puede_saltar"?
			if(iu.saltar && jugador.puede_saltar())
			{
				//No se puede saltar "hacia arriba".
				if(iu.mov_vertical==1) jugador.bajar_escalera_en_aire();
				else if(iu.mov_horizontal) jugador.saltar_en_escalera(iu.mov_horizontal);
			}
			else if(iu.mov_horizontal && jugador.es_en_escalera()) controlar_bajada_escalera=true;
		}

		virtual void visitar(Estado_jugador_preparar_rebote_salto&) 
		{
			if(iu.saltar) jugador.realizar_rebote_salto();
		}

		virtual void visitar(Estado_jugador_rebote_salto&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
		}

		virtual void visitar(Estado_jugador_impulso&) 
		{
			//Se puede saltar si al menos una de las celdas que haya abajo es sólida.
			if(iu.saltar) 
			{
				Espaciable::t_caja c=jugador.obtener_caja_debajo();

				//TODO: A retocar cuando haya cosas que no sean celdas.
				std::vector<const Celda *> suelo=nivel.celdas_para_caja(c);

				//TODO: Oh oh... cuando esto no sean celdas, esto va a ser un problema: espera una celda!!!!!
				if(suelo.size() && std::any_of(suelo.begin(), suelo.end(), [](const Celda * c){return c->es_solida() || c->es_solida_solo_arriba();}))
					jugador.establecer_en_salto_impulso();
			}

		}

		virtual void visitar(Estado_jugador_salto_impulso&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
		}

		virtual void visitar(Estado_jugador_salto_impulso_toca_suelo&) 
		{
			if(iu.mov_vertical==1) jugador.clavar_aterrizaje();
			else jugador.establecer_en_suelo(false);
		}

		virtual void visitar(Estado_jugador_agachado_salto&) {}

		virtual void visitar(Estado_jugador_agachado&) 
		{
			if(iu.mov_vertical!=1) jugador.establecer_en_suelo(false);
			//TODO: ¿Controlamos si puede saltar?.
			else if(iu.saltar) jugador.saltar_alto();
		}

		virtual void visitar(Estado_jugador_salto_alto&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
			else if(iu.mov_horizontal && jugador.ha_realizado_input_impulso()) jugador.establecer_en_impulso_salto_alto(iu.mov_horizontal);
		}

		virtual void visitar(Estado_jugador_salto_alto_impulso&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
		}

		virtual void visitar(Estado_jugador_salto_escalera&) 
		{
			if(iu.mov_vertical && !jugador.es_en_escalera()) controlar_subida_escalera=true;
		}

	}vis(jugador, iu, mapa.acc_nivel());

	//Recibimos el visitante. Inspeccionamos si aplicamos los casos que necesitan más información,
	//como bajar o subir de escaleras o agacharse, que no es posible si estamos cerca de una escalera.

	jugador.recibir_visitante_estado(vis);
	if(vis.es_controlar_bajada_escalera()) controlar_bajada_escalera(iu.mov_horizontal);
	else if(vis.es_controlar_subida_escalera()) 
	{
		controlar_subida_escalera(iu.mov_vertical);
		if(!jugador.es_en_escalera() && vis.es_agacharse()) jugador.establecer_en_agachado();
	}

	//Finalmente, según el estado final, procesar el input.
#ifndef NDEBUG
	Estado_jugador::t_estados copia_estado=jugador.obtener_estado();
#endif
	jugador.procesar_estado(delta);
#ifndef NDEBUG
	assert(jugador.obtener_estado()==copia_estado);
#endif
}

/*Importar un nivel comporta dos pasos: si el mapa actual es válido 
ejecutaríamos tareas de limpieza en el mismo. Luego importaríamos el nuevo
mapa a partir del cargador y ajustaríamos posición del jugador, cámara y demás.
*/

void Controlador_juego::importar_nivel(unsigned int indice_nivel, unsigned int indice_entrada)
{
	if(mapa.es_valido())
	{
		//TODO: Tareas de limpieza... Si hay algún puntero relacionado
		//con el mapa lo destruiremos aquí.
	}

	try
	{
		mapa=cargador_mapas.fabricar_mapa_indice(indice_nivel);
//		const Entrada_nivel& entrada=mapa.obtener_entrada_nivel(indice_entrada);
		entrada_actual=mapa.obtener_entrada_nivel(indice_entrada);
		jugador.establecer_posicion(entrada_actual.acc_espaciable_x(), entrada_actual.acc_espaciable_fy()-jugador.acc_espaciable_h());
		//TODO: Encarar al jugador según la salida.
		//TODO: Establecer el estado según la salida.

		camara.establecer_limites(0,0,
			mapa.acc_nivel().acc_w_en_celdas()*Celda::DIM_CELDA, 
			mapa.acc_nivel().acc_h_en_celdas()*Celda::DIM_CELDA);

		//Cada nivel tiene su propio enfoque de la cámara especificado...
		camara.enfocar_a(mapa.acc_camara_foco_w(), mapa.acc_camara_foco_h());
	}
	catch(Excepcion_cargador_mapas &e)
	{
		LOG<<"ERROR: No ha sido posible cargar el nivel el en indice "<<indice_nivel<<std::endl;

		//TODO: Quizás, si luego haya un controlador de sistema sea el controlador el que envíe la señal.
		std::exit(0);
	}
	catch(Excepcion_mapa &e)
	{
		LOG<<"ERROR: No se localiza la entrada "<<indice_entrada<<" en el nivel "<<indice_nivel<<std::endl;
		//TODO: Quizás, si luego haya un controlador de sistema sea el controlador el que envíe la señal.
		std::exit(0);
	}
}

Input_usuario Controlador_juego::recoger_input_usuario(const Input_base& input)
{
	Input_usuario iu;

	if(input.es_input_pulsado(Input::I_DERECHA)) iu.mov_horizontal=1;
	else if(input.es_input_pulsado(Input::I_IZQUIERDA)) iu.mov_horizontal=-1;
	else if(input.es_input_up(Input::I_DERECHA)) iu.keyup_horizontal=1;
	else if(input.es_input_up(Input::I_IZQUIERDA)) iu.keyup_horizontal=-1;

	if(input.es_input_pulsado(Input::I_ARRIBA)) iu.mov_vertical=-1;
	else if(input.es_input_pulsado(Input::I_ABAJO)) iu.mov_vertical=1;

	if(input.es_input_down(Input::I_SALTAR)) iu.saltar=true;

	return iu;
}

/*dir será menor que cero cuando la pared está a la izquierda. Cualquier
otro caso indicará una pared a la derecha.

Los criterios son estrictos: el jugador debe estar en el aire y subiendo (o
al menos no bajando). Todas las celdas con las que ha chocado (se supone que
nos estamos moviendo en el eje X) deben ser rebotables. La celda que esté más
alta debe tener su parte alta en Y mayor que el jugador. La celda más baja 
igual, pero por debajo. Se permite que no haya una celda enmedio :P.
*/

bool Controlador_juego::jugador_puede_rebotar_con_pared(int dir) const
{
	t_direcciones tdir=dir < 0 ? 
		t_direcciones::IZQUIERDA : 
		t_direcciones::DERECHA;

	//Comprobaciones más rápidas...
	if(jugador.es_cumple_condiciones_para_salto_rebote(tdir))	
	{
		//Ahora las comprobaciones más duras...

		//Sólo las celdas son rebotables, no hay un actor rebotable.
		Espaciable::t_caja caja_rebote=jugador.obtener_caja_rebote_salto(tdir);
		std::vector<const Celda *> celdas_rebotables=mapa.acc_nivel().celdas_para_caja(caja_rebote);

		//Todas las celdas deben ser rebotables.
		if(!std::all_of(celdas_rebotables.begin(), celdas_rebotables.end(), [](const Celda * c){return c->es_rebote_salto();}))
		{
			return false;
		}

		//Las ordenamos de más alta a más baja con un one liner. "The value returned indicates whether the element passed as first argument is considered to go before the second".
		struct cmp{bool operator()(const Espaciable * const a, const Espaciable * const b) const {return a->acc_espaciable_y() <= b->acc_espaciable_y();}}comparador;

		//Una vez ordenadas miramos si el jugador está "contenido".
		std::sort(celdas_rebotables.begin(), celdas_rebotables.end(), comparador);
		if(jugador.acc_espaciable_y() < celdas_rebotables.front()->acc_espaciable_y() ||
			jugador.acc_espaciable_fy() > celdas_rebotables.back()->acc_espaciable_fy())
		{
			return false;
		}
		
		return true;
	}

	return false;	
}

/*La lógica del jugador es la que genera realmente el movimiento. Junto con 
la generación del movimiento cabe el caso borderline de que se cambie también
el estado (por ejemplo, se ha terminado el impulso).*/

void Controlador_juego::logica_jugador(float delta)
{
	class Vis:public Visitante_estado_jugador
	{
		private:

		unsigned int flags;

		const unsigned int MODO_ESCALERA=1;
		const unsigned int MODO_IMPULSO=2; //Indica que estás en impulso y que por tanto puede terminarse el estado cuando te frenes.
		const unsigned int EJE_X=4;
		const unsigned int EJE_Y=8;

		public:

		Vis(): flags(0) {}

		bool es_modo_escalera() const {return flags & MODO_ESCALERA;}
		bool es_modo_impulso() const {return flags & MODO_IMPULSO;}
		bool es_con_eje_x() const {return flags & EJE_X;}
		bool es_con_eje_y() const {return flags & EJE_Y;}
	
		virtual void visitar(Estado_jugador_suelo&)			{flags=EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_aire&)			{flags=EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_escalera&)			{flags=MODO_ESCALERA;}
		virtual void visitar(Estado_jugador_preparar_rebote_salto&)	{}
		virtual void visitar(Estado_jugador_rebote_salto&) 		{flags=EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_impulso&) 			{flags=MODO_IMPULSO | EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_salto_impulso&) 		{flags=EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_salto_alto&) 		{flags=EJE_X | EJE_Y;}
		virtual void visitar(Estado_jugador_salto_impulso_toca_suelo&)	{}
		virtual void visitar(Estado_jugador_agachado_salto&)		{}
		virtual void visitar(Estado_jugador_agachado&)			{}
		virtual void visitar(Estado_jugador_salto_alto_impulso&) 	{flags=EJE_X | EJE_Y | MODO_IMPULSO;}
		virtual void visitar(Estado_jugador_salto_escalera&) 		{flags=EJE_X | EJE_Y;}
	}vis;

	jugador.recibir_visitante_estado(vis);

	auto v=jugador.acc_vector();
	float vx=v.x, vy=v.y;

	//Esto es un caso especial de cambio de estado: te has frenado haciendo el impulso.
	if(vis.es_modo_impulso() && fabs(vx) < Estado_jugador::VEL_MAXIMA_SUELO)
	{
		if(jugador.esta_en_el_aire())
		{
			jugador.establecer_en_aire();		
		}
		else
		{
			if(jugador.es_cumple_condiciones_para_clavar_fin_impulso()) jugador.clavar_fin_impulso();
			jugador.establecer_en_suelo(true);
		}
	}

	if(vis.es_modo_escalera())
	{
		if(vy) jugador.desplazar_caja(0.0, vy * delta);
		controlar_colisiones_jugador_escalera();
	}
	else
	{
		Espaciable::t_caja caja_original=jugador.copia_caja();
		if(vis.es_con_eje_x()) movimiento_jugador_suelo_aire_eje_x(delta, v, caja_original);
		if(vis.es_con_eje_y()) movimiento_jugador_suelo_aire_eje_y(delta, v, caja_original);
	}
	
}
	
/*Cuando estamos en una escalera se aplican unas normas ligeramente distintas.
Para empezar, como la escalera es una celda sabemos que la X del jugador va a
estar fija en una posición, de modo que sólo tenemos que encargarnos de las
celdas que haya en lo alto y en lo bajo de la caja de colisión.

La caja de colisión es especial: se una una caja de menor altura que la del 
jugador, para permitir que los "pies" queden colgando y que no sea tan rígido.

El funcionamiento es sencillo: cualquier cosa que NO sea una escalera se 
considera una colisión. Cuando la colisión es "por arriba" nos detenemos.
Cuando es por abajo, nos bajamos de la escalera. */

void Controlador_juego::controlar_colisiones_jugador_escalera()
{
	DLibH::Vector_2d<float> v=jugador.acc_vector();

	//Si no hay vector, a casa.
	if(!v.y) 
	{
		return;
	}
	//Si vamos subiendo y la celda superior no es una escalera, detenemos el movimiento, actuando como una colisión normal.
	else if(v.y < 0)
	{
		Espaciable::t_caja caja=jugador.obtener_caja_escalera();
		//Sólo las celdas pueden ser escalera, ok.
		Celda &c=mapa.acc_nivel().obtener_celda_en_coordenadas(caja.origen.x, caja.origen.y);

		if(!c.es_escalera())
		{
			jugador.mut_y_caja(c.acc_espaciable_fy());
		}
	}
	//Si vamos bajando y la celda inferior no es una escalera, nos bajamos.
	else
	{
		//Empezamos por la caja completa: tiene sentido puesto que podemos
		//estar contra algo sólido (nos paramos) o contra el final de una
		//escalera colgando... Si vemos que lo que hay abajo NO es una 
		//escalera miraremos si es sólido.

		Espaciable::t_caja caja=jugador.copia_caja();
		Celda &c=mapa.acc_nivel().obtener_celda_en_coordenadas(caja.origen.x, caja.origen.y+caja.h);

		if(!c.es_escalera())
		{
			if(c.es_solida())
			{
				jugador.mut_y_caja(c.acc_espaciable_y()-jugador.acc_espaciable_h());
				jugador.establecer_en_suelo(false);
			}
			else
			{
				//Lo que hay en los pies no es sólido, puede que estemos en una escalera
				//colgante... Podemos seguir bajando hasta que esta otra caja, más pequeña,
				//nos tire.
				Espaciable::t_caja caja_e=jugador.obtener_caja_escalera();
				Celda &ce=mapa.acc_nivel().obtener_celda_en_coordenadas(caja_e.origen.x, caja_e.origen.y+caja_e.h);
				if(!ce.es_escalera()) jugador.establecer_en_aire();
			}
		}
	}
}


/*Cuando se recibe una pulsación de "arriba" o "abajo" y el jugador no está
en una escalera controlamos la posibilidad de subir a una. Si el jugador está
completamente dentro de una celda de escalera estableceremos su estado y 
lo centraremos en la escalera en si. 

Hay un detalle a tener en cuenta: si estás en el suelo, con una escalera en la
celda actual y dejas pulsado abajo, alternarías entre los estados de escalera
y soltarse. Esta particularidad la controlamos aquí.*/

void Controlador_juego::controlar_subida_escalera(short int dir)
{
	if(!jugador.puede_subir_escalera()) return;

	Espaciable::t_caja caja=jugador.obtener_caja_escalera();
	float centro=round(caja.origen.x + (caja.w / 2));

	//Para que entremos, todas las celdas deben ser del tipo escalera. No es posible agarrar
	//sólo la parte de abajo o la parte de arriba.
	//Ojo, vamos a comprobar también si hay resultados!!!.
	std::vector<const Celda *> todas_cajas=mapa.acc_nivel().celdas_para_caja(caja);
	if(todas_cajas.size() && std::all_of(todas_cajas.begin(), todas_cajas.end(), [centro](const Celda* c) {return c->es_escalera() && c->acc_espaciable_x() <= centro && centro <= c->acc_espaciable_fx();}))
	{
		//Una última comprobación, si hemos hecho click "abajo", el
		//jugador está en tierra y en la celda de abajo no hay escalera,
		//te vas a tu casa.

		if(jugador.es_en_suelo() && dir > 0)
		{
			Espaciable::t_caja caja_s=jugador.obtener_caja_suelo();
			DLibH::Punto_2d<int> p=Nivel::rejilla_para_coordenadas(caja_s.origen.x, caja_s.origen.y);
			if(p.x >= 0 && p.y >= 0)
			{
				Celda& c=mapa.acc_nivel().obtener_celda_en_rejilla(p.x, p.y+1); //Sacamos la siguiente.
				if(!c.es_escalera()) return;							
			}
		}

		jugador.establecer_en_escalera(*todas_cajas[0]);
	}
}

/*Este método controla cuando, desde lo alto de una escalera, hemos pulsado 
izquierda o derecha. Si tenemos a los pies una escalera sólida por debajo o una
celda sólida vamos a bajar de la escalera. En función de si estamos bajando de
una escalera hacia "arriba" o hacia "abajo" hay dos distancias máximas a las
que podemos estar.*/

void Controlador_juego::controlar_bajada_escalera(short int dir)
{
	//La celda a los pies...
	Espaciable::t_caja caja_s=jugador.obtener_caja_suelo();

	DLibH::Punto_2d<int> p=Nivel::rejilla_para_coordenadas(caja_s.origen.x, caja_s.origen.y);
	if(p.x >= 0 && p.y >= 0)
	{
		Celda& c=mapa.acc_nivel().obtener_celda_en_rejilla(p.x, p.y+1); //Sacamos la siguiente.
		if(c.es_solida() || c.es_escalera_solida()) 	
		{
			if(jugador.comprobar_bajar_escalera_abajo(c)) jugador.bajar_escalera(c);
		}
		else
		{
			//Opción B, el jugador está directamente con los pies en una celda escalera sólida: está al final de una escalera por arriba.
			Celda& ce=mapa.acc_nivel().obtener_celda_en_rejilla(p.x, p.y);
			if(ce.es_escalera_solida())
			{
				if(jugador.comprobar_bajar_escalera_arriba(ce)) jugador.bajar_escalera(ce);
			}
		}			
	}
}

void Controlador_juego::evaluar_perder_vida()
{
	//Evaluar si el jugador está directamente sobre un suelo que le deja frito...
	//Tiene que estar en el suelo y bajo él no debe haber nada bueno. Si hay una sóla
	//cosa buena se salva.

	//TODO: Realmente no me gusta esto... Si hay que cambiar cada vez que añadimos un estado sería un coñaaazo.
	if(jugador.es_en_suelo() || jugador.es_en_agachado_salto())
	{

		//TODO: A retocar cuando haya cosas que no sean celdas.
		Espaciable::t_caja c=jugador.obtener_caja_debajo();
		std::vector<const Celda *> suelo=mapa.acc_nivel().celdas_para_caja(c);
		
		if(suelo.size() && std::all_of(suelo.begin(), suelo.end(), [](const Celda * c){return c->es_letal();}))
			jugador.establecer_posicion(entrada_actual.acc_espaciable_x(), entrada_actual.acc_espaciable_fy()-jugador.acc_espaciable_h());
	}
}

void Controlador_juego::movimiento_jugador_suelo_aire_eje_x(float delta, const DLibH::Vector_2d<float>& v, const Espaciable::t_caja& caja_original)
{
	if(v.x) jugador.desplazar_caja(v.x * delta, 0.0);

	//Sistema de colisión ULTRA SIMPLE, mi movimiento determina mi corrección,
	//sólo vale contra cosas estáticas.

	//TODO: A retocar cuando haya cosas que no sean celdas.

	std::vector<const Celda *> colisiones_x=mapa.acc_nivel().celdas_para_caja(jugador.copia_caja(), true);
	Celda::purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(colisiones_x, caja_original);

	//Quitar rampas, escaleras normales y, si estamos ajustando y, controles de rampa.
	//Es importante quitar los controles cuando estamos en una rampa para permitir subir hasta el final.
	auto it=std::remove_if(colisiones_x.begin(), colisiones_x.end(), [this](const Celda * c) {return c->es_rampa() || c->acc_tipo()==Celda::T_ESCALERA_NORMAL || (TEST_jugador_debe_ajustar_y && c->es_control_rampa() );});
	colisiones_x.erase(it, colisiones_x.end());

	//TODO: Implementar un poco mejor.

	if(colisiones_x.size())
	{
		if(v.x < 0.0)
		{
			int fx=-1;
			for(const Espaciable * e: colisiones_x)
			{
				if(e->acc_espaciable_fx() >= fx)
				{
					fx=e->acc_espaciable_fx();
				}
			}

			if(fx != -1)
			{
				jugador.mut_x_caja(fx);

				if(jugador_puede_rebotar_con_pared(-1))
				{
					jugador.establecer_en_preparar_rebote_salto(1);
				}
				else
				{
					jugador.chocar_con_pared();	
				}
			}
		}
		else if(v.x > 0.0)
		{

			int fx=-1;
			for(const Espaciable * e: colisiones_x)
			{
				if(e->acc_espaciable_x() >= fx)
				{
					fx=e->acc_espaciable_x();
				}
			}

			//TODO: Hacer un buen tratamiento de esto, un método propio: jugador
			//ha chocado con pared o algo así. Dentro podemos comprobar
			//los distintos estados para ver las cosas que puede hacer.
			if(fx != -1)
			{
				jugador.mut_x_caja(fx-jugador.acc_espaciable_w());

				if(jugador_puede_rebotar_con_pared(1))
				{
					jugador.establecer_en_preparar_rebote_salto(-1);
				}
				else
				{	
					jugador.chocar_con_pared();
				}
			}
		}
	}
}

/*Está bien que ese método vaya sólo para el jugador, porque controla
cosas como la gravedad, chocar con suelo, rebotes en paredes y demás
que son propios del jugador.*/

void Controlador_juego::movimiento_jugador_suelo_aire_eje_y(float delta, const DLibH::Vector_2d<float>& v, const Espaciable::t_caja& caja_original)
{
	auto chocar_con_suelo=[this](float fy, bool en_rampa)
	{
		jugador.mut_y_caja(fy-jugador.acc_espaciable_h());

		if(jugador.es_cae_a_velocidad_agacharse())
		{
			jugador.clavar_aterrizaje();
		}
		else if(jugador.es_en_salto_impulso())
		{
			//Establece en el estado "pre choque", que permite clavar el salto.
			jugador.chocar_con_suelo_salto_impulso();
		}
		else
		{
			if(!en_rampa) 
			{
				jugador.chocar_con_suelo();
			}
			//Si estamos en una rampa comprobaremos si no venimos del aire: en caso contrario perderíamos, por ejemplo, el impulso.
			else 
			{
				if(jugador.esta_en_el_aire())
				{
					jugador.chocar_con_suelo();
				}
			}
		}
	};


	//Para un puñado de celdas del tipo rampa comprobar 
	auto comprobar_rampa=[this, chocar_con_suelo](const Espaciable::t_caja& caja, const std::vector<const Celda *>& celdas) -> bool
	{
		float x=caja.origen.x + (caja.w / 2);

		for(const Celda * r : celdas)
		{
			//Esto es "estoy centrado en esa rampa".
			if(r->acc_espaciable_x() <= x && x < r->acc_espaciable_fx())
			{
				float px=ceil(x - r->acc_espaciable_x());	//El ceil nos permite salir de una rampa.
				float y=r->calcular_y_arriba_para_x(px);

				//Esto es "estoy por debajo de la línea de la celda".
				if(caja.origen.y + caja.h >= y)
				{
					chocar_con_suelo(y, true);
					return true;
				}
			}
		}

		return false;
	};

	auto forzar_snap=[this](const Celda& celda_snap, float x) -> bool
	{
		if(celda_snap.es_solida() || celda_snap.es_rampa())
		{
			float px=ceil(x - celda_snap.acc_espaciable_x());	//El ceil nos permite salir de una rampa.
			float fy=celda_snap.calcular_y_arriba_para_x(px);		
			jugador.mut_y_caja(fy-jugador.acc_espaciable_h());
			return true;
		}
	
		return false;
	};

//EXPERIMENTAL RAMPAS... ***************************************

	//TODO: Salto impulso lo traspasa.
	//TODO: Saltar en una rampa contra un control rampa hace que te choques con ellos... Mejor aún, anda hasta el punto de control y salta entonces :D!.
	//TODO: Otro caso freak, saltar en el pico entre dos rampas.

	/*El jugador está dentro de una celda que obliga a hacer ajuste en el
	eje Y. Comprobaremos el punto central - abajo queda por debajo de la 
	línea definida por la celda y haremos snap.*/

	Espaciable::t_caja caja_rampa=jugador.copia_caja();
	std::vector<const Celda *> rampas_y=mapa.acc_nivel().celdas_para_caja(caja_rampa, true);

	auto it=std::remove_if(rampas_y.begin(), rampas_y.end(), [](const Celda * c) {return !c->es_rampa();});
	rampas_y.erase(it, rampas_y.end());

	//TODO: Al estar en salto de impulso y cosas así se traspasa.
	if(rampas_y.size() && v.y >= 0.0)
	{
		if(comprobar_rampa(caja_rampa, rampas_y))
		{
			TEST_jugador_debe_ajustar_y=true;
			return;
		}
		//Hay una parte en rampa y en el turno anterior ajustamos...
		//Podemos presumir que estamos bajando una rampa. En este momento
		//puede ser que la rampa continúe o no.
		else 
		{		
			if(TEST_jugador_debe_ajustar_y)
			{
				float x=jugador.acc_espaciable_x() + (jugador.acc_espaciable_w() / 2);
 				float y=jugador.acc_espaciable_fy();
				Celda& celda_snap=mapa.acc_nivel().obtener_celda_en_coordenadas(x, y);
				if(celda_snap.es_valida())
				{
					if(forzar_snap(celda_snap, x)) return;
				}
				else
				{				
					Celda& celda_snap2=mapa.acc_nivel().obtener_celda_en_rejilla(celda_snap.acc_x(), celda_snap.acc_y()+1);					
					if(celda_snap2.es_valida())
					{
						if(forzar_snap(celda_snap2, x)) return;
					}
				}
			}			
		}		
	}
	//Últimísimo caso de rampa... Estamos en el suelo especial de iniciar 
	//bajada de rampa, comprobamos por tanto que en el centro abajo hay
	//rampa y si es el caso ajustamos.
	else
	{
		std::vector<const Celda *> especiales=mapa.acc_nivel().celdas_para_caja(jugador.obtener_caja_debajo(), true);
		if(	
			especiales.size() && 
			(
				(	
					std::any_of(especiales.begin(), especiales.end(), [](const Celda * c){return c->es_control_rampa();}) 
					&& std::any_of(especiales.begin(), especiales.end(), [](const Celda * c){return c->es_rampa();})
				)
				|| 	
				(	std::all_of(especiales.begin(), especiales.end(), [](const Celda * c){return c->es_rampa();})
					&& especiales.size() > 1
				)				
			)
		)
		{
			float x=jugador.acc_espaciable_x() + (jugador.acc_espaciable_w() / 2);
			float y=jugador.acc_espaciable_fy()+4;
			Celda& celda_snap=mapa.acc_nivel().obtener_celda_en_coordenadas(x, y);
			if(celda_snap.es_rampa())
			{
				if(forzar_snap(celda_snap, x)) 
				{
					TEST_jugador_debe_ajustar_y=true;
					return;
				}
			}
		}
	}

	//Hemos llegado a este punto, asumimos que no debe ajustarse.
	TEST_jugador_debe_ajustar_y=false;

// FIN EXPERIMENTAL RAMPAS... ***********************************

	//Comprobamos si vamos a aplicar la gravedad...
	//Si no hay celdas debajo o todas las que hay abajo dejan pasar (como por ejemplo, una escalera).

	//TODO: A retocar cuando haya cosas que no sean celdas.
	std::vector<const Celda *> celdas_debajo=mapa.acc_nivel().celdas_para_caja(jugador.obtener_caja_debajo());
	Celda::purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(celdas_debajo, caja_original);

	if(!celdas_debajo.size() || std::all_of(celdas_debajo.begin(), celdas_debajo.end(), [](const Celda * c){return c->es_no_solida_para_gravedad();}))
	{
		//Este es el único punto en que cambiamos de andar a caer.
		if(jugador.es_en_suelo()) jugador.establecer_en_aire();
		jugador.accion_gravedad(delta, 1.0);
	}

	if(v.y) jugador.desplazar_caja(0.0, v.y * delta);


	//Se inicia el proceso normal, en el que no hay rampas.
	//TODO: A retocar cuando haya cosas que no sean celdas.
	std::vector<const Celda *> colisiones_y=mapa.acc_nivel().celdas_para_caja(jugador.copia_caja(), true);
	Celda::purgar_vector_de_celdas_solo_solidas_por_arriba_segun_caja(colisiones_y, caja_original);
	Celda::purgar_vector_de_celdas_por_tipo(colisiones_y, Celda::T_ESCALERA_NORMAL);
	Celda::purgar_vector_de_celdas_por_tipo(colisiones_y, Celda::T_RAMPA_SUBE_ID);
	Celda::purgar_vector_de_celdas_por_tipo(colisiones_y, Celda::T_RAMPA_SUBE_DI);

	//TODO: Implementar un poco mejor.

	if(colisiones_y.size())
	{
		if(v.y < 0.0)
		{
			int fy=-1;
			for(const Espaciable * e: colisiones_y)
			{
				if(e->acc_espaciable_fy() >= fy)
				{
					fy=e->acc_espaciable_fy();
				}
			}

			if(fy != -1)
			{
				jugador.mut_y_caja(fy);
				jugador.chocar_con_techo();
			}
		}
		else if(v.y > 0.0)
		{
			int fy=-1;
			for(const Espaciable * e: colisiones_y)
			{
				if(e->acc_espaciable_y() >= fy)
				{
					fy=e->acc_espaciable_y();
				}
			}

			if(fy != -1)
			{
				chocar_con_suelo(fy, false);
			}
		}
	}
}

void Controlador_juego::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(128, 128, 128, 255);

	evaluar_enfoque_camara();

	std::vector<Representable *> vr;
	const auto& c_foco=camara.acc_caja_foco();
	const Espaciable::t_caja caja(c_foco.x, c_foco.y, c_foco.w, c_foco.h);

	mapa.acc_nivel().obtener_celdas_representables(vr, caja);
	vr.push_back(&jugador);

	Ordenador_representables ordenador;
	std::sort(vr.begin(), vr.end(), ordenador);
	representador.generar_vista(pantalla, camara, vr);

	SDL_Rect cp=DLibH::Herramientas_SDL::nuevo_sdl_rect(
		jugador.acc_espaciable_x()-c_foco.x, 
		jugador.acc_espaciable_y()-c_foco.y, 
		jugador.acc_espaciable_w(), 
		jugador.acc_espaciable_h()); 

	DLibV::Representacion_primitiva_caja_estatica CAJA(cp, 255, 0, 0);
	CAJA.establecer_alpha(128);
	CAJA.volcar(pantalla);

}

/*El nivel dictamina si la cámara es estática o no.
Si la cámara va pantalla a pantalla calculamos cuantas celdas caben
en la pantalla por cada dimensión y dividimos el nivel en partes 
iguales. A partir de ahí calculamos la posición de la cámara
según dónde esté el jugador.*/

void Controlador_juego::evaluar_enfoque_camara()
{
	int x=0, y=0;

	const auto& c_foco=camara.acc_caja_foco();

	if(mapa.es_camara_scroll_x())
	{
		x=jugador.acc_espaciable_x()-(c_foco.w / 2 );
	}
	else
	{
		int celdas_por_pantalla_x=ceil((float) c_foco.w / (float) Celda::DIM_CELDA);
		int celdas_mostrando_x=celdas_por_pantalla_x * Celda::DIM_CELDA;
		x=floor(jugador.acc_espaciable_x() / celdas_mostrando_x) * celdas_mostrando_x;
	}

	if(mapa.es_camara_scroll_y())
	{
		y=jugador.acc_espaciable_y()-(c_foco.h / 2);
	}
	else
	{
		int celdas_por_pantalla_y=ceil((float) c_foco.h / (float) Celda::DIM_CELDA);
		int celdas_mostrando_y=celdas_por_pantalla_y * Celda::DIM_CELDA;
		y=floor(jugador.acc_espaciable_y() / celdas_mostrando_y) * celdas_mostrando_y;
	}

	camara.enfocar_a(x, y);
}
