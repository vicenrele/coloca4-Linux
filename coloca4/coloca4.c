/*************************************************************************************************
*	Víctor Enrique Elena Redondo.											DNI: 07978966J       *
*                                                                                                *
*   Práctica Linux: coloca4												Interfaces Gráficas		 *
**************************************************************************************************/

#include <glib.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <gtk/gtk.h>


gint delete_event_cb(GtkWidget *w, GdkEventAny *e, gpointer datos);
void destroy_event_cb(GtkWidget *w,gpointer datos);
gint evento_expose(GtkWidget *w,GdkEventExpose *evento,gpointer datos);
gboolean evento_configure(GtkWidget *w,GdkEventConfigure *c,gpointer datos);
GdkPixbuf *create_pixbuf(const gchar *filename); //Icono
gint button_press_event (GtkWidget *w, GdkEventButton *evento);
gboolean ganador();
void inicializar();
void partida_ganada();
void dibujar();
void ayuda(GtkWidget *w,gpointer datos);
void click_ayuda(GtkWidget *w,guint id, gpointer datos);
void salir(GtkWidget *w,gpointer datos);
void click_salir(GtkWidget *w,guint id, gpointer datos);
void nuevo(GtkWidget *w,gpointer datos);
void click_nuevo(GtkWidget *w,guint id, gpointer datos);
static void configuracion (GtkToolItem *boton, gpointer ventana);
void color_ganador(GtkWidget *w,gpointer datos);
static void color_changed_cb(GtkWidget *widget, GtkColorSelection *colorsel );




struct juego_t {
	int jugador;
	int tablero[7][7];
	int salir;
}juego;


struct config_t {
	GtkWidget *tablero;
	GtkWidget *etiqueta, *ventanaAux;
	GdkGC *tablero_gc, *fondo_gc, *P1_gc, *P2_gc;
	GdkColor color_tablero, color_fondo, colorP1, colorP2, color;
	int elegirColor;
	int tam_x, tam_y;	
}config;


int main (int argc, char *argv[]){
	GtkWidget *ventana, *caja;	
	GtkWidget *menu;
	GtkWidget *menu_juego;
	GtkWidget *menu_juego_contenedor;
	GtkWidget *menu_juego_nuevo, *menu_juego_configuracion, *menu_juego_colorGanador, *menu_juego_salir;
	GtkWidget *menu_ayuda;
	GtkWidget *menu_ayuda_contenedor;
	GtkWidget *menu_ayuda_instrucciones;
	
	gtk_init (&argc, &argv); //Inicializar el toolkit
	inicializar();
	config.tam_x=900;
	config.tam_y=600;
	config.elegirColor=6; //La primera partida no se puede elegir color de fichas del ganador
	
	config.color_tablero.red = 0;
	config.color_tablero.blue = 45000;
	config.color_tablero.green = 0;	
	
	config.color_fondo.red = 65535;
	config.color_fondo.blue = 65535;
	config.color_fondo.green = 65535;
	
	config.colorP1.red = 55000;
	config.colorP1.blue = 0;
	config.colorP1.green = 0;
	
	config.colorP2.red = 65535;
	config.colorP2.blue = 65535;
	config.colorP2.green = 65535;
	
	//Ventana principal	
	ventana = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER ( ventana ), 0);
	gtk_window_set_title (GTK_WINDOW ( ventana ), "Coloca 4");
	gtk_window_set_position (GTK_WINDOW ( ventana ), GTK_WIN_POS_CENTER);
	
	gtk_window_set_icon(GTK_WINDOW(ventana), create_pixbuf("game4.png"));
	
	g_signal_connect ( ventana , "destroy",gtk_main_quit, NULL);
	g_signal_connect ( ventana , "delete_event", G_CALLBACK(delete_event_cb), NULL);

	caja = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER ( ventana ), caja);

	//Menu
	menu=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(caja),menu,FALSE,FALSE,0);


	menu_juego=gtk_menu_item_new_with_label(g_locale_to_utf8("Juego",-1,NULL,NULL,NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_juego);
	menu_juego_contenedor=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_juego),menu_juego_contenedor);

	    menu_juego_nuevo=gtk_menu_item_new_with_label(g_locale_to_utf8("Nuevo",-1,NULL,NULL,NULL));
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu_juego_contenedor),menu_juego_nuevo);
	    g_signal_connect(G_OBJECT(menu_juego_nuevo),"activate",G_CALLBACK(nuevo),NULL);
	    
	    menu_juego_configuracion=gtk_menu_item_new_with_label(g_locale_to_utf8("Configuración",-1,NULL,NULL,NULL));
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu_juego_contenedor),menu_juego_configuracion);
	    g_signal_connect(G_OBJECT(menu_juego_configuracion),"activate",G_CALLBACK(configuracion),ventana);
	    	    
	    menu_juego_colorGanador=gtk_menu_item_new_with_label(g_locale_to_utf8("Ganador",-1,NULL,NULL,NULL));
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu_juego_contenedor),menu_juego_colorGanador);
	    g_signal_connect(G_OBJECT(menu_juego_colorGanador),"activate",G_CALLBACK(color_ganador),NULL);
		
	    menu_juego_salir=gtk_menu_item_new_with_label(g_locale_to_utf8("Salir",-1,NULL,NULL,NULL));
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu_juego_contenedor),menu_juego_salir);
	    g_signal_connect(G_OBJECT(menu_juego_salir),"activate",G_CALLBACK(salir),NULL);

	menu_ayuda=gtk_menu_item_new_with_label(g_locale_to_utf8("Ayuda",-1,NULL,NULL,NULL));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_ayuda);
	menu_ayuda_contenedor=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_ayuda),menu_ayuda_contenedor);

	    menu_ayuda_instrucciones=gtk_menu_item_new_with_label(g_locale_to_utf8("Instrucciones",-1,NULL,NULL,NULL));
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu_ayuda_contenedor),menu_ayuda_instrucciones);
	    g_signal_connect(G_OBJECT(menu_ayuda_instrucciones),"activate",G_CALLBACK(ayuda),NULL);

	//Tablero en la ventana principal
	config.tablero=gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(config.tablero),config.tam_x,config.tam_y);
	gtk_box_pack_start(GTK_BOX(caja),config.tablero,TRUE,TRUE,0);	
	gtk_widget_show(config.tablero);
	g_signal_connect(G_OBJECT(config.tablero),"configure_event",G_CALLBACK(evento_configure),NULL);
	g_signal_connect(G_OBJECT(config.tablero),"button_press_event",G_CALLBACK(button_press_event),NULL);
	g_signal_connect(G_OBJECT(config.tablero),"expose_event",G_CALLBACK(evento_expose),NULL);
	gtk_widget_set_events (config.tablero,GDK_BUTTON_PRESS_MASK);
	
	config.ventanaAux=ventana;
	gtk_widget_show_all (ventana);
	gtk_main (); // Entra en el bucle del main de gtk 
	return 0;
}

void destroy_event_cb(GtkWidget *w, gpointer datos){
	gtk_main_quit();
}

gint delete_event_cb(GtkWidget *w, GdkEventAny *e, gpointer datos){
    return FALSE;
}

gint evento_expose(GtkWidget *w,GdkEventExpose *evento,gpointer datos){  //Para el fondo de la ventana principal
	config.fondo_gc = gdk_gc_new(config.tablero->window);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(config.tablero),&config.color_fondo,FALSE,TRUE); //asigna el color
	gdk_gc_set_foreground(config.fondo_gc, &config.color_fondo);
	gdk_draw_rectangle(w->window,config.fondo_gc,TRUE,0,0,w->allocation.width,w->allocation.height);
	dibujar();
	return FALSE;
}

gboolean evento_configure(GtkWidget *w,GdkEventConfigure *c,gpointer datos){
	dibujar();
	return TRUE;
}

GdkPixbuf *create_pixbuf(const gchar *filename){
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}


void dibujar(void){
	int ancho,alto,i,j;
	
	GdkDrawable *d;
	d=config.tablero->window;

	ancho=config.tam_x/9;
	alto=config.tam_y/9;

       	config.P1_gc = gdk_gc_new(config.tablero->window);
		gdk_colormap_alloc_color(gtk_widget_get_colormap(config.tablero),&config.colorP1,FALSE,TRUE); //asigna el color
		gdk_gc_set_foreground(config.P1_gc, &config.colorP1);
		
       	config.P2_gc = gdk_gc_new(config.tablero->window);
		gdk_colormap_alloc_color(gtk_widget_get_colormap(config.tablero),&config.colorP2,FALSE,TRUE); //asigna el color
		gdk_gc_set_foreground(config.P2_gc, &config.colorP2);
						
		config.tablero_gc = gdk_gc_new(config.tablero->window);
		gdk_colormap_alloc_color(gtk_widget_get_colormap(config.tablero),&config.color_tablero,FALSE,TRUE); //asigna el color
		gdk_gc_set_foreground(config.tablero_gc, &config.color_tablero);	
		

	for(i=1;i<8;i++){
		for(j=1;j<8;j++){
			gdk_draw_rectangle(d,config.tablero_gc,TRUE, j*ancho, i*alto, ancho-7, alto-2); 
			
		}
	}
	for(i=1;i<8;i++){
		for(j=1;j<8;j++){
			if(juego.tablero[i-1][j-1]!=0)
			{
				if(juego.tablero[i-1][j-1]==1){	//jugador 1
					gdk_draw_arc(d,config.P1_gc,TRUE,(j*ancho)+10, (i*alto)+10, ancho-30, alto-20, 0,360*64);				
				}
				else if(juego.tablero[i-1][j-1]==2){	//jugador 2  
					gdk_draw_arc(d,config.P2_gc,TRUE,(j*ancho)+10, (i*alto)+10, ancho-30, alto-20, 0,360*64);
				}
			}
		}
	}
}

gint button_press_event (GtkWidget *w, GdkEventButton *evento){
	int xPos,yPos,ancho,alto,i,j;
	
	if (evento->button == 1 && juego.salir==0){
		xPos=(int)evento->x;
		yPos=(int) evento->y;
		ancho=config.tam_x/9;
		alto=config.tam_y/9;
		for(i=1;i<8;i++){
			for(j=1;j<8;j++){
				if( juego.salir==0 && xPos>=((j*ancho)) && yPos>=((i*alto)+1) && xPos<=((j+1)*ancho-8) //donde se pincha
					&& yPos<=((i+1)*alto) && (juego.tablero[i-1][j-1]==6)){
				 	if(juego.jugador==1){	//jugador 1
						juego.tablero[i-1][j-1]=1;
						dibujar();	
						if((i-2)>=0){juego.tablero[i-2][j-1]=6;} //Validar posición para colocar ficha
						if(ganador()==TRUE){
							partida_ganada();
							config.elegirColor=1;
							juego.salir=1;
						}
						juego.jugador=2;
					}
					else if(juego.jugador==2){	//jugador 2
						juego.tablero[i-1][j-1]=2;
						dibujar();
						if((i-2)>=0){juego.tablero[i-2][j-1]=6;}
						if(ganador()==TRUE){
							partida_ganada();
							config.elegirColor=2;
							juego.salir=1;
						}
						juego.jugador=1;
					}
						
				}
			}
		}
	}
}

void inicializar(){
	int i,j;	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			juego.tablero[i][j]=0; //Posición vacía del tablero
							
		}
	}

	for(j=0;j<7;j++){
		juego.tablero[6][j]=6; //Posición válida del tablero
							
	}

	juego.jugador=1;
	juego.salir=0;
	
}

void partida_ganada(){
	GtkWidget *dialog_ganar;
	dialog_ganar=gtk_dialog_new_with_buttons("Enhorabuena!!",NULL, GTK_DIALOG_MODAL,GTK_STOCK_CLOSE,GTK_RESPONSE_CLOSE, NULL);
	if(juego.jugador==1){	
		config.etiqueta=gtk_label_new("Gana el JUGADOR 1 \n\n(Premio!: En menu:Juego->Ganador puede elegir color para sus fichas).");
	}	
	else{
		config.etiqueta=gtk_label_new("Gana el JUGADOR 2 \n\n(Premio!: En menu:Juego->Ganador puede elegir color para sus fichas).");
	}	
	gtk_misc_set_padding(GTK_MISC(config.etiqueta),10,10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_ganar)->vbox),config.etiqueta,TRUE,TRUE,0);
	gtk_widget_show(config.etiqueta);
	g_signal_connect(G_OBJECT(dialog_ganar),"response",G_CALLBACK(click_ayuda),NULL);
	gtk_widget_show(dialog_ganar);
}

void ayuda(GtkWidget *w,gpointer datos){
	GtkWidget *dialog_instrucciones;
	dialog_instrucciones=gtk_dialog_new_with_buttons("Instrucciones",NULL, GTK_DIALOG_MODAL,GTK_STOCK_CLOSE,GTK_RESPONSE_CLOSE, NULL);
	config.etiqueta=gtk_label_new("Gana el jugador que antes haga una línea de 4.\nLas fichas se colocan de abajo a arriba.");
	gtk_misc_set_padding(GTK_MISC(config.etiqueta),10,10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_instrucciones)->vbox),config.etiqueta,TRUE,TRUE,0);
	gtk_widget_show(config.etiqueta);
	g_signal_connect(G_OBJECT(dialog_instrucciones),"response",G_CALLBACK(click_ayuda),NULL);
	gtk_widget_show(dialog_instrucciones);
}

void click_ayuda(GtkWidget *w,guint id, gpointer datos){
	switch(id){
	case GTK_RESPONSE_CLOSE:
	    gtk_widget_destroy(w);
	    break;
	}
}

void nuevo(GtkWidget *w,gpointer datos){
	GtkWidget *dialog_nuevo;
	dialog_nuevo=gtk_dialog_new_with_buttons("Nuevo",NULL, GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, NULL);
	config.etiqueta=gtk_label_new("¿Desea empezar una partida nueva?");
	gtk_misc_set_padding(GTK_MISC(config.etiqueta),10,10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_nuevo)->vbox),config.etiqueta,TRUE,TRUE,0);
	gtk_widget_show(config.etiqueta);
	g_signal_connect(G_OBJECT(dialog_nuevo),"response",G_CALLBACK(click_nuevo),NULL);
	gtk_widget_show(dialog_nuevo);
}

void click_nuevo(GtkWidget *w,guint id, gpointer datos){
	switch(id){
	case GTK_RESPONSE_OK:
	    gtk_widget_destroy(w);
	    inicializar();
	    dibujar();
	    break;
	case GTK_RESPONSE_CANCEL:
	    gtk_widget_destroy(w);
	    break;
	}
}

void color_ganador(GtkWidget *w,gpointer datos){
		gint respuesta;
		GtkWidget *colorseldlg = NULL;
		GtkColorSelection *colorsel;
		GdkColor color_anterior;
		if(config.elegirColor==1)
				color_anterior=config.colorP1; //Para no hacer cambios en el color si se cancela el cuador de dialog
		if(config.elegirColor==2)
				color_anterior=config.colorP2;

		  if (colorseldlg == NULL)
			colorseldlg = gtk_color_selection_dialog_new ("Color de las fichas del ganador");

		  // Conseguir ColorSelection widget 
		  colorsel = GTK_COLOR_SELECTION (GTK_COLOR_SELECTION_DIALOG (colorseldlg)->colorsel);

		  gtk_color_selection_set_has_palette (colorsel, TRUE);

		  //Registro de la señal "color_changed"	
		  g_signal_connect (G_OBJECT (colorsel), "color_changed",G_CALLBACK (color_changed_cb), (gpointer) colorsel); 
		  g_signal_connect (G_OBJECT (colorsel), "color_changed",G_CALLBACK (dibujar), (gpointer) colorsel);
		
		  //Se muestra el dialogo 
		  respuesta = gtk_dialog_run (GTK_DIALOG (colorseldlg));

		  if (respuesta != GTK_RESPONSE_OK){  //Para no hacer cambios en el color si se cancela el cuador de dialog
			if(config.elegirColor==1)
				config.colorP1=color_anterior; 
			if(config.elegirColor==2)
				config.colorP2=color_anterior;
			dibujar();
		  }	
		  gtk_widget_hide (colorseldlg);
}

static void color_changed_cb(GtkWidget *widget, GtkColorSelection *colorsel ){
  GdkColor ncolor;
			
  gtk_color_selection_get_current_color (colorsel, &ncolor);
  if(config.elegirColor==1)
			config.colorP1=ncolor;
  if(config.elegirColor==2)
			config.colorP2=ncolor;    
}

static void configuracion (GtkToolItem *boton, gpointer ventana){
	GtkWidget *dialogo, *marco[2], *combo[2];
	GdkDrawable *d;
	d=config.tablero->window;
	gint ret;
	static int actual_fondo = 0;
	static int actual_fichas = 0;
	
	dialogo = gtk_dialog_new_with_buttons ("Configuración", 
			  GTK_WINDOW (ventana), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK,
			  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			  
	combo[0] = gtk_combo_box_new_text();
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo[0]), "Blanco");
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo[0]), "Gris");
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo[0]), "Negro");
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo[0]), actual_fondo);
	marco[0] = gtk_frame_new ("Elija un color para el fondo");
	gtk_frame_set_shadow_type (GTK_FRAME (marco[0]), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (marco[0]), combo[0]);
	
	combo[1] = gtk_combo_box_new_text();
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo[1]), "Rojas y blancas");
	gtk_combo_box_append_text (GTK_COMBO_BOX (combo[1]), "Negras y amarillas");
	gtk_combo_box_set_active (GTK_COMBO_BOX (combo[1]), actual_fichas);
	marco[1] = gtk_frame_new ("Elija un color para las fichas");
	gtk_frame_set_shadow_type (GTK_FRAME (marco[1]), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (marco[1]), combo[1]);
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialogo)->vbox), marco[0], 0, 0, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialogo)->vbox), marco[1], 0, 0, 0);	
	gtk_widget_show_all(dialogo);
			  
    ret = gtk_dialog_run (GTK_DIALOG (dialogo));
	if (ret == GTK_RESPONSE_OK) {
		switch (gtk_combo_box_get_active (GTK_COMBO_BOX (combo[0]))){
			case 0://Blanco
				config.color_fondo.red = 65535;
				config.color_fondo.blue = 65535;
				config.color_fondo.green = 65535;
				actual_fondo = 0;
				break;
			case 1://Gris
				config.color_fondo.red = 45000;
				config.color_fondo.blue = 45000;
				config.color_fondo.green = 45000;
				actual_fondo = 1;
				break;
			case 2://Negro
				config.color_fondo.red = 0;
				config.color_fondo.blue = 0;
				config.color_fondo.green = 0;
				actual_fondo = 2;
				break;	
		}
		switch (gtk_combo_box_get_active (GTK_COMBO_BOX (combo[1]))){
			case 0://Rojas y Blancas
				config.colorP1.red = 55000;
				config.colorP1.blue = 0;
				config.colorP1.green = 0;
				
				config.colorP2.red = 65535;
				config.colorP2.blue = 65535;
				config.colorP2.green = 65535;
				actual_fichas = 0;
				break;
			case 1://Negras y Amarillas
				config.colorP1.red = 0;
				config.colorP1.blue = 0;
				config.colorP1.green = 0;
				
				config.colorP2.red = 55000;
				config.colorP2.blue = 0;
				config.colorP2.green = 55000;
				actual_fichas = 1;
				break;
		}
		config.fondo_gc = gdk_gc_new(config.tablero->window);
		gdk_colormap_alloc_color(gtk_widget_get_colormap(config.tablero),&config.color_fondo,FALSE,TRUE); //asigna el color
		gdk_gc_set_foreground(config.fondo_gc, &config.color_fondo);
		gdk_draw_rectangle(d,config.fondo_gc,TRUE,0,0,config.ventanaAux->allocation.width,config.ventanaAux->allocation.height); //fondo	
		dibujar();
	}
	gtk_widget_destroy (dialogo);
	
}


void salir(GtkWidget *w,gpointer datos){
	GtkWidget *dialog_salir;
	dialog_salir=gtk_dialog_new_with_buttons("Salir",NULL, GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, NULL);
	config.etiqueta=gtk_label_new("¿Desea salir de Coloca 4?");
	gtk_misc_set_padding(GTK_MISC(config.etiqueta),10,10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog_salir)->vbox),config.etiqueta,TRUE,TRUE,0);
	gtk_widget_show(config.etiqueta);
	g_signal_connect(G_OBJECT(dialog_salir),"response",G_CALLBACK(click_salir),NULL);
	gtk_widget_show(dialog_salir);
}

void click_salir(GtkWidget *w,guint id, gpointer datos){
	switch(id){
	case GTK_RESPONSE_OK:
	    gtk_main_quit();
	    break;
	case GTK_RESPONSE_CANCEL:
	    gtk_widget_destroy(w);
	    break;
	}
}


gboolean ganador(){
	int i, j;

	//comprobación de las filas y columnas
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			if((j+3)<7 && juego.tablero[i][j]==juego.jugador && juego.tablero[i][j+1]==juego.jugador 
				&& juego.tablero[i][j+2]==juego.jugador && juego.tablero[i][j+3]==juego.jugador){							
				return TRUE;
			}

			if((i+3)<7 && juego.tablero[i][j]==juego.jugador && juego.tablero[i+1][j]==juego.jugador 
				&& juego.tablero[i+2][j]==juego.jugador && juego.tablero[i+3][j]==juego.jugador){
				return TRUE;
			}
		}
	}
	//comprobación de las diagonales	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			if((i+3)<7 && (j+3)<7 && juego.tablero[i][j]==juego.jugador && juego.tablero[i+1][j+1]==juego.jugador 
				&& juego.tablero[i+2][j+2]==juego.jugador && juego.tablero[i+3][j+3]==juego.jugador){
				return TRUE;
			}

			if((i+3)<7 && (6-j-3)>=0 && juego.tablero[i][6-j]==juego.jugador && juego.tablero[i+1][6-j-1]==juego.jugador 
				&& juego.tablero[i+2][6-j-2]==juego.jugador && juego.tablero[i+3][6-j-3]==juego.jugador){
				return TRUE;
			}
		}
	}
	return FALSE;	
}






