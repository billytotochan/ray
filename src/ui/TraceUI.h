//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_ambientLightRedSlider;
	Fl_Slider*			m_ambientLightGreenSlider;
	Fl_Slider*			m_ambientLightBlueSlider;
	Fl_Slider*			m_antialiasingSlider;
	Fl_Slider*			m_adaptiveThresholdSlider;
	Fl_Slider*			m_jitterSlider;
	Fl_Slider*			m_constantAttenuationCoeffSlider;
	Fl_Slider*			m_linearAttenuationCoeffSlider;
	Fl_Slider*			m_quadraticAttenuationCoeffSlider;
	Fl_Slider*	m_superSamplingSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double			getAmbientLightRed();
	double			getAmbientLightGreen();
	double			getAmbientLightBlue();
	int		getAntialiasing();
	double			getAdaptiveThreshold();
	int		getJitter();
	double		getConstantAttenuationCoefficient();
	double		getLinearAttenuationCoefficient();
	double		getQuadraticAttenuationCoefficient();
	int getSuperSampling();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double			m_nAmbientLightRed;
	double			m_nAmbientLightGreen;
	double			m_nAmbientLightBlue;
	int		m_nAntialiasing;
	int         m_nJitter;
	double      m_nAdaptiveThreshold;
	double      m_nConstantAttenuationCoefficient;
	double      m_nLinearAttenuationCoefficient;
	double      m_nQuadraticAttenuationCoefficient;
	int m_nSuperSampling;
	
// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightRedSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightGreenSlides(Fl_Widget* o, void* v);
	static void cb_ambientLightBlueSlides(Fl_Widget* o, void* v);
	static void cb_antialiasingSlides(Fl_Widget* o, void* v);
	static void cb_adaptiveThresholdSlides(Fl_Widget* o, void* v);
	static void cb_jitterSlides(Fl_Widget* o, void* v);
	static void cb_constantAttenuationCoeffSlides(Fl_Widget* o, void* v);
	static void cb_linearAttenuationCoeffSlides(Fl_Widget* o, void* v);
	static void cb_quadraticAttenuationCoeffSlides(Fl_Widget* o, void* v);
	static void cb_superSamplingSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
