//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_ambientLightRedSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLightRed = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_ambientLightGreenSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLightGreen = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_ambientLightBlueSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLightBlue = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_antialiasingSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAntialiasing = int(((Fl_Slider *)o)->value());
}
void TraceUI::cb_adaptiveThresholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAdaptiveThreshold = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_jitterSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nJitter = int(((Fl_Slider *)o)->value());
}
void TraceUI::cb_constantAttenuationCoeffSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nConstantAttenuationCoefficient = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_linearAttenuationCoeffSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nLinearAttenuationCoefficient = double(((Fl_Slider *)o)->value());
}
void TraceUI::cb_quadraticAttenuationCoeffSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nQuadraticAttenuationCoefficient = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->setDepth(pUI->getDepth());
				pUI->raytracer->setAmbientLightRed(pUI->getAmbientLightRed());
				pUI->raytracer->setAmbientLightGreen(pUI->getAmbientLightGreen());
				pUI->raytracer->setAmbientLightBlue(pUI->getAmbientLightBlue());
				pUI->raytracer->setAntialiasing(pUI->getAntialiasing());
				pUI->raytracer->setJitter(pUI->getJitter());
				pUI->raytracer->setAdaptiveThreshold(pUI->getAdaptiveThreshold());
				pUI->raytracer->setConstantAttenuationCoefficient(pUI->getConstantAttenuationCoefficient());
				pUI->raytracer->setLinearAttenuationCoefficient(pUI->getLinearAttenuationCoefficient());
				pUI->raytracer->setQuadraticAttenuationCoefficient(pUI->getQuadraticAttenuationCoefficient());
				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getAmbientLightRed()
{
	return m_nAmbientLightRed;
}

double TraceUI::getAmbientLightBlue()
{
	return m_nAmbientLightBlue;
}

double TraceUI::getAmbientLightGreen()
{
	return m_nAmbientLightGreen;
}

int TraceUI::getAntialiasing()
{
	return m_nAntialiasing;
}

double TraceUI::getAdaptiveThreshold()
{
	return m_nAdaptiveThreshold;
}

int TraceUI::getJitter()
{
	return m_nJitter;
}

double TraceUI::getConstantAttenuationCoefficient()
{
	return m_nConstantAttenuationCoefficient;
}

double TraceUI::getLinearAttenuationCoefficient()
{
	return m_nLinearAttenuationCoefficient;
}

double TraceUI::getQuadraticAttenuationCoefficient()
{
	return m_nQuadraticAttenuationCoefficient;
}


// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nAmbientLightRed = 0.2;
	m_nAmbientLightGreen = 0.2;
	m_nAmbientLightBlue = 0.2;
	m_nAntialiasing = 1;
	m_nJitter = 0;
	m_nAdaptiveThreshold = 0;
	m_nConstantAttenuationCoefficient = 0.0;
	m_nLinearAttenuationCoefficient = 0.0;
	m_nQuadraticAttenuationCoefficient = 0.0;

	m_mainWindow = new Fl_Window(100, 40, 320, 500, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		m_ambientLightRedSlider = new Fl_Value_Slider(10, 80, 180, 20, "Ambient Red");
		m_ambientLightRedSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightRedSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightRedSlider->labelfont(FL_COURIER);
		m_ambientLightRedSlider->labelsize(12);
		m_ambientLightRedSlider->minimum(0);
		m_ambientLightRedSlider->maximum(1);
		m_ambientLightRedSlider->step(0.01);
		m_ambientLightRedSlider->value(m_nAmbientLightRed);
		m_ambientLightRedSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightRedSlider->callback(cb_ambientLightRedSlides);

		m_ambientLightGreenSlider = new Fl_Value_Slider(10, 105, 180, 20, "Ambient Green");
		m_ambientLightGreenSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightGreenSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightGreenSlider->labelfont(FL_COURIER);
		m_ambientLightGreenSlider->labelsize(12);
		m_ambientLightGreenSlider->minimum(0);
		m_ambientLightGreenSlider->maximum(1);
		m_ambientLightGreenSlider->step(0.01);
		m_ambientLightGreenSlider->value(m_nAmbientLightGreen);
		m_ambientLightGreenSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightGreenSlider->callback(cb_ambientLightGreenSlides);

		m_ambientLightBlueSlider = new Fl_Value_Slider(10, 130, 180, 20, "Ambient Blue");
		m_ambientLightBlueSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientLightBlueSlider->type(FL_HOR_NICE_SLIDER);
		m_ambientLightBlueSlider->labelfont(FL_COURIER);
		m_ambientLightBlueSlider->labelsize(12);
		m_ambientLightBlueSlider->minimum(0);
		m_ambientLightBlueSlider->maximum(1);
		m_ambientLightBlueSlider->step(0.01);
		m_ambientLightBlueSlider->value(m_nAmbientLightBlue);
		m_ambientLightBlueSlider->align(FL_ALIGN_RIGHT);
		m_ambientLightBlueSlider->callback(cb_ambientLightBlueSlides);

		m_antialiasingSlider = new Fl_Value_Slider(10, 155, 180, 20, "Antialising");
		m_antialiasingSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_antialiasingSlider->type(FL_HOR_NICE_SLIDER);
		m_antialiasingSlider->labelfont(FL_COURIER);
		m_antialiasingSlider->labelsize(12);
		m_antialiasingSlider->minimum(1);
		m_antialiasingSlider->maximum(5);
		m_antialiasingSlider->step(1);
		m_antialiasingSlider->value(m_nAntialiasing);
		m_antialiasingSlider->align(FL_ALIGN_RIGHT);
		m_antialiasingSlider->callback(cb_antialiasingSlides);

		m_adaptiveThresholdSlider = new Fl_Value_Slider(10, 180, 180, 20, "Adaptive Termination");
		m_adaptiveThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_adaptiveThresholdSlider->type(FL_HOR_NICE_SLIDER);
		m_adaptiveThresholdSlider->labelfont(FL_COURIER);
		m_adaptiveThresholdSlider->labelsize(12);
		m_adaptiveThresholdSlider->minimum(0);
		m_adaptiveThresholdSlider->maximum(1);
		m_adaptiveThresholdSlider->step(0.05);
		m_adaptiveThresholdSlider->value(m_nAdaptiveThreshold);
		m_adaptiveThresholdSlider->align(FL_ALIGN_RIGHT);
		m_adaptiveThresholdSlider->callback(cb_adaptiveThresholdSlides);

		m_jitterSlider = new Fl_Value_Slider(10, 205, 180, 20, "Jittering");
		m_jitterSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_jitterSlider->type(FL_HOR_NICE_SLIDER);
		m_jitterSlider->labelfont(FL_COURIER);
		m_jitterSlider->labelsize(12);
		m_jitterSlider->minimum(0);
		m_jitterSlider->maximum(1);
		m_jitterSlider->step(1);
		m_jitterSlider->value(m_nJitter);
		m_jitterSlider->align(FL_ALIGN_RIGHT);
		m_jitterSlider->callback(cb_jitterSlides);

		m_constantAttenuationCoeffSlider = new Fl_Value_Slider(10, 230, 180, 20, "Constant Attenuation Coeff");
		m_constantAttenuationCoeffSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_constantAttenuationCoeffSlider->type(FL_HOR_NICE_SLIDER);
		m_constantAttenuationCoeffSlider->labelfont(FL_COURIER);
		m_constantAttenuationCoeffSlider->labelsize(12);
		m_constantAttenuationCoeffSlider->minimum(0);
		m_constantAttenuationCoeffSlider->maximum(1);
		m_constantAttenuationCoeffSlider->step(0.001);
		m_constantAttenuationCoeffSlider->value(m_nConstantAttenuationCoefficient);
		m_constantAttenuationCoeffSlider->align(FL_ALIGN_RIGHT);
		m_constantAttenuationCoeffSlider->callback(cb_constantAttenuationCoeffSlides);

		m_linearAttenuationCoeffSlider = new Fl_Value_Slider(10, 255, 180, 20, "Linear Attenuation Coeff");
		m_linearAttenuationCoeffSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_linearAttenuationCoeffSlider->type(FL_HOR_NICE_SLIDER);
		m_linearAttenuationCoeffSlider->labelfont(FL_COURIER);
		m_linearAttenuationCoeffSlider->labelsize(12);
		m_linearAttenuationCoeffSlider->minimum(0);
		m_linearAttenuationCoeffSlider->maximum(1);
		m_linearAttenuationCoeffSlider->step(0.001);
		m_linearAttenuationCoeffSlider->value(m_nLinearAttenuationCoefficient);
		m_linearAttenuationCoeffSlider->align(FL_ALIGN_RIGHT);
		m_linearAttenuationCoeffSlider->callback(cb_linearAttenuationCoeffSlides);

		m_quadraticAttenuationCoeffSlider = new Fl_Value_Slider(10, 280, 180, 20, "Quadratic Attenuation Coeff");
		m_quadraticAttenuationCoeffSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_quadraticAttenuationCoeffSlider->type(FL_HOR_NICE_SLIDER);
		m_quadraticAttenuationCoeffSlider->labelfont(FL_COURIER);
		m_quadraticAttenuationCoeffSlider->labelsize(12);
		m_quadraticAttenuationCoeffSlider->minimum(0);
		m_quadraticAttenuationCoeffSlider->maximum(1);
		m_quadraticAttenuationCoeffSlider->step(0.001);
		m_quadraticAttenuationCoeffSlider->value(m_nQuadraticAttenuationCoefficient);
		m_quadraticAttenuationCoeffSlider->align(FL_ALIGN_RIGHT);
		m_quadraticAttenuationCoeffSlider->callback(cb_quadraticAttenuationCoeffSlides);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}