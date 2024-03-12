#include <ansi_c.h>
#include <advanlys.h>
#include <formatio.h>
#include <utility.h>
#include <cvirte.h>		
#include <userint.h>
#include "Proiect_Et1.h"


#define SAMPLE_RATE 0
#define NPOINTS 1

#define WINTYPE_TRIUNGHIULAR 0
#define WINTYPE_HANNING	1 		
	

int waveInfo[2];
//waveInfo[0]=sampleRate
//waveInfo[1]=number of elements

int bitMapID;
double sampleRate = 0.0;
int npoints = 0;

//vector->valori semnal audio
double *waveData = 0;

//vector-> valori filtrate semnal audio
double* filter=0;

static int panelHandle;
static int panelHandle2;



//Partea a doua
//Variabile

int N;

double df=0.0; //pasul in frecventa
double freakPeak=0.0; //val maxima din spectrul de putere(autoSpectrum)
double powerPeak=0.0; //frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
char unit[32]="V"; //voltage signal - descriere a semnalului achizitionat
static WindowConst winConst;
double *convertedSpectrum;//vectorul utilizat pentru reprezentarea spectrului
double *autoSpectrum;//spectrul de putere


int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Proiect_Et1.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "Proiect_Et1.uir", PANEL_2)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	 	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}



int CVICALLBACK OnExit (int panel, int event, void *callbackData,
						int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
			
		case EVENT_CLOSE:
			
			//activam butonul de inchidere a interfetei
			QuitUserInterface (0);
			
			break;
	}
	return 0;
}



int CVICALLBACK OnLoadButtonCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{	
	 int min_index=0;
	 double min=0.0;
	 int max_index=0;
	 double max=0.0;
	 double medie=0.0;
	 double dispersie=0.0;
	 double mediana=0.0;
	 int zero=0;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//executa script python pentru conversia unui fisierului .wav in .txt
			//LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf
			PlotY(panel, PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			//calcul valori minime si maxime alaturi de indecsii aferenti
			MaxMin1D(waveData, npoints, &max, &max_index, &min, &min_index);
			
			//calcul valoare medie
			Mean (waveData, npoints, &medie);
			
			//calcul dispersie
			Variance (waveData, npoints, &medie, &dispersie);
			
			//calcul mediana
			Median (waveData, npoints, &mediana);
			
			//calcul treceri prin zero
			for(int i=0; i<npoints-1;++i)
			{
				if( ((waveData[i]<0) && (waveData[i+1]>0)) || ((waveData[i]>0) && (waveData[i+1]<0)) || waveData[i]==0)
				{
					zero++;	
				}	
			}
			
			//Setam valorile calculate in conformitate cu butoanele specifice
			
			SetCtrlVal (panel, PANEL_MIN, min);
			SetCtrlVal (panel, PANEL_MAX, max);
			SetCtrlVal(panel, PANEL_INDEX_MIN, (double)min_index);
			SetCtrlVal (panel, PANEL_INDEX_MAX, (double)max_index);
			SetCtrlVal (panel, PANEL_MEDIE, medie);
			SetCtrlVal (panel, PANEL_DISPERSIE, dispersie);
			SetCtrlVal (panel, PANEL_Mediana, mediana);
			SetCtrlVal (panel, PANEL_ZERO, (double)zero);
			
			
			//Afisare histograma
			
			int histogram[100];
			double* ax;
			int lungime=25;
			
		
			ax=(double*)calloc(npoints,sizeof(double));
			
			Histogram (waveData, npoints, min-1, max+1, histogram, ax, lungime);
			DeleteGraphPlot (panel, PANEL_HISTO, -1, VAL_DELAYED_DRAW);
			PlotXY (panel, PANEL_HISTO, ax, histogram, lungime, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			

			break;
	}
	return 0;
}


double *relatieFiltrare(double alpha)
{	
	
	filter=(double*)calloc(npoints, sizeof(double));
	//filter[0]=0;
	for(int i=1; i<npoints;++i)
	{
		filter[i]=(1-alpha)*filter[i-1]+alpha*waveData[i];
	}
	
	return filter;
}	


int CVICALLBACK Selectie_Filtrare (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	
	int valoare_selectie;
	int dimensiune_fereastra;
	double alpha;
	//filter=(double*)calloc(npoints, sizeof(double));
	switch (event)
	 
	{
		case EVENT_COMMIT:
			
			GetCtrlVal (panel, PANEL_FILTRU, &valoare_selectie);
			
			//Filtrare Alpha
			if(valoare_selectie==0)
			{
				SetCtrlAttribute (panel, PANEL_DIM_FEREASTRA, ATTR_DIMMED, 1);
				SetCtrlAttribute (panel, PANEL_ALPHA, ATTR_DIMMED, 0);
				GetCtrlVal (panel, PANEL_ALPHA, &alpha);
				filter=relatieFiltrare(alpha);
				DeleteGraphPlot (panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				PlotY (panel, PANEL_GRAPH_2, filter, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_MAGENTA);
				
			}
			else
			{
				SetCtrlAttribute (panel, PANEL_DIM_FEREASTRA, ATTR_DIMMED, 0);
				SetCtrlAttribute (panel, PANEL_ALPHA, ATTR_DIMMED, 1);
				GetCtrlVal (panel, PANEL_DIM_FEREASTRA, &dimensiune_fereastra);
				if(dimensiune_fereastra==16)
				{
					MedianFilter (waveData, npoints, 16, 0, filter);
					DeleteGraphPlot (panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
					PlotY (panel, PANEL_GRAPH_2, filter, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_MAGENTA);
				}
				else
					if(dimensiune_fereastra==32)
					{
						MedianFilter (waveData, npoints, 32, 0, filter);
						DeleteGraphPlot (panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
						PlotY (panel, PANEL_GRAPH_2, filter, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_MAGENTA);
					}
			}
			
			break;
	}
	return 0;
}




int CVICALLBACK Prev_Next_Second (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	int start=0;
	int stop=0;
	double skewness=0.0;
	double kurtosis=0.0;
	double medie=0.0;
	double m=0.0;
	double n=0.0;
	//double alpha;
	switch (event)
	{	
		case EVENT_COMMIT:
			
		GetCtrlVal (panel, PANEL_START, &start);
		GetCtrlVal (panel, PANEL_STOP, &stop);
		//GetCtrlVal (panel, PANEL_ALPHA, &alpha);
		//filter=relatieFiltrare(alpha);
		switch(control)
		{
			case PANEL_NEXT_BUTTON:
				switch(event)
				{
					case EVENT_COMMIT:
						if(stop<6 && start+stop<npoints/6)
						{
							SetCtrlVal (panel, PANEL_START, ++start);
							SetCtrlVal (panel, PANEL_STOP, ++stop);
							
							
							int intervalStart=start*npoints/6;
							double* semnal=waveData+intervalStart;
							double* semnal_filtrat=filter+intervalStart;
							
							
							
							//Skewness
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								medie+=waveData[i];
							}
				
							medie=medie/256;
				
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								m+=pow((waveData[i]-medie),3);
							}
				
							m=m/256;
				
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								n+=pow((waveData[i]-medie),2);
							}
				
							n=n/256;
							n=pow(n,3./2);
				
							skewness=m/n;
							SetCtrlVal (panel, PANEL_SKEWNESS, skewness);
							
							//Kurtosis
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								medie+=waveData[i];
							}
				
							medie=medie/256;
				
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								m+=pow((waveData[i]-medie),4);
							}
				
							m=m/256;
				
							for(int i=sampleRate*start; i<sampleRate*start+256;i++)
							{
								n+=pow((waveData[i]-medie),2);
							}
				
							n=n/256;
							n=pow(n,2);
				
							kurtosis=m/n;
							SetCtrlVal (panel, PANEL_KURTOSIS, kurtosis);
				
				//ArrayToFile("test_filter.txt", filter, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER,  VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
				//ArrayToFile("test.txt", semnal_filtrat, VAL_DOUBLE, npoints/6, 1, VAL_GROUPS_TOGETHER,  VAL_GROUPS_AS_COLUMNS, VAL_CONST_WIDTH, 10, VAL_ASCII, VAL_TRUNCATE);
				DeleteGraphPlot (panel, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY (panel, PANEL_GRAPH, semnal, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			
				DeleteGraphPlot (panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				PlotY (panel, PANEL_GRAPH_2, semnal_filtrat, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			
						}
			
				break;

				}
				break;

			case PANEL_PREV_BUTTON:
				switch(event)
				{
					case EVENT_COMMIT:
						if(stop>1 && start+stop<npoints/6)
						{
							SetCtrlVal (panel, PANEL_START, --start);
							SetCtrlVal (panel, PANEL_STOP, --stop);
							
							int intervalStart=start*npoints/6;
							double* semnal=waveData+intervalStart;
							double* semnal_filtrat=filter+intervalStart;

							DeleteGraphPlot (panel, PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY (panel, PANEL_GRAPH, semnal, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_YELLOW);
			
							DeleteGraphPlot (panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
							PlotY (panel, PANEL_GRAPH_2, semnal_filtrat, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_YELLOW);
			
						}
						break;
						
				}
				break;
				
		}
		break;
		
	}
	return 0;
}




int CVICALLBACK Derivare (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
	
			if(npoints==0)
				return -1;
			
			double* v;
			v=(double*)calloc(npoints,sizeof(double));
			DifferenceEx (waveData, npoints, 1.0, waveData+1, 1, waveData+npoints-1, 1, DIFF_SECOND_ORDER_CENTRAL, v);
			
			PlotY(panel, PANEL_GRAPH_2, v, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);   
			
			break;
	}
	return 0;
}



int CVICALLBACK Anvelopa (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			//LaunchExecutable("python anvelopa.py");
			
			Delay(4);
			
			double* anvelopa;
			anvelopa = (double *) calloc(npoints, sizeof(double)); 
			
			FileToArray("anvelopa.txt", anvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			PlotY(panel,PANEL_GRAPH, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE); 
			PlotY(panel, PANEL_ANVELOP, anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_BLUE);
			PlotY(panel, PANEL_ANVELOP, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			break;
	}
	return 0;
}





int CVICALLBACK SaveImg (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	
	char semnalOriginal[256]={0};
	char semnalFiltrat[256]={0};
	
	switch (event)
	{
	
		case EVENT_COMMIT:
		
			sprintf(semnalOriginal, "grafic_nefiltrat.jpeg");
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH, 1, &bitMapID);
			SaveBitmapToJPEGFile(bitMapID, semnalOriginal, JPEG_PROGRESSIVE, 100);

			sprintf(semnalFiltrat, "graf_filtrat.jpeg");
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH_2, 1, &bitMapID);
			SaveBitmapToJPEGFile(bitMapID, semnalFiltrat, JPEG_PROGRESSIVE, 100);
			
			break;
	}
	return 0;
}

int CVICALLBACK SwitchPanel (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == panelHandle)
			{
				SetCtrlVal(panelHandle2, PANEL_2_SWITCH_PANEL_2, 1);
				DisplayPanel(panelHandle2);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(panelHandle, PANEL_SWITCH_PANEL_1, 0);
				DisplayPanel(panelHandle);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}


int CVICALLBACK Load (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			//executa script python pentru conversia unui fisierului .wav in .txt
			//LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf a semnalului initial (total)
			PlotY(panel, PANEL_2_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			
			//preluam de pe interfata numarul de esantioane necesar ferastruirii si filtrarii
			GetCtrlVal(panel, PANEL_2_IDC_NR_PUNCTE, &N);
			
			//alocare memorie pentru numarul de puncte
			autoSpectrum=(double*)calloc(N/2,sizeof(double));//spectrul de putere
			convertedSpectrum=(double*)calloc(N/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
			
			ScaledWindowEx (waveData, N, RECTANGLE_, 0, &winConst);
				
			//se calculeaza partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
			AutoPowerSpectrum(waveData, N, 1.0/sampleRate, autoSpectrum, &df);
				
			//functia returneaza frequencyPeak si powerPeak
			PowerFrequencyEstimate (autoSpectrum, N/2, -1.0, winConst, df, 7, &freakPeak, &powerPeak);
				
			//se converteste spectrul de intrare în formate ce permit o reprezentare grafica mai convenabila
			SpectrumUnitConversion(autoSpectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
				
			//afisam spectrul pe grafic
			PlotWaveform(panel,  PANEL_2_SPECTRUM, convertedSpectrum, N /2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_RED);		
				
			//afisam pe interfata valorile pentru frequencyPeak si powerPeak
			SetCtrlVal(panel, PANEL_2_IDC_FREQ_PEAK, freakPeak);
			SetCtrlVal(panel, PANEL_2_IDC_POWER_PEAK, powerPeak);
			
	
			break;
	}
	return 0;
}

int CVICALLBACK Aply (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{	
	int s;
	double *v;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			//Extrag de pe interfata secunda curenta 
			GetCtrlVal(panel,PANEL_2_IDC_SECUNDA,&s);
			
			//preluam de pe interfata numarul de esantioane necesar ferastruirii si filtrarii
			GetCtrlVal(panel, PANEL_2_IDC_NR_PUNCTE, &N);
			
			v=(double*)calloc(N,sizeof(double));
			for(int i=0;i<N;i++)
			{
				if(s*npoints/6+i<npoints)
				{
					v[i]=waveData[s*npoints/6+i];
				}
				else
				{
					v[i]=0;
				}
			}
			
			//afisez semnal initial pe secunde
			DeleteGraphPlot (panel, PANEL_2_DATA_SECOND, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_DATA_SECOND, v, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			
			//afisare spectrum pe secunde
			
			//alocare memorie pentru numarul de puncte
			autoSpectrum=(double*)calloc(N/2,sizeof(double));//spectrul de putere
			convertedSpectrum=(double*)calloc(N/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului


			//se calculeaza partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
			AutoPowerSpectrum(v, N, 1.0/sampleRate, autoSpectrum, &df);
				
			//functia returneaza frequencyPeak si powerPeak
			PowerFrequencyEstimate (autoSpectrum, N/2, -1.0, winConst, df, 7, &freakPeak, &powerPeak);
				
			//se converteste spectrul de intrare în formate ce permit o reprezentare grafica mai convenabila
			SpectrumUnitConversion(autoSpectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
				
			//afisam spectrul pe grafic
			PlotWaveform(panel,  PANEL_2_SPECTRUM_SECOND, convertedSpectrum, N /2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_RED);		
				
			//afisam pe interfata valorile pentru frequencyPeak si powerPeak
			SetCtrlVal(panel, PANEL_2_IDC_FREQ_PEAK, freakPeak);
			SetCtrlVal(panel, PANEL_2_IDC_POWER_PEAK, powerPeak);
			
			
			break;
			
		}
	return 0;
}



int CVICALLBACK modify (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	int s;
	double *v;
	double *v2;
	
	int fereastra; //var folosita pt alegere tip fereastra
	
	double *window; 
	window=(double*)calloc(N,sizeof(double));
	
	double *data_ferestruita; 
	data_ferestruita=(double*)calloc(N,sizeof(double));
	
	
	int filtru;  //var folosita pt alegere tip filtru
	
	double *filtredSignal;
	filtredSignal = (double *) calloc(N, sizeof(double));
		
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			
			GetCtrlVal(panel,PANEL_2_IDC_SECUNDA,&s);
			GetCtrlVal(panel,PANEL_2_IDC_FEREASTRA,&fereastra);
			GetCtrlVal(panel,PANEL_2_IDC_FILTRU, &filtru);
			
			
			
			v=(double*)calloc(N,sizeof(double));
			for(int i=0;i<N;i++)
			{
				if(s*npoints/6+i<npoints)
				{
					v[i]=waveData[s*npoints/6+i];
				}
				else
				{
					v[i]=0;
				}
			}
			
			switch (fereastra)
			{
				case WINTYPE_HANNING:
					
					LinEv1D(v,N,0,1,window);
					HanWin (window, N);
					break;
					
				case WINTYPE_TRIUNGHIULAR:
					
					LinEv1D(v,N,0,1,window);
					TriWin (window, N);
					break;
					
			}
			
			//afisare tip fereastra
			
			DeleteGraphPlot (panel, PANEL_2_WINDOW, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_WINDOW, window, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
			//afisare semnal ferastruit
			
			Mul1D(v,window,N,data_ferestruita);
			DeleteGraphPlot (panel,PANEL_2_WINDOW_2, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel,PANEL_2_WINDOW_2, data_ferestruita, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_MAGENTA);
				
			v2=(double*)calloc(N,sizeof(double));
			for(int i=0;i<N;i++)
			{
				if(s*npoints/6+i<npoints)
				{
					v2[i]=waveData[s*npoints/6+i];
				}
				else
				{
					v2[i]=0;
				}
			}
			
			switch (filtru)
			{
				case 0:
					
					Bw_HPF (v2, N, sampleRate, sampleRate/4, 5, filtredSignal);
					
					break;
					
				case 1:
					Ch_BPF (v2, N, sampleRate, sampleRate/8, sampleRate/4, 0.1, 5, filtredSignal);
					
					break;
			}
			
			//afisare (doar) semnal filtrat
			DeleteGraphPlot (panel, PANEL_2_GRAPH_FILTRED_2, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_GRAPH_FILTRED_2, filtredSignal, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			
			
			//afisare semnal ferastruit+filtrat
			//pe semnal filtrat aplic fereastra
			
			double *finish_data; 
			finish_data=(double*)calloc(N,sizeof(double));
			
			Mul1D(filtredSignal,window,N,finish_data);
			DeleteGraphPlot (panel, PANEL_2_GRAPH_FILTRED, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_GRAPH_FILTRED, finish_data, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			
			
			break;
	}
	return 0;
}


int CVICALLBACK spectrum (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	
	int s;
	double *v;
	double *v2;
	
	int fereastra; //var folosita pt alegere tip fereastra
	int filtru;    //var folosita pt alegere tip filtru
	
	double *window; 
	window=(double*)calloc(N,sizeof(double));
	
	double *data_ferestruita; 
	data_ferestruita=(double*)calloc(N,sizeof(double));
	
	
	double *filtredSignal;
	filtredSignal = (double *) calloc(N, sizeof(double));
	
	//alocare memorie pentru numarul de puncte
	autoSpectrum=(double*)calloc(N/2,sizeof(double));//spectrul de putere
	convertedSpectrum=(double*)calloc(N/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
			
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panel,PANEL_2_IDC_SECUNDA,&s);
			GetCtrlVal(panel,PANEL_2_IDC_FEREASTRA,&fereastra);
			GetCtrlVal(panel,PANEL_2_IDC_FILTRU, &filtru);
			
			
			v=(double*)calloc(N,sizeof(double));
			for(int i=0;i<N;i++)
			{
				if(s*npoints/6+i<npoints)
				{
					v[i]=waveData[s*npoints/6+i];
				}
				else
				{
					v[i]=0;
				}
			}
			
			switch (fereastra)
			{
				case WINTYPE_HANNING:
					
					LinEv1D(v,N,0,1,window);
					HanWin (window, N);
					break;
					
				case WINTYPE_TRIUNGHIULAR:
					
					LinEv1D(v,N,0,1,window);
					TriWin (window, N);
					break;
					
			}
			
			Mul1D(v,window,N,data_ferestruita);
		
			//Afisare spectru dupa ferestruire
			
			AutoPowerSpectrum (data_ferestruita, N, 1.0/sampleRate, autoSpectrum, &df);
			PowerFrequencyEstimate (autoSpectrum, N/2, -1, winConst, df, 7, &freakPeak,&powerPeak);
			SpectrumUnitConversion(autoSpectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst,convertedSpectrum, unit);
			
			DeleteGraphPlot (panel, PANEL_2_SPECTRUM_WINDOW, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_SPECTRUM_WINDOW, convertedSpectrum, N/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_MAGENTA);
			
			
			SetCtrlVal(panel, PANEL_2_IDC_FREQ_PEAK, freakPeak);
			SetCtrlVal(panel, PANEL_2_IDC_POWER_PEAK, powerPeak);
			
			
			v2=(double*)calloc(N,sizeof(double));
			for(int i=0;i<N;i++)
			{
				if(s*npoints/6+i<npoints)
				{
					v2[i]=waveData[s*npoints/6+i];
				}
				else
				{
					v2[i]=0;
				}
			}
			
			switch (filtru)
			{
				case 0:
					
					Bw_HPF (v2, N, sampleRate, sampleRate/4, 5, filtredSignal);
					
					break;
					
				case 1:
					Ch_BPF (v2, N, sampleRate, sampleRate/8, sampleRate/4, 0.1, 5, filtredSignal);
					
					break;
			}
			
			
			AutoPowerSpectrum (filtredSignal, N, 1.0/sampleRate, autoSpectrum, &df);
			SpectrumUnitConversion(autoSpectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst,convertedSpectrum, unit);
			
			//afisare spectru doar pt semnal filtrat 
			DeleteGraphPlot (panel, PANEL_2_SPECTRUM_FILTRED_2, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_SPECTRUM_FILTRED_2, convertedSpectrum ,N/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			
			
			double *finish_data; 
			finish_data=(double*)calloc(N,sizeof(double));
			
			 Mul1D(filtredSignal,window,N,finish_data);
			
			AutoPowerSpectrum (finish_data, N, 1.0/sampleRate, autoSpectrum, &df);
			SpectrumUnitConversion(autoSpectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst,convertedSpectrum, unit);
			
			//afisare spectru pt semnal filtrat si ferastruit
			DeleteGraphPlot (panel, PANEL_2_SPECTRUM_FILTRED, -1, VAL_IMMEDIATE_DRAW);
			PlotY (panel, PANEL_2_SPECTRUM_FILTRED, convertedSpectrum ,N/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			break;
	}
	return 0;
	
}

int CVICALLBACK Save (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	int imgHandle;  
	
	char fileName[256] = {0};
	char fileName1[256]={0};
	char fileName2[256]={0};
	char fileName3[256]={0};
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetSystemDate(&month, &day, &year);
			GetSystemTime(&hour, &minute, &second);
			
				
			sprintf(fileName, "SEMNAL_INITIAL_%2d.%2d.%4d__%2d_%2d_%2d.jpg", day, month, year, hour, minute, second);
			GetCtrlDisplayBitmap(panel,PANEL_2_DATA_SECOND,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName,JPEG_PROGRESSIVE,100);
			DiscardBitmap(imgHandle);
			
			sprintf(fileName1, "SEMNAL_FILTRAT_%2d.%2d.%4d__%2d_%2d_%2d.jpg", day, month, year, hour, minute, second);
			GetCtrlDisplayBitmap(panel,PANEL_2_GRAPH_FILTRED_2,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName1,JPEG_PROGRESSIVE,100);
			DiscardBitmap(imgHandle);
			
			
			sprintf(fileName2,"SPECTRU_S_INITIAL_%2d.%2d.%4d__%2d_%2d_%2d.jpg", day, month, year, hour, minute, second);
			GetCtrlDisplayBitmap(panel,PANEL_2_SPECTRUM_SECOND,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName2,JPEG_PROGRESSIVE,100);
			DiscardBitmap(imgHandle);
			
			
			sprintf(fileName3, "SEMNAL_S_FILTRAT_%2d.%2d.%4d__%2d_%2d_%2d.jpg", day, month, year, hour, minute, second);
			GetCtrlDisplayBitmap(panel,PANEL_2_SPECTRUM_FILTRED,1,&imgHandle);
			SaveBitmapToJPEGFile(imgHandle,fileName3,JPEG_PROGRESSIVE,100);
			DiscardBitmap(imgHandle);
			
			break;
	}
	return 0;
}
